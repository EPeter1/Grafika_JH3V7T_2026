#ifndef SCENE_H
#define SCENE_H

#include "firework_renderer.h"
#include "fireworks.h"
#include "utils.h"

#include <obj/model.h>

typedef struct Scene
{
    Model model;
    Material material;
    GLuint water_texture;
    GLuint spark_texture;
    float global_brightness;
    float particle_intensity;
    Firework fireworks[MAX_FIREWORKS];
    FireworkRenderer renderer;
} Scene;

/**
 * Initialize the scene by loading models.
 */
void init_scene(Scene* scene);

/**
 * Set the lighting of the scene.
 */
void set_lighting(float brightness);

/**
 * Set the current material.
 */
void set_material(const Material* material);

/**
 * Update the scene.
 */
void update_scene(Scene* scene, float delta_time);

/**
 * Render the scene objects.
 */
void render_scene(const Scene* scene);

/**
 * Draw the origin of the world coordinate system.
 */
void draw_origin();

void draw_water_surface(GLuint texture_id, float brightness);
void render_reflection(const Scene* scene);
void destroy_scene(Scene* scene);

#endif /* SCENE_H */
