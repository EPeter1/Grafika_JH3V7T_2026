#include "fireworks.h"

#include "audio.h"
#include "color.h"
#include "explosion.h"
#include "firework_pattern.h"
#include "firework_renderer.h"
#include "gl_state.h"
#include "spark.h"
#include "utils.h"
#include "vec3.h"

#include <GL/glew.h>

#include <stdbool.h>
#include <stdlib.h>

void init_fireworks(Firework* fireworks) {
    for (int i = 0; i < MAX_FIREWORKS; i++) {
        fireworks[i].state = FIREWORK_READY;
    }
}

void launch_firework(Firework* fireworks, FireworkPattern pattern, vec3 camera_position) {
    for (int i = 0; i < MAX_FIREWORKS; i++) {
        Firework* firework = &fireworks[i];

        if (firework->state == FIREWORK_READY) {

            firework->state = FIREWORK_RISING;
            firework->pattern = pattern;
            firework->generation = 0;

            set_vec3_scalar(&firework->position, 0.0f);

            set_vec3(&firework->speed, 
                rand_range(-0.1f, 0.1f),
                rand_range(-0.1f, 0.1f),
                rand_range(0.6f, 0.9f)
            );

            if (rand_range(0.0f, 1.0f) <= 0.5f) {
                SoundType launch_sound = (rand() % 2 == 0) ? SOUND_BANG : SOUND_WHISTLE;
                play_firework_sound(launch_sound, firework->position, camera_position);
            }

            break;
        }
    }
}

void launch_mini_explosion(Firework* fireworks, FireworkPattern pattern, vec3 camera_position, vec3 firework_position, Color color, int current_generation) {
    if (current_generation >= 1) {
        return; 
    }

    for (int i = 0; i < MAX_FIREWORKS; i++) {
        Firework* mini = &fireworks[i];

        if (mini->state == FIREWORK_READY) {

            mini->state = FIREWORK_EXPLODED;
            mini->pattern = pattern;
            mini->position = firework_position;
            mini->generation = current_generation + 1;

            init_explosion(mini, 15, color);
            play_firework_sound(SOUND_FIZZLE, mini->position, camera_position);

            break;
        }
    }
}

void update_rising_fireworks(Firework* fireworks, vec3 camera_position, float delta_time) {
    for (int i = 0; i < MAX_FIREWORKS; i++) {
        Firework* firework = &fireworks[i];

        if (firework->state != FIREWORK_RISING) {
            continue;
        }

        firework->position = add_vec3(firework->position, scale_vec3(firework->speed, delta_time));
        firework->speed.z -= 0.4f * delta_time;

        if (firework->speed.z <= 0.0f) {
            int spark_count = get_physics_config(firework->pattern).spark_count;
            Color random_color = get_color((ColorName)(rand() % COLOR_COUNT));

            init_explosion(firework, spark_count, random_color);
            firework->state = FIREWORK_EXPLODED;

            play_firework_sound(SOUND_BANG, firework->position, camera_position);
        }
    }
}

void update_exploded_fireworks(Firework* fireworks, vec3 camera_position, float delta_time) {
    for (int i = 0; i < MAX_FIREWORKS; i++) {
        Firework* firework = &fireworks[i];

        if (firework->state != FIREWORK_EXPLODED) {
            continue;
        }

        const PhysicsConfig config = get_physics_config(firework->pattern);
        bool any_alive = false;

        float timer = firework->sparks[0].current_life;

        for (int j = 0; j < MAX_SPARKS; j++) {
            Spark* spark = &firework->sparks[j];

            if (spark->current_life <= 0.0f) {
                continue;
            }
            any_alive = true;

            update_spark_trail(spark, firework->trails, delta_time);

            if (firework->pattern == PATTERN_CROSSETTE && get_spark_progress(spark) > 0.9f) {
                launch_mini_explosion(fireworks, PATTERN_CROSSETTE, camera_position, spark->position, spark->color, firework->generation);
                spark->current_life = 0.0f;

                continue;
            }

            apply_spark_physics(spark, config, delta_time);
            apply_pattern_behavior(firework, spark, config, delta_time);

            if (should_spark_render(firework, spark)) {
                apply_spark_visuals(firework, spark, &spark->render_color, &spark->size);
            }
            else {
                spark->render_color.alpha = 0.0f;
            }

            spark->current_life -= delta_time;
        }

        if (firework->pattern == PATTERN_CHRYSANTHEMUM) {
            if (timer > 0.0f && firework->sparks[0].current_life <= 0.0f) {
                Color parent_color = firework->sparks[1].color;

                for (int j = 0; j < MAX_FIREWORKS; j++) {
                    vec3 offset = {
                        rand_symmetric(0.4f),
                        rand_symmetric(0.4f),
                        rand_symmetric(0.4f)
                    };

                    launch_mini_explosion(fireworks, PATTERN_PEONY, camera_position, add_vec3(firework->position, offset), parent_color, firework->generation);
                }
            }
        }

        if (!any_alive) {
            firework->state = FIREWORK_READY;
        }
    }
}

void draw_spark_heads(const FireworkRenderer* renderer) {
    if (renderer->spark_vertices <= 0) {
        return;
    }
    glUseProgram(renderer->spark_shader);

    set_gl_state_texture_2d(true);
    glBindTexture(GL_TEXTURE_2D, renderer->spark_texture);

    glEnable(GL_POINT_SPRITE);
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

    glBindVertexArray(renderer->spark_vao);
    glDrawArrays(GL_POINTS, 0, renderer->spark_vertices);

    glDisable(GL_POINT_SPRITE);
}

void draw_spark_trails(const FireworkRenderer* renderer) {
    if (renderer->trail_vertices <= 0) {
        return;
    }
    glUseProgram(renderer->trail_shader);

    set_gl_state_texture_2d(false);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(PRIMITIVE_RESTART_INDEX);

    glLineWidth(2.0f);

    glBindVertexArray(renderer->trail_vao);
    glDrawElements(GL_LINE_STRIP, renderer->trail_indices, GL_UNSIGNED_INT, 0);

    glDisable(GL_PRIMITIVE_RESTART);
}

void draw_rising_fireworks(const Firework* fireworks) {
    bool rising_started = false;

    for (int i = 0; i < MAX_FIREWORKS; i++) {
        const Firework* firework = &fireworks[i];

        if (firework->state == FIREWORK_RISING) {
            if (!rising_started) {
                glPointSize(5.0f);
                glBegin(GL_POINTS);
                glColor3f(1.0f, 1.0f, 1.0f);
                rising_started = true;
            }
            glVertex3f(firework->position.x, firework->position.y, firework->position.z);
        }
    }

    if (rising_started) {
        glEnd();
    }
}

void draw_exploded_fireworks(const FireworkRenderer* renderer) {
    draw_spark_heads(renderer);
    draw_spark_trails(renderer);

    glBindVertexArray(0);
}

void render_fireworks(const FireworkRenderer* renderer, const Firework* fireworks) {
    draw_rising_fireworks(fireworks);
    draw_exploded_fireworks(renderer);

    glUseProgram(0);
}
