#include "camera.h"

#include "utils.h"
#include "vec3.h"

#include <GL/glew.h>
#include <GL/glu.h>

#include <math.h>

void init_camera(Camera* camera) {
    camera->position = (vec3){2.0f, 2.0f, 2.0f};
    camera->rotation = (vec3){-35.0f, 0.0f, 225.0f};
    camera->speed = (vec3){0.0f, 0.0f, 0.0f};
}

void update_camera(Camera* camera, double delta_time) {
    double angle = degree_to_radian(camera->rotation.z);
    double side_angle = degree_to_radian(camera->rotation.z + 90.0);

    camera->position.x += cos(angle) * camera->speed.y * delta_time;
    camera->position.y += sin(angle) * camera->speed.y * delta_time;
    camera->position.x += cos(side_angle) * camera->speed.x * delta_time;
    camera->position.y += sin(side_angle) * camera->speed.x * delta_time;

    camera->position.z += camera->speed.z * delta_time;
}

void set_view(const Camera* camera) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    double eye_x = camera->position.x;
    double eye_y = camera->position.y;
    double eye_z = camera->position.z;

    double radian_x = degree_to_radian(camera->rotation.x);
    double radian_z = degree_to_radian(camera->rotation.z);

    double center_x = eye_x + cos(radian_z) * cos(radian_x);
    double center_y = eye_y + sin(radian_z) * cos(radian_x);
    double center_z = eye_z + sin(radian_x);

    gluLookAt(eye_x, eye_y, eye_z,
        center_x, center_y, center_z,
        0.0, 0.0, 1.0);
}

void rotate_camera(Camera* camera, double horizontal, double vertical) {
    camera->rotation.z += horizontal;
    camera->rotation.x += vertical;

    if (camera->rotation.z < 0) {
        camera->rotation.z += 360.0;
    }

    if (camera->rotation.z > 360.0) {
        camera->rotation.z -= 360.0;
    }

    if (camera->rotation.x < -89.0) {
        camera->rotation.x = -89.0;
    }

    if (camera->rotation.x > 89.0) {
        camera->rotation.x = 89.0;
    }
}

void set_camera_speed(Camera* camera, double speed) {
    camera->speed.y = speed;
}

void set_camera_side_speed(Camera* camera, double speed) {
    camera->speed.x = speed;
}

void set_camera_vertical_speed(Camera* camera, double speed) {
    camera->speed.z = speed;
}

void set_orthogonal_view(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluOrtho2D(0.0, (double)width, (double)height, 0.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

void restore_perspective_view() {
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}
