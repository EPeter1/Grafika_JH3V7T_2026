#include <GL/glew.h>

#include "color.h"
#include "fireworks.h"
#include "spark.h"
#include "utils.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static const PhysicsConfig pattern_configs[PATTERN_COUNT] = {
    [PATTERN_COMET] = {.drag = 0.05f, .gravity = 0.1f, .spark_count = 100},
    [PATTERN_CROSSETTE] = {.drag = 0.8f, .gravity = 0.1f, .spark_count = 20},
    [PATTERN_PEONY] = {.drag = 1.0f, .gravity = 0.1f, .spark_count = 300},
    [PATTERN_RING] = {.drag = 0.5f, .gravity = 0.2f, .spark_count = 300},
    [PATTERN_WILLOW] = {.drag = 2.0f, .gravity = 0.15f, .spark_count = 300},
    [PATTERN_PALM] = {.drag = 0.10f, .gravity = 0.15f, .spark_count = 40},
    [PATTERN_FISH] = {.drag = 2.0f, .gravity = 0.05f, .spark_count = 20},
    [PATTERN_STROBE] = {.drag = 0.4f, .gravity = 0.05f, .spark_count = 300},
    [PATTERN_GHOST] = {.drag = 1.0f, .gravity = 0.05f, .spark_count = 300},
    [PATTERN_TOURBILLION] = {.drag = 2.0f, .gravity = 0.1f, .spark_count = 10},
    [PATTERN_NISHIKI_KAMURO] = {.drag = 1.0f, .gravity = 0.05f, .spark_count = 500},
    [PATTERN_CHRYSANTHEMUM] = {.drag = 0.95f, .gravity = 0.15f, .spark_count = 300}
};

PhysicsConfig get_physics_config(FireworkPattern pattern) {
    if (pattern >= PATTERN_COUNT) {
        printf("[ERROR] PATTERN_COUNT is invalid!");
        exit(1);
    }

    return pattern_configs[pattern];
}

float get_spark_progress(const Spark* spark) {
    if (spark->max_life <= 0.0f) {
        return 1.0f;
    }
    float progress = 1.0f - (spark->current_life / spark->max_life);

    return clamp(progress, 0.0f, 1.0f);
}

int get_spark_index(const Firework* firework, const Spark* spark) {
    return (int)(spark - firework->sparks);
}

void set_spark_life(Spark* spark, float life) {
    spark->max_life = life;
    spark->current_life = life;
}

void apply_spark_physics(Spark* spark, const PhysicsConfig config, float delta_time) {
    vec3 drag_force = scale_vec3(spark->speed, config.drag * delta_time);
    spark->speed = subtract_vec3(spark->speed, drag_force);

    spark->speed.z -= config.gravity * delta_time;

    spark->position = add_vec3(spark->position, scale_vec3(spark->speed, delta_time));
}

void apply_pattern_behavior(Firework* firework, Spark* spark, const PhysicsConfig config, float delta_time) {
    float spark_progress = get_spark_progress(spark);
    float gravity_multiplier = 1.0f;

    if (firework->pattern == PATTERN_COMET && spark_progress < 0.5f) {
        gravity_multiplier = 0.2f;
    }
    else if (firework->pattern == PATTERN_PEONY && spark_progress < 0.75f) {
        gravity_multiplier = 0.0f;
    }

    float compensation = (1.0f - gravity_multiplier) * config.gravity * delta_time;
    spark->speed.z += compensation;

    int index = get_spark_index(firework, spark);

    if (firework->pattern == PATTERN_FISH && spark_progress < 0.8f) {
        float frequency = 25.0f;
        float amplitude = 60.0f;

        vec3 fish_movement = {
            sinf(spark->current_life * frequency + index * 13.5f) * amplitude,
            cosf(spark->current_life * (frequency * 0.9f) + index * 21.2f) * amplitude,
            sinf(spark->current_life * (frequency * 1.1f) + index * 7.7f) * amplitude
        };

        spark->speed = add_vec3(spark->speed, scale_vec3(fish_movement, delta_time));
    }
    else if (firework->pattern == PATTERN_TOURBILLION) {
        float age = spark->max_life - spark->current_life;

        float spin_frequency = 8.0f;
        float lift_force = 0.35f;
        float vortex_strength = 10.0f;
        float vortex_expansion = 1.0f + (0.5f * age);

        vec3 serpent_movement = {
            sinf(age * spin_frequency + index) * vortex_expansion,
            cosf(age * spin_frequency + index) * vortex_expansion,
            lift_force
        };

        spark->speed = add_vec3(spark->speed, scale_vec3(serpent_movement, vortex_strength * delta_time));
        spark->speed = scale_vec3(spark->speed, 1.0f - (0.4f * delta_time));
    }
}

void apply_spark_visuals(const Firework* firework, const Spark* spark, Color* color, float* size) {
    *color = spark->color;
    *size = 3.0f;

    float spark_progress = get_spark_progress(spark);

    if (spark_progress < 0.2f) {
        float t = 1.0f - (spark_progress / 0.2f);
        t = clamp(t, 0.0f, 1.0f);

        *color = mix_color(spark->color, get_color(COLOR_WHITE), t);
        *size += (t * 1.5f);
    }

    float alpha_base = 1.0f - spark_progress;
    color->alpha = alpha_base * alpha_base;

    switch (firework->pattern) {
        case PATTERN_GHOST:
            float delay_duration = 0.8f; 
            float age = spark->max_life - spark->current_life;

            if (age > delay_duration) {
                float ghost = (age - delay_duration) / (spark->max_life - delay_duration);

                Color random_color = get_color((ColorName)(rand() % COLOR_COUNT));
                *color = mix_color(random_color, spark->color, ghost);

                float flash = expf(-ghost * 5.0f); 
                *size = (4.0f + ghost * 2.0f) + (flash * 10.0f);
                color->alpha = clamp(ghost + flash, 0.0f, 1.0f);
            }
            else {
                color->alpha = 0.0f;
            }

            break;

        case PATTERN_PALM:
            *size = 15.0f;
            break;

        case PATTERN_COMET:
            *size = spark->is_leader ? 15.0f : 0.0f;
            break;

        case PATTERN_NISHIKI_KAMURO:
            *size = 2.0f;
            if (get_spark_progress(spark) > 0.8f) {
                *color = mix_color(spark->color, get_color(COLOR_RED), 0.5f);
            }
            break;

        default:
            break;
    }
}

void update_spark_trail(Spark* spark, float delta_time) {
    float sample_rate = 0.0166f;
    spark->trail.timer += delta_time;

    while (spark->trail.timer >= sample_rate) {
        spark->trail.history[spark->trail.pointer] = spark->position;
        spark->trail.pointer = (spark->trail.pointer + 1) % spark->trail.length;
        spark->trail.timer -= sample_rate;
    }
}

void draw_spark_trail(const Spark* spark) {
    if (spark->trail.length <= 1) {
        return;
    }

    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);

        Color color = spark->color;
        vec3 position = spark->position;

        glColor4f(color.red, color.green, color.blue, spark->current_life);
        glVertex3f(position.x, position.y, position.z);

        for (int i = 0; i < spark->trail.length; i++) {
            int index = (spark->trail.pointer - 1 - i + spark->trail.length) % spark->trail.length;

            float trail_fade = 1.0f - ((float)i / (float)spark->trail.length);
            vec3 history = spark->trail.history[index];

            glColor4f(color.red, color.green, color.blue, spark->current_life * trail_fade);
            glVertex3f(history.x, history.y, history.z);
        }
    glEnd();
}

void draw_spark_head(vec3 position, Color color, float size) {
    if (size <= 0.0f || color.alpha <= 0.01f) {
        return;
    }

    glPointSize(size);
    glBegin(GL_POINTS);
        glColor4f(color.red, color.green, color.blue, color.alpha);
        glVertex3f(position.x, position.y, position.z);
    glEnd();
}

void render_spark(const Firework* firework, const Spark* spark) {
    int index = get_spark_index(firework, spark);

    if (firework->pattern == PATTERN_STROBE) {
        float age = spark->max_life - spark->current_life;

        float frequency = 5.0f + (float)(index % 5);
        float phase_shift = (float)index * 0.8f;

        if (sinf(age * frequency * 2.0f * M_PI + phase_shift) < 0) {
            return;
        }
    }

    if (firework->pattern == PATTERN_CHRYSANTHEMUM && index == 0) {
        return;
    }

    if (firework->pattern == PATTERN_GHOST && get_spark_progress(spark) < 0.2f) {
        return;
    }

    Color spark_color;
    float spark_size;
    apply_spark_visuals(firework, spark, &spark_color, &spark_size);

    draw_spark_trail(spark);
    draw_spark_head(spark->position, spark_color, spark_size);
}
