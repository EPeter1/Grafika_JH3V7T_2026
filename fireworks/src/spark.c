#include "spark.h"

#include "color.h"
#include "firework_pattern.h"
#include "fireworks.h"
#include "utils.h"
#include "vec3.h"

#include <GL/glew.h>

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static const PhysicsConfig pattern_configs[PATTERN_COUNT] = {
    [PATTERN_COMET] = {.drag = 0.05f, .gravity = 0.1f, .spark_count = 100, .alpha_exponent = 0.5f},
    [PATTERN_CROSSETTE] = {.drag = 0.8f, .gravity = 0.1f, .spark_count = 20, .alpha_exponent = 0.5f},
    [PATTERN_PEONY] = {.drag = 1.0f, .gravity = 0.1f, .spark_count = 300, .alpha_exponent = 2.0f},
    [PATTERN_RING] = {.drag = 0.5f, .gravity = 0.2f, .spark_count = 300, .alpha_exponent = 1.0f},
    [PATTERN_WILLOW] = {.drag = 2.0f, .gravity = 0.15f, .spark_count = 300, .alpha_exponent = 0.5f},
    [PATTERN_PALM] = {.drag = 0.10f, .gravity = 0.15f, .spark_count = 40, .alpha_exponent = 1.0f},
    [PATTERN_FISH] = {.drag = 2.0f, .gravity = 0.05f, .spark_count = 20, .alpha_exponent = 2.0f},
    [PATTERN_STROBE] = {.drag = 0.4f, .gravity = 0.05f, .spark_count = 300, .alpha_exponent = 1.0f},
    [PATTERN_GHOST] = {.drag = 1.0f, .gravity = 0.05f, .spark_count = 300, .alpha_exponent = 2.0f},
    [PATTERN_TOURBILLION] = {.drag = 2.0f, .gravity = 0.1f, .spark_count = 10, .alpha_exponent = 1.0f},
    [PATTERN_NISHIKI_KAMURO] = {.drag = 1.0f, .gravity = 0.05f, .spark_count = 500, .alpha_exponent = 0.5f},
    [PATTERN_CHRYSANTHEMUM] = {.drag = 0.95f, .gravity = 0.15f, .spark_count = 300, .alpha_exponent = 2.0f}
};

PhysicsConfig get_physics_config(FireworkPattern pattern) {
    if (pattern >= PATTERN_COUNT) {
        printf("[ERROR] PATTERN_COUNT is invalid!");
        exit(1);
    }

    return pattern_configs[pattern];
}

int get_spark_index(const Firework* firework, const Spark* spark) {
    return (int)(spark - firework->sparks);
}

float get_spark_progress(const Spark* spark) {
    if (spark->max_life <= 0.0f) {
        return 1.0f;
    }
    float progress = 1.0f - (spark->current_life / spark->max_life);

    return clamp(progress, 0.0f, 1.0f);
}

float get_spark_alpha(float progress, float exponent) {
    float base = 1.0f - progress;

    if (exponent == 0.5f) {
        return sqrtf(base);
    }
    if (exponent == 1.0f) {
        return base;
    }
    if (exponent == 2.0f) {
        return base * base;
    }

    return powf(base, exponent);
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
    float progress = get_spark_progress(spark);
    float gravity_multiplier = 1.0f;

    if (firework->pattern == PATTERN_COMET && progress < 0.5f) {
        gravity_multiplier = 0.2f;
    }
    else if ((firework->pattern == PATTERN_PEONY || firework->pattern == PATTERN_GHOST)
        && progress < 0.75f) {
        gravity_multiplier = 0.0f;
    }

    float compensation = (1.0f - gravity_multiplier) * config.gravity * delta_time;
    spark->speed.z += compensation;

    int index = get_spark_index(firework, spark);

    if (firework->pattern == PATTERN_FISH && progress < 0.8f) {
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

    float progress = get_spark_progress(spark);
    const PhysicsConfig config = get_physics_config(firework->pattern);
    color->alpha = get_spark_alpha(progress, config.alpha_exponent);

    if (firework->pattern != PATTERN_GHOST && progress < 0.2f) {
        float t = clamp(1.0f - (progress / 0.2f), 0.0f, 1.0f);

        *color = mix_color(spark->color, get_color(COLOR_WHITE), t);
        *size += (t * 1.5f);
    }

    switch (firework->pattern) {
        case PATTERN_GHOST:
            handle_ghost_logic(firework, spark, color, size);
            break;

        case PATTERN_PALM:
            *size = 15.0f;
            break;

        case PATTERN_COMET:
            *size = spark->is_leader ? 15.0f : 0.0f;
            break;

        case PATTERN_NISHIKI_KAMURO:
            *size = 2.0f;
            if (progress > 0.8f) {
                *color = mix_color(spark->color, get_color(COLOR_RED), 0.5f);
            }
            break;

        default:
            break;
    }
}

void handle_ghost_logic(const Firework* firework, const Spark* spark, Color* color, float* size) {
    float progress = get_spark_progress(spark);
    float age = spark->max_life - spark->current_life;

    float expansion_duration = 1.0f;
    float wave_duration = 1.2f;

    int seed = (int)(firework->position.x * 100 + firework->position.y * 10);
    Color core_color = get_color((ColorName)(abs(seed) % COLOR_COUNT));
    Color shell_color = get_color((ColorName)((abs(seed) + 2) % COLOR_COUNT));
    Color wave_color = get_color((ColorName)((abs(seed) + 4) % COLOR_COUNT));

    if (spark->is_leader) {
        *color = core_color;
        *size = 2.5f;
        color->alpha = (progress > 0.8f) ? (1.0f - progress) * 5.0f : 1.0f;
    }
    else {
        float wave_seed = sinf(firework->position.x * 12.9898f) * 43758.5453f;
        float wave_phi = (wave_seed - floorf(wave_seed)) * 2.0f * M_PI;
        vec3 wave_direction = normalize_vec3((vec3){
            cosf(wave_phi),
            sinf(wave_phi),
            sinf(wave_phi * 0.5f)
        });

        float dot = dot_vec3(normalize_vec3(subtract_vec3(spark->position, firework->position)), wave_direction);
        float normalized_dot = (dot + 1.0f) * 0.5f;
        float wave_start_time = expansion_duration + (normalized_dot * wave_duration);

        if (age < wave_start_time) {
            *color = shell_color;
            *size = 1.5f;
            color->alpha = 1.0f;
        }
        else {
            float ghost_age = age - wave_start_time;
            float flash = expf(-ghost_age * 6.0f);

            *color = wave_color;
            *size = 3.0f + (flash * 12.0f);

            float life_after_wave = spark->current_life / (spark->max_life - wave_start_time);
            float alpha_fade = clamp(life_after_wave * 4.0f, 0.0f, 1.0f);
            color->alpha = clamp(alpha_fade + flash, 0.0f, 1.0f);
        }
    }
}

void update_spark_trail(Spark* spark, Trail* trails, float delta_time) {
    Trail* trail = &trails[spark->trail_index];
    float sample_rate = 0.0166f;

    trail->timer += delta_time;

    while (trail->timer >= sample_rate) {
        trail->history[trail->pointer] = spark->position;
        trail->pointer = (trail->pointer + 1) % MAX_HISTORY;
        trail->timer -= sample_rate;
    }
}

bool should_spark_render(const Firework* firework, const Spark* spark) {
    int index = get_spark_index(firework, spark);

    if (firework->pattern == PATTERN_STROBE) {
        float age = spark->max_life - spark->current_life;

        float frequency = 5.0f + (float)(index % 5);
        float phase_shift = (float)index * 0.8f;

        if (sinf(age * frequency * 2.0f * M_PI + phase_shift) < 0) {
            return false;
        }
    }

    if (firework->pattern == PATTERN_CHRYSANTHEMUM && index == 0) {
        return false;
    }

    return true;
}
