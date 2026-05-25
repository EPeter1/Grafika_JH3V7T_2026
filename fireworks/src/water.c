#include "water.h"

#include "color.h"
#include "fireworks.h"
#include "shader.h"
#include "spark.h"
#include "texture.h"
#include "vec3.h"

#include <GL/glew.h>

#include <stddef.h>

#define MAX_LIGHT_SOURCES 10

static const VertexAttribute water_attributes[] = {
    { 0, "a_position" },
    { 1, "a_texture_uv" },
    { 2, "a_normal" },
    { 0, NULL }
};

static void init_water_mesh(WaterRenderer* renderer) {
    float size = 2.0f;
    float water_z = -1.0f;

    WaterVertex vertices[4] = {
        { {-size, -size, water_z}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} },
        { { size, -size, water_z}, {4.0f, 0.0f}, {0.0f, 0.0f, 1.0f} },
        { {-size,  size, water_z}, {0.0f, 4.0f}, {0.0f, 0.0f, 1.0f} },
        { { size,  size, water_z}, {4.0f, 4.0f}, {0.0f, 0.0f, 1.0f} }
    };

    glGenBuffers(1, &renderer->water_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->water_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &renderer->water_vao);
    glBindVertexArray(renderer->water_vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->water_vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(WaterVertex), (void*)offsetof(WaterVertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(WaterVertex), (void*)offsetof(WaterVertex, texture_uv));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(WaterVertex), (void*)offsetof(WaterVertex, normal));
}

void init_water_renderer(WaterRenderer* renderer) {
    renderer->light_count = 0;

    renderer->water_texture = load_texture("assets/textures/water.png", GL_REPEAT, true);
    renderer->water_normal = load_texture("assets/textures/water_normal.png", GL_REPEAT, true);
    renderer->water_shader = create_shader("assets/shaders/water.vert", "assets/shaders/water.frag", water_attributes);

    GLuint water_block_index = glGetUniformBlockIndex(renderer->water_shader, "SceneSettings");
    glUniformBlockBinding(renderer->water_shader, water_block_index, 0);

    renderer->uniforms.water_texture = glGetUniformLocation(renderer->water_shader, "u_water_texture");
    renderer->uniforms.water_normal = glGetUniformLocation(renderer->water_shader, "u_normal_map");
    renderer->uniforms.view_position = glGetUniformLocation(renderer->water_shader, "u_view_position");
    renderer->uniforms.light_positions = glGetUniformLocation(renderer->water_shader, "u_light_positions");
    renderer->uniforms.light_colors = glGetUniformLocation(renderer->water_shader, "u_light_colors");
    renderer->uniforms.light_count = glGetUniformLocation(renderer->water_shader, "u_light_count");
    renderer->uniforms.current_time = glGetUniformLocation(renderer->water_shader, "u_current_time");

    init_water_mesh(renderer);
}

void update_water_buffers(WaterRenderer* renderer, const Firework* fireworks) {
    renderer->light_count = 0;

    for (int i = 0; i < MAX_FIREWORKS && renderer->light_count < MAX_LIGHT_SOURCES; i++) {
        const Firework* firework = &fireworks[i];

        if (firework->state == FIREWORK_RISING) {
            renderer->light_positions[renderer->light_count] = firework->position;
            renderer->light_colors[renderer->light_count] = (vec3){0.3f, 0.3f, 0.2f};
            renderer->light_count++;
        }
        else if (firework->state == FIREWORK_EXPLODED) {
            vec3 sum_position = { 0.0f, 0.0f, 0.0f };
            vec3 sum_color = { 0.0f, 0.0f, 0.0f };

            float total_alpha = 0.0f;
            int alive_sparks = 0;

            for (int j = 0; j < MAX_SPARKS; j++) {
                const Spark* spark = &firework->sparks[j];

                if (spark->current_life > 0.0f) {
                    Color actual_color = spark->render_color;

                    if (actual_color.alpha > 0.01f) {
                        sum_position = add_vec3(sum_position, spark->position);

                        sum_color.x += actual_color.red * actual_color.alpha;
                        sum_color.y += actual_color.green * actual_color.alpha;
                        sum_color.z += actual_color.blue * actual_color.alpha;

                        total_alpha += actual_color.alpha;
                        alive_sparks++;
                    }
                }
            }

            if (alive_sparks > 0) {
                renderer->light_positions[renderer->light_count] = scale_vec3(sum_position, 1.0f / (float)alive_sparks);

                float intensity = (total_alpha / (float)MAX_SPARKS) * 1.5f;
                if (intensity > 0.8f) {
                    intensity = 0.8f;
                }

                vec3 average_color = scale_vec3(sum_color, 1.0f / (float)alive_sparks);

                renderer->light_colors[renderer->light_count].x = average_color.x * intensity;
                renderer->light_colors[renderer->light_count].y = average_color.y * intensity;
                renderer->light_colors[renderer->light_count].z = average_color.z * intensity;

                renderer->light_count++;
            }
        }
    }
}

void draw_water_surface(const WaterRenderer* renderer, const vec3 camera_position, float current_time) {
    glUseProgram(renderer->water_shader);

    glUniform1i(renderer->uniforms.water_texture, 0);
    glUniform1i(renderer->uniforms.water_normal, 1);
    glUniform3fv(renderer->uniforms.view_position, 1, (float*)&camera_position);

    glUniform1i(renderer->uniforms.light_count, renderer->light_count);
    if (renderer->light_count > 0) {
        glUniform3fv(renderer->uniforms.light_positions, renderer->light_count, (float*)renderer->light_positions);
        glUniform3fv(renderer->uniforms.light_colors, renderer->light_count, (float*)renderer->light_colors);
    }

    glUniform1f(renderer->uniforms.current_time, current_time);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->water_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, renderer->water_normal);

    glBindVertexArray(renderer->water_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

void destroy_water_renderer(WaterRenderer* renderer) {
    glDeleteVertexArrays(1, &renderer->water_vao);
    glDeleteBuffers(1, &renderer->water_vbo);

    glDeleteTextures(1, &renderer->water_texture);
    glDeleteTextures(1, &renderer->water_normal);

    glDeleteProgram(renderer->water_shader);
}
