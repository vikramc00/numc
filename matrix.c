#include "matrix.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#include <x86intrin.h>
#endif

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set1_pd (double a)
 * __m256d _mm256_set_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
*/

/*
 * Generates a random double between `low` and `high`.
 */
double rand_double(double low, double high) {
    double range = (high - low);
    double div = RAND_MAX / range;
    return low + (rand() / div);
}

/*
 * Generates a random matrix with `seed`.
 */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
    srand(seed);
    for (int i = 0; i < result->rows; i++) {
        for (int j = 0; j < result->cols; j++) {
            set(result, i, j, rand_double(low, high));
        }
    }
}

/*
 * Allocate space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data array
 * and initialize all entries to be zeros. Remember to set all fieds of the matrix struct.
 * `parent` should be set to NULL to indicate that this matrix is not a slice.
 * You should return -1 if either `rows` or `cols` or both have invalid values, or if any
 * call to allocate memory in this function fails. If you don't set python error messages here upon
 * failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix(matrix **mat, int rows, int cols) {
    /* TODO: YOUR CODE HERE */
    if (rows < 1 || cols < 1) {
        return -1;
    }
    *mat = malloc(sizeof(matrix));
    if ((*mat) == NULL) {
        return -1;
    }
    matrix *mat_p = *mat;
    mat_p->rows = rows;
    mat_p->cols = cols;
    mat_p->ref_cnt = 1;
    mat_p->is_slice = 0;
    if (rows == 1 || cols == 1) {
        mat_p->is_1d = 1;
    } else {
        mat_p->is_1d = 0;
    }
    mat_p->parent = NULL;
    mat_p->data = malloc(rows * sizeof(double*));
    if (mat_p->data == NULL) {
        free(mat_p);
        return -1;
    }
    double *p = calloc(rows * cols, sizeof(double));
    if (p == NULL) {
        free(mat_p->data);
        free(mat_p);
        return -1;
    }
    int stop = (rows / 4) * 4;
    #pragma omp parallel for if(rows > 5000)
    for (int i = 0; i < stop; i+=4) {
        mat_p->data[i] = p + cols * i;
        mat_p->data[i + 1] = p + cols * (i + 1);
        mat_p->data[i + 2] = p + cols * (i + 2);
        mat_p->data[i + 3] = p + cols * (i + 3);
    }
    for (int i = stop; i < rows; i++) {
        mat_p->data[i] = p + cols * i;
    }
    return 0;
}

/*
 * Allocate space for a matrix struct pointed to by `mat` with `rows` rows and `cols` columns.
 * This is equivalent to setting the new matrix to be
 * from[row_offset:row_offset + rows, col_offset:col_offset + cols]
 * If you don't set python error messages here upon failure, then remember to set it in numc.c.
 * Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int row_offset, int col_offset,
                        int rows, int cols) {
    /* TODO: YOUR CODE HERE */
    if (rows < 1 || cols < 1) {
        return -1;
    }
    matrix *mat_p = *mat;
    mat_p = malloc(sizeof(matrix));
    if (mat_p == NULL) {
        return -1;
    }
    mat_p->rows = rows;
    mat_p->cols = cols;
    if (rows == 1 || cols == 1) {
        mat_p->is_1d = 1;
    } else {
        mat_p->is_1d = 0;
    }
    mat_p->ref_cnt = 1;
    mat_p->is_slice = 1;
    matrix *root_mat = from;
    while ((root_mat->parent) != NULL) {
        root_mat = root_mat->parent;
    }
    root_mat->ref_cnt += 1;
    mat_p->parent = root_mat;
    mat_p->data = malloc(rows * sizeof(double*));
    if (mat_p->data == NULL) {
        free(mat_p);
        return -1;
    }
    int stop = rows / 4 * 4;
    #pragma omp parallel for
    for (int i = 0; i < stop; i += 4) {
        (mat_p->data)[i] = (from->data)[row_offset + i] + col_offset;
        (mat_p->data)[i + 1] = (from->data)[row_offset + i + 1] + col_offset;
        (mat_p->data)[i + 2] = (from->data)[row_offset + i + 2] + col_offset;
        (mat_p->data)[i + 3] = (from->data)[row_offset + i + 3] + col_offset;
    }
    for (int i = stop; i < rows; i++) {
        (mat_p->data)[i] = (from->data)[row_offset + i] + col_offset;
    }
    *mat = mat_p;
    return 0;
}

/*
 * This function will be called automatically by Python when a numc matrix loses all of its
 * reference pointers.
 * You need to make sure that you only free `mat->data` if no other existing matrices are also
 * referring this data array.
 * See the spec for more information.
 */
void deallocate_matrix(matrix *mat) {
    /* TODO: YOUR CODE HERE */
    if (mat) {
        if (mat->parent != NULL) {
            matrix *root_mat = mat;
            while ((root_mat->parent) != NULL) {
                root_mat = root_mat->parent;
            }
            root_mat->ref_cnt -= 1;
        }
        if (mat->ref_cnt <= 1 && (mat->parent) == NULL) {
            free(mat->data[0]);
        }
        free(mat->data);
        free(mat);
    }
}

/*
 * Return the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid.
 */
double get(matrix *mat, int row, int col) {
    /* TODO: YOUR CODE HERE */
    return (mat->data)[row][col];
}

/*
 * Set the value at the given row and column to val. You may assume `row` and
 * `col` are valid
 */
void set(matrix *mat, int row, int col, double val) {
    /* TODO: YOUR CODE HERE */
    (mat->data)[row][col] = val;
}

/*
 * Set all entries in mat to val
 */
void fill_matrix(matrix *mat, double val) {
    /* TODO: YOUR CODE HERE */
    int num_elems = mat->rows * mat->cols;
    int num_elems_stop = num_elems / 4 * 4;
    #pragma omp parallel for if(num_elems > 5000)
    for (int i = 0; i < num_elems_stop; i+=4) {
        (mat->data)[0][i] = val;
        (mat->data)[0][i+1] = val;
        (mat->data)[0][i+2] = val;
        (mat->data)[0][i+3] = val;
    }
    for (int i = num_elems_stop; i < num_elems; i++) {
        (mat->data)[0][i] = val;
    }
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if ((mat1->rows != mat2->rows) || (mat1->cols != mat2->cols)) {
        return -1;
    } else if ((mat1->rows != result->rows) || (mat1->cols != result->cols)) {
        return -1;
    }
    double **result_data = result->data;
    double **mat1_data = mat1->data;
    double **mat2_data = mat2->data;
    #pragma omp parallel for if (result->rows * result->cols > 500000)
    for (int i = 0; i < mat1->rows; i++) {
        int stop = (mat1->cols) / 4 * 4;
        for (int j = 0; j < stop; j+=4) {
            result_data[i][j] = mat1_data[i][j] + mat2_data[i][j];
            result_data[i][j+1] = mat1_data[i][j+1] + mat2_data[i][j+1];
            result_data[i][j+2] = mat1_data[i][j+2] + mat2_data[i][j+2];
            result_data[i][j+3] = mat1_data[i][j+3] + mat2_data[i][j+3];
        }
        for (int j = stop; j < mat1->cols; j++) {
            result_data[i][j] = mat1_data[i][j] + mat2_data[i][j];
        }
    }
    return 0;
}

/*
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if ((mat1->rows != mat2->rows) || (mat1->cols != mat2->cols)) {
        return -1;
    } else if ((mat1->rows != result->rows) || (mat1->cols != result->cols)) {
        return -1;
    }
    double **result_data = result->data;
    double **mat1_data = mat1->data;
    double **mat2_data = mat2->data;
    #pragma omp parallel for if (result->rows * result->cols > 500000)
    for (int i = 0; i < mat1->rows; i++) {
        int stop = (mat1->cols) / 4 * 4;
        for (int j = 0; j < stop; j+=4) {
            result_data[i][j] = mat1_data[i][j] - mat2_data[i][j];
            result_data[i][j+1] = mat1_data[i][j+1] - mat2_data[i][j+1];
            result_data[i][j+2] = mat1_data[i][j+2] - mat2_data[i][j+2];
            result_data[i][j+3] = mat1_data[i][j+3] - mat2_data[i][j+3];
        }
        for (int j = stop; j < mat1->cols; j++) {
            result_data[i][j] = mat1_data[i][j] - mat2_data[i][j];
        }
    }
    return 0;
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that matrix multiplication is not the same as multiplying individual elements.
 */
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {
    /* TODO: YOUR CODE HERE */
    if (mat1->cols != mat2->rows) {
        return -1;
    } else if (mat1->rows != result->rows || mat2->cols != result->cols) {
        return -1;
    }
    double **result_data = result->data;
    int num_elems = result->rows * result->cols;
    int num_elems_stop = num_elems / 4 * 4;
    #pragma omp parallel for if(num_elems > 5000)
    for (int i = 0; i < num_elems_stop; i+=4) {
        result_data[0][i] = 0.0;
        result_data[0][i+1] = 0.0;
        result_data[0][i+2] = 0.0;
        result_data[0][i+3] = 0.0;
    }
    for (int i = num_elems_stop; i < num_elems; i++) {
        result_data[0][i] = 0.0;
    }
    double **mat1_data = mat1->data;
    double **mat2_data = mat2->data;
    int stop = (mat1->cols) / 4 * 4;
    #pragma omp parallel for if(result->rows * result->cols > 2000)
    for (int i = 0; i < mat1->rows; i++) {
        for (int k = 0; k < stop; k+=4) {
            for (int j = 0; j < mat2->cols; j++) {
                result_data[i][j] += mat1_data[i][k] * mat2_data[k][j];
                result_data[i][j] += mat1_data[i][k+1] * mat2_data[k+1][j];
                result_data[i][j] += mat1_data[i][k+2] * mat2_data[k+2][j];
                result_data[i][j] += mat1_data[i][k+3] * mat2_data[k+3][j];
            }
        }
        for (int k = stop; k < mat1->cols; k++) {
            for (int j = 0; j < mat2->cols; j++) {
                result_data[i][j] += mat1_data[i][k] * mat2_data[k][j];
            }
        }
    }
    return 0;
}

/*
 * Store the result of raising mat to the (pow)th power to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that pow is defined with matrix multiplication, not element-wise multiplication.
 */
int pow_matrix(matrix *result, matrix *mat, int pow) {
    /* TODO: YOUR CODE HERE */
    if (mat->rows != result->rows || mat->cols != result->cols || mat->rows != mat->cols) {
        return -1;
    }
    int stop = mat->cols;
    int num_elems = mat->rows * mat->cols;
    int num_elems_stop = num_elems / 4 * 4;
    matrix *temp_x = NULL;
    if (allocate_matrix(&temp_x, result->rows, result->cols) != 0) {
        return -1;
    }
    omp_set_num_threads(4);
    if (mat->is_slice) {
        #pragma omp parallel for if(result->rows * result->cols > 5000)
        for (int i = 0; i < result->rows; i++) {
            for (int j = 0; j < stop; j+=4) {
                (temp_x->data)[i][j] = (mat->data)[i][j];
                (temp_x->data)[i][j+1] = (mat->data)[i][j+1];
                (temp_x->data)[i][j+2] = (mat->data)[i][j+2];
                (temp_x->data)[i][j+3] = (mat->data)[i][j+3];
            }
            for (int j = stop; j < result->cols; j++) {
                (temp_x->data)[i][j] = (mat->data)[i][j];
            }
        }
        #pragma omp parallel for if(result->rows * result->cols > 5000)
        for (int i = 0; i < num_elems_stop; i += 4) {
            if (0 == i % ((mat->cols) + 1)) {
                (result->data)[0][i] = 1.0;
            }
            if (0 == (i + 1) % ((mat->cols) + 1)) {
                (result->data)[0][i + 1] = 1.0;
            }
            if (0 == (i + 2) % ((mat->cols) + 1)) {
                (result->data)[0][i + 2] = 1.0;
            }
            if (0 == (i + 3) % ((mat->cols) + 1)) {
                (result->data)[0][i + 3] = 1.0;
            }
        }
        for (int i = num_elems_stop; i < num_elems; i++) {
            if (0 == i % ((mat->cols) + 1)) {
                (result->data)[0][i] = 1.0;
            }
        }
    } else {
        #pragma omp parallel for if(result->rows * result->cols > 5000)
        for (int i = 0; i < num_elems_stop; i += 4) {
            (temp_x->data)[0][i] = (mat->data)[0][i];
            (temp_x->data)[0][i + 1] = (mat->data)[0][i + 1];
            (temp_x->data)[0][i + 2] = (mat->data)[0][i + 2];
            (temp_x->data)[0][i + 3] = (mat->data)[0][i + 3];
            if (0 == i % ((mat->cols) + 1)) {
                (result->data)[0][i] = 1.0;
            }
            if (0 == (i + 1) % ((mat->cols) + 1)) {
                (result->data)[0][i + 1] = 1.0;
            }
            if (0 == (i + 2) % ((mat->cols) + 1)) {
                (result->data)[0][i + 2] = 1.0;
            }
            if (0 == (i + 3) % ((mat->cols) + 1)) {
                (result->data)[0][i + 3] = 1.0;
            }
        }
        for (int i = num_elems_stop; i < num_elems; i++) {
            (temp_x->data)[0][i] = (mat->data)[0][i];
            if (0 == i % ((mat->cols) + 1)) {
                (result->data)[0][i] = 1.0;
            }
        }
    }
    matrix *temp_mul = NULL;
    if (allocate_matrix(&temp_mul, result->rows, result->cols) != 0) {
        return -1;
    }
    while (pow > 0) {
        if (pow % 2 == 1) {
            #pragma omp parallel for if(result->rows * result->cols > 5000)
            for (int j = 0; j < num_elems_stop; j+=4) {
                (temp_mul->data)[0][j] = (result->data)[0][j];
                (temp_mul->data)[0][j+1] = (result->data)[0][j+1];
                (temp_mul->data)[0][j+2] = (result->data)[0][j+2];
                (temp_mul->data)[0][j+3] = (result->data)[0][j+3];
            }
            for (int j = num_elems_stop; j < num_elems; j++) {
                (temp_mul->data)[0][j] = (result->data)[0][j];
            }
            mul_matrix(result, temp_mul, temp_x);
            pow--;
        } else {
            #pragma omp parallel for if(result->rows * result->cols > 5000)
            for (int j = 0; j < num_elems_stop; j+=4) {
                (temp_mul->data)[0][j] = (temp_x->data)[0][j];
                (temp_mul->data)[0][j+1] = (temp_x->data)[0][j+1];
                (temp_mul->data)[0][j+2] = (temp_x->data)[0][j+2];
                (temp_mul->data)[0][j+3] = (temp_x->data)[0][j+3];
            }
            for (int j = num_elems_stop; j < num_elems; j++) {
                (temp_mul->data)[0][j] = (temp_x->data)[0][j];
            }
            mul_matrix(temp_x, temp_mul, temp_mul);
            pow = pow/2;
        }
    }
    deallocate_matrix(temp_x);
    deallocate_matrix(temp_mul);
    omp_set_num_threads(8);
    return 0;
}

/*
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int neg_matrix(matrix *result, matrix *mat) {
    /* TODO: YOUR CODE HERE */
    if (mat->rows != result->rows || mat->cols != result->cols) {
        return -1;
    }
    double **result_data = result->data;
    double **mat_data = mat->data;
    #pragma omp parallel for if(result->rows * result->cols > 500000)
    for (int i = 0; i < mat->rows; i++) {
        int stop = (mat->cols) / 4 * 4;
        for (int j = 0; j < stop; j+=4) {
            result_data[i][j] = 0.0 - mat_data[i][j];
            result_data[i][j+1] = 0.0 - mat_data[i][j+1];
            result_data[i][j+2] = 0.0 - mat_data[i][j+2];
            result_data[i][j+3] = 0.0 - mat_data[i][j+3];
        }
        for (int j = stop; j < mat->cols; j++) {
            result_data[i][j] = 0.0 - mat_data[i][j];
        }
    }
    return 0;
}

/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int abs_matrix(matrix *result, matrix *mat) {
    /* TODO: YOUR CODE HERE */
    if (mat->rows != result->rows || mat->cols != result->cols) {
        return -1;
    }
    double **result_data = result->data;
    double **mat_data = mat->data;
    #pragma omp parallel for if(result->rows * result->cols > 500000)
    for (int i = 0; i < mat->rows; i++) {
        int stop = (mat->cols) / 4 * 4;
        for (int j = 0; j < stop; j+=4) {
            result_data[i][j] = fabs(mat_data[i][j]);
            result_data[i][j+1] = fabs(mat_data[i][j+1]);
            result_data[i][j+2] = fabs(mat_data[i][j+2]);
            result_data[i][j+3] = fabs(mat_data[i][j+3]);
        }
        for (int j = stop; j < mat->cols; j++) {
            result_data[i][j] = fabs(mat_data[i][j]);
        }
    }
    return 0;
}

