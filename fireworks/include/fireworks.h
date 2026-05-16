#ifndef FIREWORKS_H
#define FIREWORKS_H

#include "color.h"
#include "firework_pattern.h"
#include "spark.h"
#include "vec3.h"

typedef struct Scene Scene;

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
    int generation;
} Firework;

void launch_firework(Scene* scene, FireworkPattern pattern);
void launch_mini_explosion(Scene* scene, vec3 position, Color color, FireworkPattern pattern, int current_generation);
void update_rising_firework(Firework* firework, float delta_time);
void update_exploded_firework(Scene* scene, Firework* firework, float delta_time);
void render_fireworks(const Scene* scene);

#endif // FIREWORKS_H
