#ifndef UTILS_H
#define UTILS_H

#include "vec3.h"

/**
 * Calculates radian from degree.
 */
double degree_to_radian(double degree);

float rand_range(float min, float max);
float rand_symmetric(float deviation);
float clamp(float value, float min, float max);
vec3 get_spherical_direction();
vec3 get_cone_direction(vec3 axis, float spread);

#endif // UTILS_H
