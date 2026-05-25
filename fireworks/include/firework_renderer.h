#ifndef FIREWORK_RENDERER_H
#define FIREWORK_RENDERER_H

#include "fireworks.h"
#include "spark.h"
#include "vec3.h"

#include <GL/glew.h>

#define MAX_TOTAL_SPARKS (MAX_FIREWORKS * MAX_SPARKS)
#define MAX_TRAIL_VERTICES (MAX_TOTAL_SPARKS * MAX_HISTORY)
#define MAX_TRAIL_INDICES (MAX_TOTAL_SPARKS * (MAX_HISTORY + 1))

#define PRIMITIVE_RESTART_INDEX 0xFFFFFFFF

typedef struct VertexColor {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} VertexColor;

typedef struct SparkVertex {
    vec3 position;
    VertexColor color;
    float size;
} SparkVertex;

typedef struct TrailVertex {
    vec3 position;
    VertexColor color;
    float fade;
} TrailVertex;

typedef struct FireworkRenderer {
    GLuint spark_vao;
    GLuint trail_vao;

    GLuint spark_vbo;
    GLuint trail_vbo;
    GLuint trail_ibo;

    GLuint spark_texture;
    GLuint spark_shader;
    GLuint trail_shader;

    int spark_vertices;
    int trail_vertices;
    int trail_indices;
} FireworkRenderer;

void init_firework_renderer(FireworkRenderer* renderer);
void update_firework_buffers(FireworkRenderer* renderer, const Firework* fireworks);
void destroy_firework_renderer(FireworkRenderer* renderer);

#endif // FIREWORK_RENDERER_H
