#include "transform.h"

#include <stdio.h>

int main() {

    float point[3] = {1, 2, 1};
    float transformation_matrix[3][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };
    
    scale(1.5, 2, transformation_matrix);
    shift(-5, 8, transformation_matrix);
    rotate(90, transformation_matrix);
    transform_point(transformation_matrix, point);

    printf("A pont transzformacio utan: (%.2f, %.2f)", point[0], point[1]);

    return 0;
}
