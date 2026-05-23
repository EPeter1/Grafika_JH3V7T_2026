#include "app.h"

#include "camera.h"
#include "event.h"
#include "fireworks.h"
#include "gl_state.h"
#include "scene.h"
#include "texture.h"
#include "ui.h"

#include <GL/glew.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>

#include <stdbool.h>
#include <stdio.h>

void init_app(App* app, int width, int height)
{
    int error_code;
    int inited_loaders;

    app->is_running = false;

    error_code = SDL_Init(SDL_INIT_EVERYTHING);
    if (error_code != 0) {
        printf("[ERROR] SDL initialization error: %s\n", SDL_GetError());
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    app->window = SDL_CreateWindow(
        "Fireworks!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL);
    if (app->window == NULL) {
        printf("[ERROR] Unable to create the application window!\n");
        return;
    }

    inited_loaders = IMG_Init(IMG_INIT_PNG);
    if (inited_loaders == 0) {
        printf("[ERROR] IMG initialization error: %s\n", IMG_GetError());
        return;
    }

    inited_loaders = TTF_Init();
    if (inited_loaders == -1) {
        printf("[ERROR] TTF initialization error: %s\n", TTF_GetError());
        return;
    }

    app->gl_context = SDL_GL_CreateContext(app->window);
    if (app->gl_context == NULL) {
        printf("[ERROR] Unable to create the OpenGL context!\n");
        return;
    }

    glewExperimental = GL_TRUE;
    GLenum error = glewInit();

    if (GLEW_OK != error) {
        printf("[ERROR] GLEW initialization error: %s\n", glewGetErrorString(error));
        return;
    }

    char* font_path = "assets/fonts/Orbitron-Regular.ttf";
    TTF_Font* font = TTF_OpenFont(font_path, 24);

    if (font) {
        init_user_interface(font);
        TTF_CloseFont(font);
    }
    else {
        printf("[ERROR] Failed to load font %s: %s\n", font_path, TTF_GetError());
        return;
    }

    SDL_GL_SetSwapInterval(1);

    init_opengl();
    reshape(width, height);

    init_camera(&(app->camera));
    init_scene(&(app->scene));

    app->background_texture = load_texture("assets/textures/menu_background.jpg");
    app->current_state = STATE_MAIN_MENU;
    app->menu_selection = 0;
    app->is_confirmed = false;
    app->is_help_shown = false;

    app->uptime = (double)SDL_GetTicks() / 1000.0;
    app->is_running = true;
}

void init_opengl()
{
    glShadeModel(GL_SMOOTH);

    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);

    glClearColor(0.1, 0.1, 0.1, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    set_state_depth_test(GL_TRUE);
    glClearDepth(1.0);

    set_state_texture_2d(GL_TRUE);

    set_state_lighting(GL_TRUE);
    glEnable(GL_LIGHT0);

    set_state_blend(GL_TRUE);
    set_state_blend_function(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_PROGRAM_POINT_SIZE);
}

void reshape(GLsizei width, GLsizei height)
{
    double aspect_ratio = (double)width / height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60.0, aspect_ratio, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void handle_app_events(App* app)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            app->is_running = false;
        }

        switch (app->current_state) {
            case STATE_MAIN_MENU:
                handle_menu_events(app, &event, MAIN_MENU_COUNT, 300, get_main_menu_label_size);
                break;

            case STATE_SIMULATION:
                handle_simulation_events(app, &event);
                break;

            case STATE_SETTINGS:
                handle_settings_events(app, &event);
                break;
            
            case STATE_PAUSED:
                handle_menu_events(app, &event, PAUSE_MENU_COUNT, 250, get_pause_menu_label_size);
                break;
        }
    }
}

void update_app(App* app)
{
    double current_time;
    double elapsed_time;

    current_time = (double)SDL_GetTicks() / 1000.0;
    elapsed_time = current_time - app->uptime;
    app->uptime = current_time;

    if (app->current_state == STATE_SIMULATION) {
        if (elapsed_time > 0.05) {
            elapsed_time = 0.05;
        }

        update_camera(&(app->camera), elapsed_time);
        update_scene(&(app->scene), elapsed_time);
    }
}

void render_app(App* app)
{
    set_state_depth_mask(GL_TRUE); 
    glStencilMask(0xFF);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    if (app->current_state == STATE_MAIN_MENU || 
       (app->current_state == STATE_SETTINGS && app->previous_state == STATE_MAIN_MENU)) {
        render_background(app->background_texture);
    }
    else {
        glPushMatrix();
        set_view(&(app->camera));
        render_scene(&(app->scene));
        glPopMatrix();
    }

    switch (app->current_state) {
        case STATE_MAIN_MENU:
            render_dim_overlay();
            render_menu(get_main_menu_labels(), MAIN_MENU_COUNT, app->menu_selection, app->is_confirmed, 300);
            break;

        case STATE_SETTINGS:
            render_dim_overlay();
            render_settings(app->menu_selection, app->scene.global_brightness, app->scene.particle_intensity);
            break;

        case STATE_SIMULATION:
            break;

        case STATE_PAUSED:
            render_dim_overlay();
            render_menu(get_pause_menu_labels(), PAUSE_MENU_COUNT, app->menu_selection, app->is_confirmed, 250);
            break;
    }

    if (app->is_help_shown) {
        render_help_overlay(app);
    }

    SDL_GL_SwapWindow(app->window);
}

void destroy_app(App* app)
{
    destroy_scene(&app->scene);
    destroy_user_interface();

    if (app->gl_context != NULL) {
        SDL_GL_DeleteContext(app->gl_context);
    }

    if (app->window != NULL) {
        SDL_DestroyWindow(app->window);
    }

    TTF_Quit();
    SDL_Quit();
}
