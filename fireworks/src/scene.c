#include "scene.h"

#include "camera.h"
#include "firework_renderer.h"
#include "fireworks.h"
#include "gl_state.h"
#include "water.h"

#include <GL/glew.h>
#include <obj/model.h>
#include <SDL2/SDL_stdinc.h>

static void init_settings_ubo(GLuint* settings_ubo) {
    glGenBuffers(1, settings_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, *settings_ubo);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, *settings_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void init_scene(Scene* scene)
{
    scene->global_brightness = 1.0f;
    scene->particle_intensity = 1.0f;

    init_fireworks(scene->fireworks);
    init_firework_renderer(&scene->fire_renderer);
    init_water_renderer(&scene->water_renderer);

    init_settings_ubo(&scene->settings_ubo);
}

void update_scene_settings(Scene* scene) {
    glBindBuffer(GL_UNIFORM_BUFFER, scene->settings_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &scene->global_brightness);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float), sizeof(float), &scene->particle_intensity);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void update_scene(Scene* scene, float delta_time) {
    update_rising_fireworks(scene->fireworks, delta_time);
    update_exploded_fireworks(scene->fireworks, delta_time);

    update_firework_buffers(&scene->fire_renderer, scene->fireworks);
    update_water_buffers(&scene->water_renderer, scene->fireworks);
}

void render_scene(const Scene* scene, const Camera* camera, float current_time)
{
    set_state_depth_test(GL_TRUE);
    set_state_depth_mask(GL_TRUE);
    set_state_lighting(GL_TRUE);

    set_state_stencil_test(GL_TRUE);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    set_state_texture_2d(GL_TRUE);

    draw_water_surface(&scene->water_renderer, camera->position, current_time);
    render_reflection(scene);

    set_state_stencil_test(GL_FALSE);
    set_state_lighting(GL_FALSE);
    set_state_texture_2d(GL_FALSE);
    set_state_depth_test(GL_TRUE);
    set_state_depth_mask(GL_TRUE);
    set_state_blend(GL_TRUE);
    set_state_blend_function(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    draw_origin();

    set_state_depth_mask(GL_FALSE);
    set_state_blend_function(GL_SRC_ALPHA, GL_ONE);

    render_fireworks(&scene->fire_renderer, scene->fireworks);
}

void draw_origin()
{
    glBegin(GL_LINES);

    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0); glVertex3f(1, 0, 0);
    glVertex3f(1.1f, 0.1f, 0); glVertex3f(1.2f, -0.1f, 0);
    glVertex3f(1.1f, -0.1f, 0); glVertex3f(1.2f, 0.1f, 0);

    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0); glVertex3f(0, 1, 0);
    glVertex3f(0.1f, 1.1f, 0); glVertex3f(0.0f, 1.2f, 0);
    glVertex3f(-0.1f, 1.1f, 0); glVertex3f(0.0f, 1.2f, 0);
    glVertex3f(0.0f, 1.2f, 0); glVertex3f(0.0f, 1.3f, 0);

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0); glVertex3f(0, 0, 1);
    glVertex3f(-0.05f, 0.05f, 1.1f); glVertex3f(0.05f, 0.05f, 1.1f);
    glVertex3f(0.05f, 0.05f, 1.1f);  glVertex3f(-0.05f, -0.05f, 1.1f);
    glVertex3f(-0.05f, -0.05f, 1.1f); glVertex3f(0.05f, -0.05f, 1.1f);

    glEnd();
}

void render_reflection(const Scene* scene) {
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilMask(0x00);

    set_state_lighting(GL_FALSE);
    set_state_texture_2d(GL_FALSE);
    set_state_blend(GL_TRUE);
    set_state_blend_function(GL_SRC_ALPHA, GL_ONE);
    set_state_depth_test(GL_FALSE);
    set_state_depth_mask(GL_FALSE);

    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -2.0f);
        glScalef(1.0f, 1.0f, -1.0f);
        render_fireworks(&scene->fire_renderer, scene->fireworks);
    glPopMatrix();
}

void destroy_scene(Scene* scene) {
    free_model(&scene->model);

    destroy_firework_renderer(&scene->fire_renderer);
    destroy_water_renderer(&scene->water_renderer);
}
