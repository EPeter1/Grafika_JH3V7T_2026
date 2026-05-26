#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "firework_renderer.h"
#include "fireworks.h"
#include "water.h"

#include <obj/model.h>

typedef struct Scene {
    Model model;
    GLuint settings_ubo;
    float global_brightness;
    float particle_intensity;
    Firework fireworks[MAX_FIREWORKS];
    FireworkRenderer fire_renderer;
    WaterRenderer water_renderer;
} Scene;

/**
 * Initialize the scene by loading models.
 */
void init_scene(Scene* scene);

/**
 * Update the scene.
 */
void update_scene(Scene* scene, float delta_time);

/**
 * Render the scene objects.
 */
void render_scene(const Scene* scene, const Camera* camera, float current_time);

/**
 * Draw the origin of the world coordinate system.
 */
void draw_origin();

void update_scene_settings(Scene* scene);
void render_reflection(const Scene* scene);
void destroy_scene(Scene* scene);

#endif // SCENE_H
