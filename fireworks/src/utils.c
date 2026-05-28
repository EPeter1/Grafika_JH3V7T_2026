#include "utils.h"

#include "vec3.h"

#include <math.h>
#include <stdlib.h>

double degree_to_radian(double degree) {
    return degree * M_PI / 180.0;
}

double radian_to_degree(double radian) {
    return radian * 180.0 / M_PI;
}

vec3 get_spherical_direction() {
    float z = rand_range(-1.0f, 1.0f);
    float phi = rand_range(0.0f, 2.0f * M_PI);
    float radius = sqrtf(1.0f - z * z);

    vec3 direction = {
        radius * cosf(phi),
        radius * sinf(phi),
        z
    };

    return direction;
}

vec3 get_cone_direction(vec3 axis, float spread) {
    vec3 direction = axis;

    direction.x += rand_symmetric(spread);
    direction.y += rand_symmetric(spread);
    direction.z += rand_symmetric(spread);

    return normalize_vec3(direction);
}

float rand_range(float min, float max) {
    return (float)rand() / (float)RAND_MAX * (max - min) + min;
}

float rand_symmetric(float deviation) {
    return ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * deviation;
}

float clamp(float value, float min, float max) {
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}
