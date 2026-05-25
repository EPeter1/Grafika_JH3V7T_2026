#ifndef WATER_H
#define WATER_H

#include "fireworks.h"
#include "texture.h"
#include "vec3.h"

#include <GL/glew.h>

#define MAX_LIGHT_SOURCES 10

typedef struct WaterVertex {
    vec3 position;
    TextureUV texture_uv;
    vec3 normal;
} WaterVertex;

typedef struct Uniforms {
    GLint water_texture;
    GLint water_normal;
    GLint view_position;
    GLint light_positions;
    GLint light_colors;
    GLint light_count;
    GLint current_time;
} Uniforms;

typedef struct WaterRenderer {
    GLuint water_vao;
    GLuint water_vbo;

    GLuint water_texture;
    GLuint water_normal;
    GLuint water_shader;

    Uniforms uniforms;

    int light_count;
    vec3 light_positions[MAX_LIGHT_SOURCES];
    vec3 light_colors[MAX_LIGHT_SOURCES];
} WaterRenderer;

void init_water_renderer(WaterRenderer* renderer);
void update_water_buffers(WaterRenderer* renderer, const Firework* fireworks);
void draw_water_surface(const WaterRenderer* renderer, const vec3 camera_position, float current_time);
void destroy_water_renderer(WaterRenderer* renderer);

#endif // WATER_H
