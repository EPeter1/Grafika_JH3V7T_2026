#include "matrix.h"

#include <stdio.h>

int main() {

    float matrix_a[3][3] = {
        {0.5, 2.3, -1.9},
        {-1, 2, 3},
        {-4.7, 10.1, 0}
    };
    float matrix_b[3][3] = {
        {1, 2, 3},
        {-4, 5, -6},
        {7, -8, 9}
    };
    float result_matrix[3][3];

    add_matrices(matrix_a, matrix_b, result_matrix);
    printf("Az A es B matrixok osszege:\n\n");
    print_matrix(result_matrix);

    multiply_matrices(matrix_a, matrix_b, result_matrix);
    printf("\nAz A es B matrixok szorzata:\n\n");
    print_matrix(result_matrix);

    init_identity_matrix(result_matrix);
    multiply_matrix_by_scalar(result_matrix, 666.66);

    printf("\nAz egysegmatrix ordoggel valo szorzata:\n\n");
    print_matrix(result_matrix);

    return 0;
}
