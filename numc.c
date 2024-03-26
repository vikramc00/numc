#include "numc.h"
#include <structmember.h>

PyTypeObject Matrix61cType;

/* Helper functions for initalization of matrices and vectors */

/*
 * Return a tuple given rows and cols
 */
PyObject *get_shape(int rows, int cols) {
  if (rows == 1 || cols == 1) {
    return PyTuple_Pack(1, PyLong_FromLong(rows * cols));
  } else {
    return PyTuple_Pack(2, PyLong_FromLong(rows), PyLong_FromLong(cols));
  }
}
/*
 * Matrix(rows, cols, low, high). Fill a matrix random double values
 */
int init_rand(PyObject *self, int rows, int cols, unsigned int seed, double low,
              double high) {
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    rand_matrix(new_mat, seed, low, high);
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(rows, cols, val). Fill a matrix of dimension rows * cols with val
 */
int init_fill(PyObject *self, int rows, int cols, double val) {
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed)
        return alloc_failed;
    else {
        fill_matrix(new_mat, val);
        ((Matrix61c *)self)->mat = new_mat;
        ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    }
    return 0;
}

/*
 * Matrix(rows, cols, 1d_list). Fill a matrix with dimension rows * cols with 1d_list values
 */
int init_1d(PyObject *self, int rows, int cols, PyObject *lst) {
    if (rows * cols != PyList_Size(lst)) {
        PyErr_SetString(PyExc_ValueError, "Incorrect number of elements in list");
        return -1;
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    int count = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set(new_mat, i, j, PyFloat_AsDouble(PyList_GetItem(lst, count)));
            count++;
        }
    }
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * Matrix(2d_list). Fill a matrix with dimension len(2d_list) * len(2d_list[0])
 */
int init_2d(PyObject *self, PyObject *lst) {
    int rows = PyList_Size(lst);
    if (rows == 0) {
        PyErr_SetString(PyExc_ValueError,
                        "Cannot initialize numc.Matrix with an empty list");
        return -1;
    }
    int cols;
    if (!PyList_Check(PyList_GetItem(lst, 0))) {
        PyErr_SetString(PyExc_ValueError, "List values not valid");
        return -1;
    } else {
        cols = PyList_Size(PyList_GetItem(lst, 0));
    }
    for (int i = 0; i < rows; i++) {
        if (!PyList_Check(PyList_GetItem(lst, i)) ||
                PyList_Size(PyList_GetItem(lst, i)) != cols) {
            PyErr_SetString(PyExc_ValueError, "List values not valid");
            return -1;
        }
    }
    matrix *new_mat;
    int alloc_failed = allocate_matrix(&new_mat, rows, cols);
    if (alloc_failed) return alloc_failed;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            set(new_mat, i, j,
                PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(lst, i), j)));
        }
    }
    ((Matrix61c *)self)->mat = new_mat;
    ((Matrix61c *)self)->shape = get_shape(new_mat->rows, new_mat->cols);
    return 0;
}

/*
 * This deallocation function is called when reference count is 0
 */
void Matrix61c_dealloc(Matrix61c *self) {
    deallocate_matrix(self->mat);
    Py_TYPE(self)->tp_free(self);
}

/* For immutable types all initializations should take place in tp_new */
PyObject *Matrix61c_new(PyTypeObject *type, PyObject *args,
                        PyObject *kwds) {
    /* size of allocated memory is tp_basicsize + nitems*tp_itemsize*/
    Matrix61c *self = (Matrix61c *)type->tp_alloc(type, 0);
    return (PyObject *)self;
}

/*
 * This matrix61c type is mutable, so needs init function. Return 0 on success otherwise -1
 */
int Matrix61c_init(PyObject *self, PyObject *args, PyObject *kwds) {
    /* Generate random matrices */
    if (kwds != NULL) {
        PyObject *rand = PyDict_GetItemString(kwds, "rand");
        if (!rand) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (!PyBool_Check(rand)) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
        if (rand != Py_True) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        PyObject *low = PyDict_GetItemString(kwds, "low");
        PyObject *high = PyDict_GetItemString(kwds, "high");
        PyObject *seed = PyDict_GetItemString(kwds, "seed");
        double double_low = 0;
        double double_high = 1;
        unsigned int unsigned_seed = 0;

        if (low) {
            if (PyFloat_Check(low)) {
                double_low = PyFloat_AsDouble(low);
            } else if (PyLong_Check(low)) {
                double_low = PyLong_AsLong(low);
            }
        }

        if (high) {
            if (PyFloat_Check(high)) {
                double_high = PyFloat_AsDouble(high);
            } else if (PyLong_Check(high)) {
                double_high = PyLong_AsLong(high);
            }
        }

        if (double_low >= double_high) {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }

        // Set seed if argument exists
        if (seed) {
            if (PyLong_Check(seed)) {
                unsigned_seed = PyLong_AsUnsignedLong(seed);
            }
        }

        PyObject *rows = NULL;
        PyObject *cols = NULL;
        if (PyArg_UnpackTuple(args, "args", 2, 2, &rows, &cols)) {
            if (rows && cols && PyLong_Check(rows) && PyLong_Check(cols)) {
                return init_rand(self, PyLong_AsLong(rows), PyLong_AsLong(cols), unsigned_seed, double_low,
                                 double_high);
            }
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    }
    PyObject *arg1 = NULL;
    PyObject *arg2 = NULL;
    PyObject *arg3 = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 3, &arg1, &arg2, &arg3)) {
        /* arguments are (rows, cols, val) */
        if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && (PyLong_Check(arg3)
                || PyFloat_Check(arg3))) {
            if (PyLong_Check(arg3)) {
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyLong_AsLong(arg3));
            } else
                return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), PyFloat_AsDouble(arg3));
        } else if (arg1 && arg2 && arg3 && PyLong_Check(arg1) && PyLong_Check(arg2) && PyList_Check(arg3)) {
            /* Matrix(rows, cols, 1D list) */
            return init_1d(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), arg3);
        } else if (arg1 && PyList_Check(arg1) && arg2 == NULL && arg3 == NULL) {
            /* Matrix(rows, cols, 1D list) */
            return init_2d(self, arg1);
        } else if (arg1 && arg2 && PyLong_Check(arg1) && PyLong_Check(arg2) && arg3 == NULL) {
            /* Matrix(rows, cols, 1D list) */
            return init_fill(self, PyLong_AsLong(arg1), PyLong_AsLong(arg2), 0);
        } else {
            PyErr_SetString(PyExc_TypeError, "Invalid arguments");
            return -1;
        }
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return -1;
    }
}

/*
 * List of lists representations for matrices
 */
PyObject *Matrix61c_to_list(Matrix61c *self) {
    int rows = self->mat->rows;
    int cols = self->mat->cols;
    PyObject *py_lst = NULL;
    if (self->mat->is_1d) {  // If 1D matrix, print as a single list
        py_lst = PyList_New(rows * cols);
        int count = 0;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                PyList_SetItem(py_lst, count, PyFloat_FromDouble(get(self->mat, i, j)));
                count++;
            }
        }
    } else {  // if 2D, print as nested list
        py_lst = PyList_New(rows);
        for (int i = 0; i < rows; i++) {
            PyList_SetItem(py_lst, i, PyList_New(cols));
            PyObject *curr_row = PyList_GetItem(py_lst, i);
            for (int j = 0; j < cols; j++) {
                PyList_SetItem(curr_row, j, PyFloat_FromDouble(get(self->mat, i, j)));
            }
        }
    }
    return py_lst;
}

PyObject *Matrix61c_class_to_list(Matrix61c *self, PyObject *args) {
    PyObject *mat = NULL;
    if (PyArg_UnpackTuple(args, "args", 1, 1, &mat)) {
        if (!PyObject_TypeCheck(mat, &Matrix61cType)) {
            PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
            return NULL;
        }
        Matrix61c* mat61c = (Matrix61c*)mat;
        return Matrix61c_to_list(mat61c);
    } else {
        PyErr_SetString(PyExc_TypeError, "Invalid arguments");
        return NULL;
    }
}

/*
 * Add class methods
 */
PyMethodDef Matrix61c_class_methods[] = {
    {"to_list", (PyCFunction)Matrix61c_class_to_list, METH_VARARGS, "Returns a list representation of numc.Matrix"},
    {NULL, NULL, 0, NULL}
};

/*
 * Matrix61c string representation. For printing purposes.
 */
PyObject *Matrix61c_repr(PyObject *self) {
    PyObject *py_lst = Matrix61c_to_list((Matrix61c *)self);
    return PyObject_Repr(py_lst);
}

/* NUMBER METHODS */

/*
 * Add the second numc.Matrix (Matrix61c) object to the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_add(Matrix61c* self, PyObject* args) {
    /* TODO: YOUR CODE HERE */ 
    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }
    Matrix61c *arg_mat61c = (Matrix61c *) args;
    matrix *new_mat = NULL;
    if (allocate_matrix(&new_mat, self->mat->rows, self->mat->cols) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Matrix malloc failed");
        return NULL;
    }
    if (add_matrix(new_mat, self->mat, arg_mat61c->mat) != 0) {
        PyErr_SetString(PyExc_ValueError, "Input matrices do not have the same dimensions!");
        return NULL;
    }
    Matrix61c *new_mat61c = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
    new_mat61c->mat = new_mat;
    new_mat61c->shape = get_shape(new_mat->rows, new_mat->cols);
    return (PyObject *) new_mat61c;
}

/*
 * Substract the second numc.Matrix (Matrix61c) object from the first one. The first operand is
 * self, and the second operand can be obtained by casting `args`.
 */
PyObject *Matrix61c_sub(Matrix61c* self, PyObject* args) {
    /* TODO: YOUR CODE HERE */
    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }
    Matrix61c *arg_mat61c = (Matrix61c *) args;
    matrix *new_mat = NULL;
    if (allocate_matrix(&new_mat, self->mat->rows, self->mat->cols) != 0){
        PyErr_SetString(PyExc_RuntimeError, "Matrix malloc failed");
        return NULL;
    }
    if (sub_matrix(new_mat, self->mat, arg_mat61c->mat) != 0) {
        PyErr_SetString(PyExc_ValueError, "Input matrices do not have the same dimensions!");
        return NULL;
    }
    Matrix61c *new_mat61c = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
    new_mat61c->mat = new_mat;
    new_mat61c->shape = get_shape(new_mat->rows, new_mat->cols);
    return (PyObject *) new_mat61c;
}

/*
 * NOT element-wise multiplication. The first operand is self, and the second operand
 * can be obtained by casting `args`.
 */
PyObject *Matrix61c_multiply(Matrix61c* self, PyObject *args) {
    /* TODO: YOUR CODE HERE */
    if (!PyObject_TypeCheck(args, &Matrix61cType)) {
        PyErr_SetString(PyExc_TypeError, "Argument must of type numc.Matrix!");
        return NULL;
    }
    Matrix61c *arg_mat61c = (Matrix61c *) args;
    matrix *new_mat = NULL;
    if (allocate_matrix(&new_mat, self->mat->rows, arg_mat61c->mat->cols) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Matrix malloc failed");
        return NULL;
    }
    if (mul_matrix(new_mat, self->mat, arg_mat61c->mat) != 0) {
        PyErr_SetString(PyExc_ValueError, "Input matrices do not have the correct dimensions!");
        return NULL;
    }
    Matrix61c *new_mat61c = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
    new_mat61c->mat = new_mat;
    new_mat61c->shape = get_shape(new_mat->rows, new_mat->cols);
    return (PyObject *) new_mat61c;
}

/*
 * Negates the given numc.Matrix.
 */
PyObject *Matrix61c_neg(Matrix61c* self) {
    /* TODO: YOUR CODE HERE */
    matrix *new_mat = NULL;
    if (allocate_matrix(&new_mat, self->mat->rows, self->mat->cols) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Matrix malloc failed");
        return NULL;
    }
    if (neg_matrix(new_mat, self->mat) != 0) {
        PyErr_SetString(PyExc_ValueError, "bruh neg code be wrong doh");
        return NULL;
    }
    Matrix61c *new_mat61c = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
    new_mat61c->mat = new_mat;
    new_mat61c->shape = get_shape(new_mat->rows, new_mat->cols);
    return (PyObject *) new_mat61c;
}

/*
 * Take the element-wise absolute value of this numc.Matrix.
 */
PyObject *Matrix61c_abs(Matrix61c *self) {
    /* TODO: YOUR CODE HERE */
    matrix *new_mat = NULL;
    if (allocate_matrix(&new_mat, self->mat->rows, self->mat->cols) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Matrix malloc failed");
        return NULL;
    }
    if (abs_matrix(new_mat, self->mat) != 0) {
        PyErr_SetString(PyExc_ValueError, "bruh abs code be wrong doh");
        return NULL;
    }
    Matrix61c *new_mat61c = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
    new_mat61c->mat = new_mat;
    new_mat61c->shape = get_shape(new_mat->rows, new_mat->cols);
    return (PyObject *) new_mat61c;
}

/*
 * Raise numc.Matrix (Matrix61c) to the `pow`th power. You can ignore the argument `optional`.
 */
PyObject *Matrix61c_pow(Matrix61c *self, PyObject *pow, PyObject *optional) {
    /* TODO: YOUR CODE HERE */
    if (!PyLong_Check(pow)) {
        PyErr_SetString(PyExc_TypeError, "pow must be an int");
        return NULL;
    }
    int pow_int = PyLong_AsLong(pow);
    if (pow_int < 0) {
        PyErr_SetString(PyExc_ValueError, "pow cannot be negative");
        return NULL;
    } 
    if (self->mat->rows != self->mat->cols) {
        PyErr_SetString(PyExc_ValueError, "input matrix must be square");
        return NULL;
    }
    matrix *new_mat = NULL;
    if (allocate_matrix(&new_mat, self->mat->rows, self->mat->cols) != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Matrix malloc failed");
        return NULL;
    }
    if (pow_matrix(new_mat, self->mat, pow_int) != 0) {
        PyErr_SetString(PyExc_ValueError, "bruh pow code be wrong doh");
        return NULL;
    }
    Matrix61c *new_mat61c = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
    new_mat61c->mat = new_mat;
    new_mat61c->shape = get_shape(new_mat->rows, new_mat->cols);
    return (PyObject *) new_mat61c;
}

/*
 * Create a PyNumberMethods struct for overloading operators with all the number methods you have
 * define. You might find this link helpful: https://docs.python.org/3.6/c-api/typeobj.html
 */
PyNumberMethods Matrix61c_as_number = {
    /* TODO: YOUR CODE HERE */
    .nb_add = (binaryfunc) Matrix61c_add,
    .nb_subtract = (binaryfunc) Matrix61c_sub,
    .nb_multiply = (binaryfunc) Matrix61c_multiply,
    .nb_negative = (unaryfunc) Matrix61c_neg,
    .nb_absolute = (unaryfunc) Matrix61c_abs,
    .nb_power = (ternaryfunc) Matrix61c_pow
};


/* INSTANCE METHODS */

/*
 * Given a numc.Matrix self, parse `args` to (int) row, (int) col, and (double/int) val.
 * Return None in Python (this is different from returning null).
 */
PyObject *Matrix61c_set_value(Matrix61c *self, PyObject* args) {
    /* TODO: YOUR CODE HERE */
    int row;
    int col;
    double val;
    if (!PyTuple_Check(args)) {
        PyErr_SetString(PyExc_TypeError, "args is not a tuple");
        return NULL;    
    } else if (PyTuple_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_TypeError, "args must have 3 elements");
        return NULL;   
    } 
    PyObject *firstItem = PyTuple_GET_ITEM(args, 0);
    PyObject *secondItem = PyTuple_GET_ITEM(args, 1);
    PyObject *thirdItem = PyTuple_GET_ITEM(args, 2);
    if (!PyLong_Check(firstItem) || !PyLong_Check(secondItem)) {
        PyErr_SetString(PyExc_TypeError, "rows and/or cols is not an int");
        return NULL;   
    }
    row = PyLong_AsLong(firstItem);
    col = PyLong_AsLong(secondItem);
    if (PyLong_Check(thirdItem)) {
        val = PyLong_AsDouble(thirdItem);
    } else if (PyFloat_Check(thirdItem)) {
        val = PyFloat_AsDouble(thirdItem);
    } else {
        PyErr_SetString(PyExc_TypeError, "val is not an int or float");
        return NULL;   
    }
    if (row >= self->mat->rows || col >= self->mat->cols || row < 0 || col < 0) {
        PyErr_SetString(PyExc_ValueError, "row and/or col is out of range");
        return NULL;   
    }
    set(self->mat, row, col, val);
    return Py_None;
}

/*
 * Given a numc.Matrix `self`, parse `args` to (int) row and (int) col.
 * Return the value at the `row`th row and `col`th column, which is a Python
 * float/int.
 */
PyObject *Matrix61c_get_value(Matrix61c *self, PyObject* args) {
    /* TODO: YOUR CODE HERE */
    int row;
    int col;
    if (!PyTuple_Check(args)) {
        PyErr_SetString(PyExc_TypeError, "args is not a tuple");
        return NULL;    
    } else if (PyTuple_GET_SIZE(args) != 2) {
        PyErr_SetString(PyExc_TypeError, "args must have 2 elements");
        return NULL;   
    } 
    PyObject *firstItem = PyTuple_GET_ITEM(args, 0);
    PyObject *secondItem = PyTuple_GET_ITEM(args, 1);
    if (!PyLong_Check(firstItem) || !PyLong_Check(secondItem)) {
        PyErr_SetString(PyExc_TypeError, "rows and/or cols is not an int");
        return NULL;   
    }
    row = PyLong_AsLong(firstItem);
    col = PyLong_AsLong(secondItem);
    if (row >= self->mat->rows || col >= self->mat->cols || row < 0 || col < 0) {
        PyErr_SetString(PyExc_ValueError, "row and/or col is out of range");
        return NULL;   
    }
    return PyFloat_FromDouble(get(self->mat, row, col));
}

/*
 * Create an array of PyMethodDef structs to hold the instance methods.
 * Name the python function corresponding to Matrix61c_get_value as "get" and Matrix61c_set_value
 * as "set"
 * You might find this link helpful: https://docs.python.org/3.6/c-api/structures.html
 */
PyMethodDef Matrix61c_methods[] = {
    /* TODO: YOUR CODE HERE */
    {"set", (PyCFunction)Matrix61c_set_value, METH_VARARGS, "docstring"},
    {"get", (PyCFunction)Matrix61c_get_value, METH_VARARGS, "docstring"},
    {NULL, NULL, 0, NULL}
};

/* INDEXING */

/*
 * Given a numc.Matrix `self`, index into it with `key`. Return the indexed result.
 */
PyObject *Matrix61c_subscript(Matrix61c* self, PyObject* key) {
    /* TODO: YOUR CODE HERE */
    //KEY IS AN INT 
    if (PyLong_Check(key)) {
        int index = PyLong_AsLong(key);
        double val;
        if (self->mat->is_1d) {
            if (self->mat->rows == 1) {
                if (index >= self->mat->cols || index < 0) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range");
                    return NULL;    
                }
                val = get(self->mat, 0, index);
            } else {
                if (index >= self->mat->rows || index < 0) {
                    PyErr_SetString(PyExc_IndexError, "Index out of range");
                    return NULL;    
                }
                val = get(self->mat, index, 0);
            }
            return (PyObject*) PyFloat_FromDouble(val);
        }
        if (index >= self->mat->rows || index < 0) {
            PyErr_SetString(PyExc_IndexError, "Index out of range");
            return NULL;    
        }
        int row = index;
        int num_rows = 1;
        int col = 0;
        int num_cols = self->mat->cols;
        matrix *new_mat = NULL;
        allocate_matrix_ref(&new_mat, self->mat, row, col, num_rows, num_cols);
        Matrix61c *new_mat61c = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
        new_mat61c->mat = new_mat;
        new_mat61c->shape = get_shape(num_rows, num_cols);
        return (PyObject *) new_mat61c; 
    //KEY IS A SLICE
    } else if (PySlice_Check(key)) {
        Py_ssize_t len, start = 0, stop = 0, step = 0, slicelength = 0;
        len = self->mat->rows != 1 ? self->mat->rows : self->mat->cols;
        PySlice_GetIndicesEx(key, len, &start, &stop, &step, &slicelength);
        if (step != 1) {
            PyErr_SetString(PyExc_ValueError, "Slice step is not 1");
            return NULL;
        } else if (slicelength < 1) {
            PyErr_SetString(PyExc_ValueError, "Slice length is less than 1");
            return NULL;
        }
        start = (int) start; 
        stop = (int) stop;
        int row, num_rows, col, num_cols;
        if (self->mat->rows == 1) {
            row = 0;
            num_rows = 1;
            col = start;
            num_cols = stop - start;
        } else {
            row = start;
            num_rows = stop - start;
            col = 0;
            num_cols = self->mat->cols;
        }
        matrix *new_mat = NULL;
        allocate_matrix_ref(&new_mat, self->mat, row, col, num_rows, num_cols);
        if (new_mat->rows == 1 && new_mat->cols == 1) {
            double val = get(new_mat, 0, 0);
            return (PyObject*) PyFloat_FromDouble(val);
        }
        Matrix61c *new_mat61c = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
        new_mat61c->mat = new_mat;
        new_mat61c->shape = get_shape(num_rows, num_cols);
        return (PyObject *) new_mat61c; 
    //KEY IS A TUPLE 
    } else if (PyTuple_Check(key)) {
        if (PyTuple_GET_SIZE(key) != 2) {
            PyErr_SetString(PyExc_TypeError, "Input tuple does not have 2 inputs");
            return NULL;
        }
        if (self->mat->is_1d) {
            PyErr_SetString(PyExc_TypeError, "1D matrices can only be sliced with 1 index");
            return NULL;
        }
        PyObject *firstItem = PyTuple_GET_ITEM(key, 0);
        PyObject *secondItem = PyTuple_GET_ITEM(key, 1);
        int row, num_rows, col, num_cols;
        if (PyLong_Check(firstItem)) {
            row = PyLong_AsLong(firstItem);
            if (row >= self->mat->rows || row < 0) {
                PyErr_SetString(PyExc_IndexError, "Row index out of range");
                return NULL;    
            }
            num_rows = 1;
        } else if (PySlice_Check(firstItem)) {
            Py_ssize_t row_len , row_start = 0, row_stop = 0, row_step = 0, row_slicelength = 0;
            row_len = self->mat->rows != 1 ? self->mat->rows : self->mat->cols;
            PySlice_GetIndicesEx(firstItem, row_len, &row_start, &row_stop, &row_step, &row_slicelength);
            if (row_step != 1) {
                PyErr_SetString(PyExc_ValueError, "Slice step is not 1");
                return NULL;
            } else if (row_slicelength < 1) {
                PyErr_SetString(PyExc_ValueError, "Slice length is less than 1");
                return NULL;
            }
            row_start = (int) row_start; 
            row_stop = (int) row_stop;
            row = row_start;
            num_rows = row_stop - row_start;
        } else {
            PyErr_SetString(PyExc_TypeError, "Item 1 in key tuple is invalid");
            return NULL;
        }
        if (PyLong_Check(secondItem)) {
            col = PyLong_AsLong(secondItem);
            num_cols = 1;
            if (col >= self->mat->cols || col < 0) {
                PyErr_SetString(PyExc_IndexError, "Col index out of range");
                return NULL;    
            }
        } else if (PySlice_Check(secondItem)) {
            Py_ssize_t col_len = 0, col_start = 0, col_stop = 0, col_step = 0, col_slicelength = 0;
            col_len = self->mat->cols != 1 ? self->mat->cols : self->mat->rows;
            PySlice_GetIndicesEx(secondItem, col_len, &col_start, &col_stop, &col_step, &col_slicelength);
            if (col_step != 1) {
                PyErr_SetString(PyExc_ValueError, "Slice step is not 1");
                return NULL;
            } else if (col_slicelength < 1) {
                PyErr_SetString(PyExc_ValueError, "Slice length is less than 1");
                return NULL;
            }
            col_start = (int) col_start; 
            col_stop = (int) col_stop;
            col = col_start;
            num_cols = col_stop - col_start;
        } else {
            PyErr_SetString(PyExc_TypeError, "Item 2 in key tuple is invalid");
            return NULL;
        }
        if (PyLong_Check(firstItem) && PyLong_Check(secondItem)) {
            double val = get(self->mat, row, col);
            return (PyObject*) PyFloat_FromDouble(val);
        }
        matrix *new_mat = NULL;
        allocate_matrix_ref(&new_mat, self->mat, row, col, num_rows, num_cols);
        if (new_mat->rows == 1 && new_mat->cols == 1) {
            double val = get(new_mat, 0, 0);
            return (PyObject*) PyFloat_FromDouble(val);
        }
        Matrix61c *new_mat61c = (Matrix61c*) Matrix61c_new(&Matrix61cType, NULL, NULL);
        new_mat61c->mat = new_mat;
        new_mat61c->shape = get_shape(num_rows, num_cols);
        return (PyObject *) new_mat61c; 
    } else {
        PyErr_SetString(PyExc_TypeError, "Bad key :(");
        return NULL;
    }
}

/*
 * Given a numc.Matrix `self`, index into it with `key`, and set the indexed result to `v`.
 */
int Matrix61c_set_subscript(Matrix61c* self, PyObject *key, PyObject *v) {
    /* TODO: YOUR CODE HERE */
    PyObject *original_data = Matrix61c_subscript(self, key);
    if (original_data == NULL) {
        return -1;
    }
    if (PyFloat_Check(original_data)) {
        int row = 0;
        int col = 0;
        double val;
        if (PyFloat_Check(v)) {
            val = PyFloat_AsDouble(v);
        } else if (PyLong_Check(v)) {
            val = PyLong_AsDouble(v);
        } else {
            PyErr_SetString(PyExc_TypeError, "v is not a double or int:(");
            return -1;
        }
        if (PyLong_Check(key)) {
            if (self->mat->rows == 1) {
                row = 0;
                col = PyLong_AsLong(key);
            } else {
                row = PyLong_AsLong(key);
                col = 0;
            }
        } else if (PySlice_Check(key)) {
            Py_ssize_t len = 0, start = 0, stop = 0, step = 0, slicelength = 0;
            PySlice_GetIndicesEx(key, len, &start, &stop, &step, &slicelength);
            if (self->mat->rows == 1) {
                row = 0;
                col = start;
            } else {
                row = start;
                col = 0;
            }
        } else if (PyTuple_Check(key)) {
            PyObject *firstItem = PyTuple_GET_ITEM(key, 0);
            PyObject *secondItem = PyTuple_GET_ITEM(key, 1);
            if (PyLong_Check(firstItem)) {
                row = PyLong_AsLong(firstItem);
            } else if (PySlice_Check(firstItem)) {
                Py_ssize_t row_len = 0, row_start = 0, row_stop = 0, row_step = 0, row_slicelength = 0;
                PySlice_GetIndicesEx(firstItem, row_len, &row_start, &row_stop, &row_step, &row_slicelength);
                row = row_start;
            } 
            if (PyLong_Check(secondItem)) {
                col = PyLong_AsLong(secondItem);
            } else if (PySlice_Check(secondItem)) {
                Py_ssize_t col_len = 0, col_start = 0, col_stop = 0, col_step = 0, col_slicelength = 0;
                PySlice_GetIndicesEx(secondItem, col_len, &col_start, &col_stop, &col_step, &col_slicelength);
                col = col_start;
            } 
        } 
        set(self->mat, row, col, val);
        return 0;
    } else if (PyObject_TypeCheck(original_data, &Matrix61cType)) {
        Matrix61c *original_matrix = (Matrix61c *) original_data;
        if (!PyList_Check(v)) {
            PyErr_SetString(PyExc_TypeError, "v is not a list :(");
            Matrix61c_dealloc(original_matrix);
            return -1;
        }
        matrix *original_mat = original_matrix->mat;
        if (original_mat->is_1d) {
            if (PyList_Size(v) != (original_mat->rows) * (original_mat->cols)) {
                PyErr_SetString(PyExc_ValueError, "v has the wrong length :(");
                Matrix61c_dealloc(original_matrix);
                return -1;
            }
            if (original_mat->rows == 1) {
                for (int i = 0; i < original_mat->cols; i++) {
                    double v_val;
                    if (PyFloat_Check(PyList_GetItem(v, i))) {
                        v_val = PyFloat_AsDouble(PyList_GetItem(v, i));
                    } else if (PyLong_Check(PyList_GetItem(v, i))) {
                        v_val = PyLong_AsDouble(PyList_GetItem(v, i));
                    } else {
                        PyErr_SetString(PyExc_ValueError, "v_val is not an int or double :(");
                        Matrix61c_dealloc(original_matrix);
                        return -1;  
                    }
                    set(original_mat, 0, i, v_val);
                }
            } else {
                for (int i = 0; i < original_mat->rows; i++) {
                    double v_val;
                    if (PyFloat_Check(PyList_GetItem(v, i))) {
                        v_val = PyFloat_AsDouble(PyList_GetItem(v, i));
                    } else if (PyLong_Check(PyList_GetItem(v, i))) {
                        v_val = PyLong_AsDouble(PyList_GetItem(v, i));
                    } else {
                        PyErr_SetString(PyExc_ValueError, "v_val is not an int or double :(");
                        Matrix61c_dealloc(original_matrix);
                        return -1;  
                    }
                    set(original_mat, i, 0, v_val);
                }
            }
        } else {
            if (PyList_Size(v) != original_mat->rows) {
                PyErr_SetString(PyExc_ValueError, "v has the wrong length :(");
                Matrix61c_dealloc(original_matrix);
                return -1;
            }
            for (int row = 0; row < original_mat->rows; row++) {
                PyObject *v_row = PyList_GetItem(v, row);
                if (!PyList_Check(v_row)) {
                    PyErr_SetString(PyExc_ValueError, "v element is not a list");
                    Matrix61c_dealloc(original_matrix);
                    return -1;
                }
                if (PyList_Size(v_row) != original_mat->cols) {
                    PyErr_SetString(PyExc_ValueError, "list in v has the wrong length :(");
                    Matrix61c_dealloc(original_matrix);
                    return -1;
                }
                for (int col = 0; col < original_mat->cols; col++) {
                    double v_val;
                    if (PyFloat_Check(PyList_GetItem(v_row, col))) {
                        v_val = PyFloat_AsDouble(PyList_GetItem(v_row, col));
                    } else if (PyLong_Check(PyList_GetItem(v_row, col))) {
                        v_val = PyLong_AsDouble(PyList_GetItem(v_row, col));
                    } else {
                        PyErr_SetString(PyExc_ValueError, "v_val is not an int or double :(");
                        Matrix61c_dealloc(original_matrix);
                        return -1;  
                    }
                    set(original_mat, row, col, v_val);
                }
            }
        }
        Matrix61c_dealloc(original_matrix);
    } else {
        PyErr_SetString(PyExc_TypeError, "bruh this code is wrong");
        return -1;
    }
    return 0;
    
    
    if (PyLong_Check(key)) {

    } else if (PySlice_Check(key)) {

    } else if (PyTuple_Check(key)) {
        PyObject *firstItem = PyTuple_GET_ITEM(key, 0);
        PyObject *secondItem = PyTuple_GET_ITEM(key, 1);
        if (PyLong_Check(firstItem)) {

        } else if (PySlice_Check(firstItem)) {

        } 
        if (PyLong_Check(secondItem)) {

        } else if (PySlice_Check(secondItem)) {

        } 
    } 
}

PyMappingMethods Matrix61c_mapping = {
    NULL,
    (binaryfunc) Matrix61c_subscript,
    (objobjargproc) Matrix61c_set_subscript,
};

/* INSTANCE ATTRIBUTES*/
PyMemberDef Matrix61c_members[] = {
    {
        "shape", T_OBJECT_EX, offsetof(Matrix61c, shape), 0,
        "(rows, cols)"
    },
    {NULL}  /* Sentinel */
};

PyTypeObject Matrix61cType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "numc.Matrix",
    .tp_basicsize = sizeof(Matrix61c),
    .tp_dealloc = (destructor)Matrix61c_dealloc,
    .tp_repr = (reprfunc)Matrix61c_repr,
    .tp_as_number = &Matrix61c_as_number,
    .tp_flags = Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_BASETYPE,
    .tp_doc = "numc.Matrix objects",
    .tp_methods = Matrix61c_methods,
    .tp_members = Matrix61c_members,
    .tp_as_mapping = &Matrix61c_mapping,
    .tp_init = (initproc)Matrix61c_init,
    .tp_new = Matrix61c_new
};


struct PyModuleDef numcmodule = {
    PyModuleDef_HEAD_INIT,
    "numc",
    "Numc matrix operations",
    -1,
    Matrix61c_class_methods
};

/* Initialize the numc module */
PyMODINIT_FUNC PyInit_numc(void) {
    PyObject* m;

    if (PyType_Ready(&Matrix61cType) < 0)
        return NULL;

    m = PyModule_Create(&numcmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&Matrix61cType);
    PyModule_AddObject(m, "Matrix", (PyObject *)&Matrix61cType);
    printf("CS61C Fall 2020 Project 4: numc imported!\n");
    fflush(stdout);
    return m;
}