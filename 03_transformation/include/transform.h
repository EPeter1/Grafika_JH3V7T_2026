#ifndef TRANSFORM_H
#define TRANSFORM_H

/**
 * Transforms a given point by applying the transformation matrix.
 */
void transform_point(const float transformation_matrix[3][3], float point[3]);

/**
 * Modifies the transformation matrix to perform scaling.
 */
void scale(float scale_x, float scale_y, float transformation_matrix[3][3]);

/**
 * Modifies the transformation matrix to perform shifting.
 */
void shift(float delta_x, float delta_y, float transformation_matrix[3][3]);

/**
 * Modifies the transformation matrix to perform rotation.
 */
void rotate(float angle_degrees, float transformation_matrix[3][3]);

#endif // TRANSFORM_H
