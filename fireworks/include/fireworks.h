#ifndef FIREWORKS_H
#define FIREWORKS_H

#include "color.h"
#include "firework_pattern.h"
#include "spark.h"
#include "vec3.h"

#include <GL/glew.h>

typedef struct FireworkRenderer FireworkRenderer;

#define MAX_FIREWORKS 50
#define MAX_SPARKS 500

typedef enum FireworkState {
    FIREWORK_READY,
    FIREWORK_RISING,
    FIREWORK_EXPLODED
} FireworkState;

typedef struct Firework {
    FireworkState state;
    FireworkPattern pattern;
    vec3 position;
    vec3 speed;
    Spark sparks[MAX_SPARKS];
    Trail trails[MAX_SPARKS];
    int generation;
} Firework;

void init_fireworks(Firework* fireworks);
void launch_firework(Firework* fireworks, FireworkPattern pattern);
void launch_mini_explosion(Firework* fireworks, vec3 position, Color color, FireworkPattern pattern, int current_generation);
void update_rising_fireworks(Firework* fireworks, float delta_time);
void update_exploded_fireworks(Firework* fireworks, float delta_time);
void draw_spark_heads(const FireworkRenderer* renderer, GLuint spark_texture);
void draw_spark_trails(const FireworkRenderer* renderer);
void draw_rising_fireworks(const Firework* fireworks);
void draw_exploded_fireworks(const FireworkRenderer* renderer, GLuint spark_texture);
void render_fireworks(const FireworkRenderer* renderer, const Firework* fireworks, GLuint spark_texture);

#endif // FIREWORKS_H
