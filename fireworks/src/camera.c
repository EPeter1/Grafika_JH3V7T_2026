#include <GL/glew.h>

#include "camera.h"
#include "utils.h"

#include <GL/glu.h>
#include <math.h>

void init_camera(Camera* camera)
{
    camera->position.x = 2.0;
    camera->position.y = 2.0;
    camera->position.z = 2.0;

    camera->rotation.x = 0.0;
    camera->rotation.y = 0.0;
    camera->rotation.z = 0.0;

    camera->speed.x = 0.0;
    camera->speed.y = 0.0;
    camera->speed.z = 0.0;
}

void update_camera(Camera* camera, double time)
{
    double angle;
    double side_angle;

    angle = degree_to_radian(camera->rotation.z);
    side_angle = degree_to_radian(camera->rotation.z + 90.0);

    camera->position.x += cos(angle) * camera->speed.y * time;
    camera->position.y += sin(angle) * camera->speed.y * time;
    camera->position.x += cos(side_angle) * camera->speed.x * time;
    camera->position.y += sin(side_angle) * camera->speed.x * time;

    camera->position.z += camera->speed.z * time;
}

void set_view(const Camera* camera)
{
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
        0, 0, 1);
}

void rotate_camera(Camera* camera, double horizontal, double vertical)
{
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

void set_camera_speed(Camera* camera, double speed)
{
    camera->speed.y = speed;
}

void set_camera_side_speed(Camera* camera, double speed)
{
    camera->speed.x = speed;
}

void set_camera_vertical_speed(Camera* camera, double speed)
{
    camera->speed.z = speed;
}
