#ifndef SPARK_H
#define SPARK_H

#include "color.h"
#include "firework_pattern.h"
#include "vec3.h"

#include <stdbool.h>

#define MAX_HISTORY 20

typedef struct Firework Firework;

typedef struct Trail {
    vec3 history[MAX_HISTORY];
    int length;
    int pointer;
    float timer;
} Trail;

typedef struct Spark {
    vec3 position;
    vec3 speed;
    int trail_index;
    float max_life;
    float current_life;
    Color color;
    Color render_color;
    float size;
    bool is_leader;
} Spark;

typedef struct PhysicsConfig {
    float drag;
    float gravity;
    int spark_count;
    float alpha_exponent;
} PhysicsConfig;

PhysicsConfig get_physics_config(FireworkPattern pattern);
int get_spark_index(const Firework* firework, const Spark* spark);
float get_spark_progress(const Spark* spark);
float get_spark_alpha(float progress, float exponent);
void set_spark_life(Spark* spark, float life);
void apply_spark_physics(Spark* spark, const PhysicsConfig config, float delta_time);
void apply_pattern_behavior(Firework* firework, Spark* spark, const PhysicsConfig config, float delta_time);
void apply_spark_visuals(const Firework* firework, const Spark* spark, Color* color, float* size);
void handle_ghost_logic(const Firework* firework, const Spark* spark, Color* color, float* size);
void update_spark_trail(Spark* spark, Trail* trails, float delta_time);
bool should_spark_render(const Firework* firework, const Spark* spark);

#endif // SPARK_H
