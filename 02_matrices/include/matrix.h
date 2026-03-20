#ifndef MATRIX_H
#define MATRIX_H

/**
 * Print the elements of the matrix.
 */
void print_matrix(const float matrix[3][3]);

/**
 * Initializes all elements of the matrix to zero.
 */
void init_zero_matrix(float matrix[3][3]);

/**
 * Initializes all elements of the matrix to one.
 */
void init_identity_matrix(float matrix[3][3]);

/**
 * Add matrices.
 */
void add_matrices(const float matrix_a[3][3], const float matrix_b[3][3], float result_matrix[3][3]);

/**
 * Multiply a matrix by a scalar value.
 */
void multiply_matrix_by_scalar(float matrix[3][3], float scalar);

/**
 * Multiply matrices.
 */
void multiply_matrices(const float matrix_a[3][3], const float matrix_b[3][3], float result_matrix[3][3]);

#endif // MATRIX_H
