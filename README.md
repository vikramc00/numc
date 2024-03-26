# Numc
Implemented a slower version of numpy.

### Design
In this project, I had to make a python-c interface so that C code could be run under the hood while the user calls everything in python. Much of this interface was implemeneted in numc.c. I received inputs as various types of PyObjects and had to parse them to get the data I needed. The types of data included a 61c_matrix, a tuple, a slice, and an int. After getting the data I needed, I could call the functions that performed the logic necessary to get the desired output. These functions only dealt with regular c objects and were implemented in matrix.c. In order to enable the python and C code to interact, I also had to enable the user to import the module. To do this, I read the python extension documentation and instantiated an extension based on the C files and set it up. 

In order to optimize, I primarily used openmp and unrolling. These were our sole strategies for allocating and simple instructions. I used additional strategies for mul and pow. For mul, I also reordered the for loops to make it ikj instead of ijk. For pow, I implemented repeated squaring to reduce the number of muls called. Below, I will discuss each strategy in detail. I also implemented a fake 1D impelmentation using a contiguous block of memory in row major order to repreent the matrix that still allowed for normal matrix indexing. 

- OpenMP:
Since this project employs many loops to alter the values of matrices, I decided that OpenMP could be beneficial. I used pragma omp parallel for if() for most loops. I added the conditional if because the overhead cost actually makes operations slower for small matrices that do not benefit much from parallelism. In the event where there were nested loops, I opted to use openMP on the outer-most loop. I used this for every function.

- Loop Reordering:
A strategy I used in mul was to reorder the nested for loop structure from ijk to ikj to improve mul performance. I found ikj to be more beneficial to our runtime because it takes advantage of spacial locality, utilizing the cache in a more efficient way. This technique increased the number of cache hits, allowing the program to need to access memory less often. Memory access is very costly in terms of runtime, so this greatly reduced the cost of operations.

- Fake 1D:
Originally, the rows were far away from each other in memory, making reading and writing more expensive as it does not take advantage of spatial cache. I wanted a way to have the elements close to each other so they'd be easier to access. A strategy I used in allocate was to allocate a contiguous block of memory the size of the number of elements in the matrix. Then, I set row pointers to point to specific sections of this block. This strategy also enabled us to optimize in other functions. Our code to run faster since I could set values with single for loops instead of nested loops, which reduced the number of variables that would need to be tracked, updated, and compared. It also made unrolling simpler. 

- Unrolling:
For all functions, I implemented unrolling. Originally, each iteration of a loop performed one operation on an element of a matrix. However, this is inefficient as it has to constantly update i. By doing 4 operations in one iterations, i does not need to undergo as many updates or comparisons. In the assembly code, less jumps would need to be performed as there are now fewer iterations.


## Usage

In a python environment, import the numc package. This can be done in a python interpreter or at the top of your .py file

`import numc as nc`

There are many methods of instantiating a matrix. Below, these methods are listed.

One method is to pass in the number of rows and columns, having numc will the matrix with zero's for each element. The following creates a matrix of zero's with 4 rows and 7 columns

`nc.Matrix(4, 7)`

You may also initialize a matrix with the number of rows and columns, but also specify a value to be placed into every element of the matrix. The following creates a matrix of one's with 4 rows and 7 columns

`nc.Matrix(4, 7, 1)`

Matrices can also be instantiated explicitly by passing in lists of values to be inserted into the matrix. The following will create a 3 x 2 matrix with the specified values from the lists.

`nc.Matrix([[6, 9], [5, 8], [2, 1]])`

To specify the values of a specific row and generalize it along all rows, you can input the number of rows, number of columns, and a list of values in a row. The following will create a matrix with 3 rows, 1 column, with [0, 1, 2] in each row.

`nc.Matrix(3, 1, [0, 1, 2])`

## Operations

The `+` operator will add two matrices and return their sum. Input matrices must have the same dimensions.

The `-` operator will subtract two matrices and return their difference. Input matrices must have the same dimensions.

The `*` operator will perform matrix multiplication on two input matrices and return their product. The number of columns of the first matrix must equal the number of rows of the second matrix.

The `**` operator will take a matrix and an integer and return the matrix to the power of the integer by performing matrix multiplication a certain mumber of times. The input matrix must be square. 

The `-` operator placed in front of a single matrix will negate each element of the matrix. 

The `abs` operator will take in a matrix and return the matrix with the absolute value run on every element.


## Slicing

You may index into a matrix using a single int. If 2D, a 1D matrix will be returned.
`>>>a = nc.Matrix(3, 3) `
`>>>a[0] # Key is a single number`
`[0.0, 0.0, 0.0]`

 If 1D, a double will be returned. 
`>>>a = nc.Matrix(1, 3) `
`>>>a[0] # Key is a single number`
`0.0`

You may also index into a matrix using a single slice. A 1D or 2D matrix will be returned depending on if the slice has 1 or more elements.
`>>> a[0:2] # key is a single slice`
`[[0.0, 0.0, 0.0], [0.0, 0.0, 0.0]]`
`>>> a[0:1] # key is a single slice`
`[0.0, 0.0, 0.0]`

The key may also be a tuple of 2 element, each one being an int or a slice.
`>>> a[0:2, 0:2] # key is a tuple of two slices`
`[[0.0, 0.0], [0.0, 0.0]]`
`>>> a[0:2, 0] # key is a tuple of (slice, int)`
`[0.0, 0.0]`
`>>> a[0, 0:2] # key is a tuple of (int, slice)`
`[0.0, 0.0]`
`>>> a[0, 0] # key is a tuple of (int, int)`
`0.0`

While slices can take any input that doesn't result in a slice less than or equal to 0 including negative numbers, all keys that include an int as an input index must be viable in order for the slicing to work.
