# C Linear Algebra Library

This is a C library for linear algebra operations, designed to handle vectors and matrices. It provides functions for basic arithmetic, linear transformations, and other common operations. It uses BLAS and Intel Vector Mathematical Function Library for optimized basic operations.

#### Key Features

- **Vector Operations:** Addition, subtraction, multiplication, division, dot product, scaling, normalization, etc.
- **Matrix Operations:** Addition, subtraction, multiplication, dot product, transposition, etc.
- **Memory Management:** Efficient memory allocation and deallocation for vectors and matrices.
- **Error Handling:** Basic error checking for invalid input dimensions and null pointers.
- **Performance:** Optimized for speed using vectorized instructions and efficient algorithms.

#### Files

- `lin_alg.h`: Main header file including all other headers.
- `lin_alg_config.h`: Configuration file for data types and sizes.
- `mat.h`, `mat.c`: Matrix structure definition and operations.
- `vec.h`, `vec.c`: Vector structure definition and operations.
- `payload.h`, `payload.c`: Payload structure for underlying data storage.
- `slice.h`, `slice.c`: Slice structure for indexing and views.
- `vector_eng.h`, `vector_eng_mkl.h`: Vectorized operations (using Intel MKL).
- `vec_mat.h`, `vec_mat.c`: Combined vector-matrix operations.
- `lin_alg_test.c`: Unit tests for the library.

#### Usage

1. **Include Headers:** Include the `lin_alg.h` header in your C source files.
2. **Create Objects:** Create `vec` and `mat` structures using the provided constructor functions.
3. **Perform Operations:** Use the library functions to perform various linear algebra operations on the created objects.
4. **Free Memory:** Deallocate memory used by vectors and matrices using the destructor functions.

#### Example

```c
#include "lin_alg.h"

int main() {
    mat *A = mat_new(3, 3); // Create a 3x3 matrix
    vec *x = vec_new(3);    // Create a 3-element vector

    // ... (Fill A and x with values)

    vec *b = vec_new(3);
    mat_dot_vec(b, A, x); // Calculate b = A * x

    // ... (Use the result b)

    mat_del(A); // Free matrix A
    vec_del(x); // Free vector x
    vec_del(b); // Free vector b

    return 0;
}
```

#### Dependencies

- **Intel MKL:** Intel Math Kernel Library for optimized vectorized operations.
