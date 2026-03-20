#include "matrix.h"

#include <stdio.h>

void print_matrix(const float matrix[3][3]) {

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%7.2f ", matrix[i][j]);
        }
        printf("\n");
    }
}

void init_zero_matrix(float matrix[3][3]) {

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix[i][j] = 0.0;
        }
    }
}

void init_identity_matrix(float matrix[3][3]) {

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == j) {
                matrix[i][j] = 1.0;
            }
            else {
                matrix[i][j] = 0.0;
            }
        }
    }
}

void add_matrices(const float matrix_a[3][3], const float matrix_b[3][3], float result_matrix[3][3]) {

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result_matrix[i][j] = matrix_a[i][j] + matrix_b[i][j];
        }
    }
}

void multiply_matrix_by_scalar(float matrix[3][3], float scalar) {

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrix[i][j] *= scalar;
        }
    }
}

void multiply_matrices(const float matrix_a[3][3], const float matrix_b[3][3], float result_matrix[3][3]) {

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result_matrix[i][j] = 0.0;
            for (int k = 0; k < 3; k++) {
                result_matrix[i][j] += matrix_a[i][k] * matrix_b[k][j];
            }
        }
    }
}
