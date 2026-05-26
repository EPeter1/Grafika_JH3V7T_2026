#ifndef VEC3_H
#define VEC3_H

#include <math.h>

/**
 * GLSL-like three dimensional vector
 */
typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3;

static inline void set_vec3(vec3* v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

static inline void set_vec3_scalar(vec3* v, float scalar) {
    v->x = scalar;
    v->y = scalar;
    v->z = scalar;
}

static inline vec3 add_vec3(vec3 a, vec3 b) {
    return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline vec3 subtract_vec3(vec3 a, vec3 b) {
    return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline float dot_vec3(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline vec3 scale_vec3(vec3 v, float scalar) {
    return (vec3){v.x * scalar, v.y * scalar, v.z * scalar};
}

static inline float get_vec3_length(vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline vec3 normalize_vec3(vec3 v) {
    float length = get_vec3_length(v);

    if (length > 0.0001f) {
        float inverse_length = 1.0f / length;
        return (vec3){v.x * inverse_length, v.y * inverse_length, v.z * inverse_length};
    }

    return (vec3){0.0f, 0.0f, 1.0f};
}

static inline vec3 cross_vec3(vec3 a, vec3 b) {
    vec3 result;

    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;

    return result;
}

#endif // VEC3_H
