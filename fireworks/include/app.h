#ifndef APP_H
#define APP_H

#include "camera.h"
#include "scene.h"

#include <GL/glew.h>
#include <SDL2/SDL_video.h>

#include <stdbool.h>

typedef enum AppState {
    STATE_MAIN_MENU,
    STATE_SETTINGS,
    STATE_SIMULATION,
    STATE_PAUSED
} AppState;

typedef struct App {
    SDL_Window* window;
    SDL_GLContext gl_context;
    bool is_running;
    double uptime;
    Camera camera;
    Scene scene;

    AppState current_state;
    AppState previous_state;
    GLuint background_texture;
    int menu_selection;
    bool is_confirmed;
    bool is_help_shown;
} App;

/**
 * Initialize the application.
 */
void init_app(App* app, int width, int height);

/**
 * Initialize the OpenGL context.
 */
void init_opengl();

/**
 * Reshape the window.
 */
void reshape(GLsizei width, GLsizei height);

/**
 * Handle the events of the application.
 */
void handle_app_events(App* app);

/**
 * Update the application.
 */
void update_app(App* app);

/**
 * Render the application.
 */
void render_app(App* app);

/**
 * Destroy the application.
 */
void destroy_app(App* app);

#endif // APP_H
