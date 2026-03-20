#include "transform.h"

#define _USE_MATH_DEFINES
#include <math.h>

void transform_point(const float transformation_matrix[3][3], float point[3]) {

    float point_x_new = transformation_matrix[0][0] * point[0] +
        transformation_matrix[0][1] * point[1] +
        transformation_matrix[0][2] * point[2];

    float point_y_new = transformation_matrix[1][0] * point[0] +
        transformation_matrix[1][1] * point[1] +
        transformation_matrix[1][2] * point[2];
        
    float point_w_new = transformation_matrix[2][0] * point[0] +
        transformation_matrix[2][1] * point[1] +
        transformation_matrix[2][2] * point[2];

    point[0] = point_x_new;
    point[1] = point_y_new;
    point[2] = point_w_new;
}

void scale(float scale_x, float scale_y, float transformation_matrix[3][3]) {

    transformation_matrix[0][0] *= scale_x;
    transformation_matrix[1][1] *= scale_y;
}

void shift(float delta_x, float delta_y, float transformation_matrix[3][3]) {

    transformation_matrix[0][2] += delta_x;
    transformation_matrix[1][2] += delta_y;
}

void rotate(float angle_degrees, float transformation_matrix[3][3]) {

    float angle_radians = angle_degrees * (M_PI / 180.0);
    float cos_angle = cos(angle_radians);
    float sin_angle = sin(angle_radians);

    transformation_matrix[0][0] = cos_angle;
    transformation_matrix[0][1] = -sin_angle;
    transformation_matrix[1][0] = sin_angle;
    transformation_matrix[1][1] = cos_angle;
}
