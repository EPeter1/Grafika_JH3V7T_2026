#include "scene.h"

#include "fireworks.h"
#include "gl_state.h"
#include "texture.h"
#include "utils.h"

#include <GL/glew.h>
#include <obj/draw.h>
#include <obj/load.h>

void init_scene(Scene* scene)
{
    scene->texture_id = load_texture("assets/textures/water.png");

    scene->material.ambient.red = 0.0;
    scene->material.ambient.green = 0.0;
    scene->material.ambient.blue = 0.0;

    scene->material.diffuse.red = 1.0;
    scene->material.diffuse.green = 1.0;
    scene->material.diffuse.blue = 1.0;

    scene->material.specular.red = 0.0;
    scene->material.specular.green = 0.0;
    scene->material.specular.blue = 0.0;

    scene->material.shininess = 0.0;

    scene->global_brightness = 1.0f;
    scene->particle_intensity = 1.0f;

    for (int i = 0; i < MAX_FIREWORKS; i++) {
        scene->fireworks[i].state = FIREWORK_READY;
    }
}

void set_lighting(float brightness)
{
    float ambient_light[] = { 0.05f, 0.05f, 0.05f, 1.0f };
    float diffuse_light[] = { brightness, brightness, brightness, 1.0f };
    float specular_light[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float position[] = { 0.0f, 0.0f, 10.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void set_material(const Material* material)
{
    float ambient_material_color[] = {
        material->ambient.red,
        material->ambient.green,
        material->ambient.blue,
        1.0f
    };

    float diffuse_material_color[] = {
        material->diffuse.red,
        material->diffuse.green,
        material->diffuse.blue,
        1.0f
    };

    float specular_material_color[] = {
        material->specular.red,
        material->specular.green,
        material->specular.blue,
        1.0f
    };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_material_color);

    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
}

void update_scene(Scene* scene, float delta_time) {
    for (int i = 0; i < MAX_FIREWORKS; i++) {
        Firework* firework = &(scene->fireworks[i]);
        
        if (firework->state == FIREWORK_RISING) {
            update_rising_firework(firework, delta_time);
        }
        else if (firework->state == FIREWORK_EXPLODED) {
            update_exploded_firework(scene, firework, delta_time);
        }
    }
}

void render_scene(const Scene* scene)
{
    set_state_depth_test(GL_TRUE);
    set_state_depth_mask(GL_TRUE);
    set_state_lighting(GL_TRUE);

    set_material(&(scene->material));
    set_lighting(scene->global_brightness);

    set_state_stencil_test(GL_TRUE);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    set_state_texture_2d(GL_TRUE);

    draw_water_surface(scene->texture_id, scene->global_brightness);
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

    render_fireworks(scene);
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

void draw_water_surface(GLuint texture_id, float brightness) {
    float size = 2.0f;
    float water_z = -1.0f;

    set_state_texture_2d(GL_TRUE);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glColor3f(brightness, brightness, brightness);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, water_z);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(size, -size, water_z);

        glTexCoord2f(1.0f, 1.0f); glVertex3f(size, size, water_z);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size, water_z);
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
        render_fireworks(scene);
    glPopMatrix();
}
