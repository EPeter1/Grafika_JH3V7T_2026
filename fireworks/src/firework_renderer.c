#include "firework_renderer.h"

#include "fireworks.h"
#include "shader.h"
#include "spark.h"
#include "texture.h"

#include <GL/glew.h>
#include <SDL2/SDL_stdinc.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static const VertexAttribute spark_attributes[] = {
    { 0, "a_position" },
    { 1, "a_color" },
    { 2, "a_size" },
    { 0, NULL }
};

static const VertexAttribute trail_attributes[] = {
    { 0, "a_position" },
    { 1, "a_color" },
    { 2, "a_fade" },
    { 0, NULL }
};

static void init_spark_mesh(FireworkRenderer* renderer) {
    glGenBuffers(1, &renderer->spark_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->spark_vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_TOTAL_SPARKS * sizeof(SparkVertex), NULL, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &renderer->spark_vao);
    glBindVertexArray(renderer->spark_vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->spark_vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SparkVertex), (void*)offsetof(SparkVertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(SparkVertex), (void*)offsetof(SparkVertex, color));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(SparkVertex), (void*)offsetof(SparkVertex, size));
}

static void init_trail_mesh(FireworkRenderer* renderer) {
    glGenBuffers(1, &renderer->trail_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->trail_vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_TRAIL_VERTICES * sizeof(TrailVertex), NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &renderer->trail_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->trail_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_TRAIL_INDICES * sizeof(unsigned int), NULL, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &renderer->trail_vao);
    glBindVertexArray(renderer->trail_vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->trail_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->trail_ibo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TrailVertex), (void*)offsetof(TrailVertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(TrailVertex), (void*)offsetof(TrailVertex, color));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(TrailVertex), (void*)offsetof(TrailVertex, fade));
}

static int upload_spark_data(FireworkRenderer* renderer, const Firework* fireworks) {
    int spark_count = 0;

    glBindBuffer(GL_ARRAY_BUFFER, renderer->spark_vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_TOTAL_SPARKS * sizeof(SparkVertex), NULL, GL_DYNAMIC_DRAW);
    SparkVertex* spark_pointer = (SparkVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    if (!spark_pointer) {
        fprintf(stderr, "[ERROR] Failed to map spark buffer!\n");
        exit(1); 
    }

    for (int i = 0; i < MAX_FIREWORKS; i++) {
        const Firework* firework = &fireworks[i];
        if (firework->state != FIREWORK_EXPLODED) {
            continue;
        }

        for (int j = 0; j < MAX_SPARKS; j++) {
            const Spark* spark = &(firework->sparks[j]);
            if (spark->current_life <= 0 || spark->render_color.alpha <= 0.01f || spark->size <= 0.0f) {
                continue;
            }

            SparkVertex* vertex = &spark_pointer[spark_count];
            vertex->position = spark->position;
            vertex->size = spark->size;

            vertex->color = (VertexColor){
                (unsigned char)(spark->render_color.red * 255.0f),
                (unsigned char)(spark->render_color.green * 255.0f),
                (unsigned char)(spark->render_color.blue * 255.0f),
                (unsigned char)(spark->render_color.alpha * 255.0f)
            };

            spark_count++;
        }
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    return spark_count;
}

static void upload_trail_data(FireworkRenderer* renderer, const Firework* fireworks, int* out_trail_count, int* out_index_count) {
    int trail_count = 0;
    int index_count = 0;

    glBindBuffer(GL_ARRAY_BUFFER, renderer->trail_vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_TRAIL_VERTICES * sizeof(TrailVertex), NULL, GL_DYNAMIC_DRAW);
    TrailVertex* trail_pointer = (TrailVertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->trail_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_TRAIL_INDICES * sizeof(unsigned int), NULL, GL_DYNAMIC_DRAW);
    unsigned int* index_pointer = (unsigned int*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

    if (!trail_pointer || !index_pointer) {
        fprintf(stderr, "[ERROR] Failed to map trail buffers!\n");
        exit(1);
    }

    for (int i = 0; i < MAX_FIREWORKS; i++) {
        const Firework* firework = &fireworks[i];
        if (firework->state != FIREWORK_EXPLODED) {
            continue;
        }

        for (int j = 0; j < MAX_SPARKS; j++) {
            const Spark* spark = &(firework->sparks[j]);
            const Trail* trail = &(firework->trails[spark->trail_index]);

            if (spark->current_life <= 0 || spark->render_color.alpha <= 0.01f || trail->length <= 1) {
                continue;
            }

            int length = trail->length;
            int head = trail->pointer;
            unsigned int base_vertex_index = trail_count;
            float inverse_length = 1.0f / (float)(length - 1);

            VertexColor color = {
                (unsigned char)(spark->render_color.red * 255.0f),
                (unsigned char)(spark->render_color.green * 255.0f),
                (unsigned char)(spark->render_color.blue * 255.0f),
                (unsigned char)(spark->render_color.alpha * 255.0f)
            };

            for (int k = 0; k < length; k++) {
                int index = (head - length + k + MAX_HISTORY) % MAX_HISTORY;

                TrailVertex* vertex = &trail_pointer[trail_count];
                vertex->position = trail->history[index];
                vertex->fade = 1.0f - ((float)k * inverse_length);
                vertex->color = color;

                index_pointer[index_count++] = base_vertex_index + k;
                trail_count++;
            }

            index_pointer[index_count++] = PRIMITIVE_RESTART_INDEX;
        }
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    *out_trail_count = trail_count;
    *out_index_count = index_count;
}

void init_firework_renderer(FireworkRenderer* renderer) {
    renderer->spark_vertices = 0;
    renderer->trail_vertices = 0;
    renderer->trail_indices = 0;

    renderer->spark_texture = load_texture("assets/textures/glow.png", GL_CLAMP_TO_EDGE, false);
    renderer->spark_shader = create_shader("assets/shaders/spark.vert", "assets/shaders/spark.frag", spark_attributes);
    renderer->trail_shader = create_shader("assets/shaders/trail.vert", "assets/shaders/trail.frag", trail_attributes);

    GLuint spark_block_index = glGetUniformBlockIndex(renderer->spark_shader, "SceneSettings");
    GLuint trail_block_index = glGetUniformBlockIndex(renderer->trail_shader, "SceneSettings");

    glUniformBlockBinding(renderer->spark_shader, spark_block_index, 0);
    glUniformBlockBinding(renderer->trail_shader, trail_block_index, 0);

    init_spark_mesh(renderer);
    init_trail_mesh(renderer);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void update_firework_buffers(FireworkRenderer* renderer, const Firework* fireworks) {
    renderer->spark_vertices = upload_spark_data(renderer, fireworks);

    int trail_count = 0;
    int index_count = 0;
    upload_trail_data(renderer, fireworks, &trail_count, &index_count);

    renderer->trail_vertices = trail_count;
    renderer->trail_indices = index_count;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void destroy_firework_renderer(FireworkRenderer* renderer) {
    glDeleteVertexArrays(1, &renderer->spark_vao);
    glDeleteVertexArrays(1, &renderer->trail_vao);
    
    glDeleteBuffers(1, &renderer->spark_vbo);
    glDeleteBuffers(1, &renderer->trail_vbo);
    glDeleteBuffers(1, &renderer->trail_ibo);

    glDeleteTextures(1, &renderer->spark_texture);
    glDeleteProgram(renderer->spark_shader);
    glDeleteProgram(renderer->trail_shader);
}
