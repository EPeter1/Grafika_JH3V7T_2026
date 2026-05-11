#include <GL/glew.h>

#include "app.h"
#include "fireworks.h"

#include <GL/glu.h>
#include <SDL2/SDL_image.h>

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

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

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

    SDL_GL_SetSwapInterval(1);
    
    init_opengl();
    reshape(width, height);

    init_camera(&(app->camera));
    init_scene(&(app->scene));

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

    glEnable(GL_DEPTH_TEST);

    glClearDepth(1.0);

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
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
    static bool is_mouse_down = false;
    static int mouse_x = 0;
    static int mouse_y = 0;
    int x;
    int y;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                app->is_running = false;
                break;
            case SDL_SCANCODE_W:
                set_camera_speed(&(app->camera), 1);
                break;
            case SDL_SCANCODE_S:
                set_camera_speed(&(app->camera), -1);
                break;
            case SDL_SCANCODE_A:
                set_camera_side_speed(&(app->camera), 1);
                break;
            case SDL_SCANCODE_D:
                set_camera_side_speed(&(app->camera), -1);
                break;
            case SDL_SCANCODE_Q:
                set_camera_vertical_speed(&(app->camera), 1);
                break;
            case SDL_SCANCODE_E:
                set_camera_vertical_speed(&(app->camera), -1);
                break;
            case SDL_SCANCODE_SPACE:
                launch_firework(&(app->scene), PATTERN_PEONY);
                break;
            case SDL_SCANCODE_U:
                launch_firework(&(app->scene), PATTERN_COMET);
                break;
            case SDL_SCANCODE_I:
                launch_firework(&(app->scene), PATTERN_CROSSETTE);
                break;
            case SDL_SCANCODE_O:
                launch_firework(&(app->scene), PATTERN_RING);
                break;
            case SDL_SCANCODE_P:
                launch_firework(&(app->scene), PATTERN_WILLOW);
                break;
            case SDL_SCANCODE_H:
                launch_firework(&(app->scene), PATTERN_PALM);
                break;
            case SDL_SCANCODE_J:
                launch_firework(&(app->scene), PATTERN_FISH);
                break;
            case SDL_SCANCODE_K:
                launch_firework(&(app->scene), PATTERN_STROBE);
                break;
            case SDL_SCANCODE_L:
                launch_firework(&(app->scene), PATTERN_GHOST);
                break;
            case SDL_SCANCODE_B:
                launch_firework(&(app->scene), PATTERN_TOURBILLION);
                break;
            case SDL_SCANCODE_N:
                launch_firework(&(app->scene), PATTERN_NISHIKI_KAMURO);
                break;
            case SDL_SCANCODE_M:
                launch_firework(&(app->scene), PATTERN_CHRYSANTHEMUM);
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_S:
                set_camera_speed(&(app->camera), 0);
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_D:
                set_camera_side_speed(&(app->camera), 0);
                break;
            case SDL_SCANCODE_Q:
            case SDL_SCANCODE_E:
                set_camera_vertical_speed(&(app->camera), 0);
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            is_mouse_down = true;
            break;
        case SDL_MOUSEMOTION:
            SDL_GetMouseState(&x, &y);
            if (is_mouse_down) {
                rotate_camera(&(app->camera), mouse_x - x, mouse_y - y);
            }
            mouse_x = x;
            mouse_y = y;
            break;
        case SDL_MOUSEBUTTONUP:
            is_mouse_down = false;
            break;
        case SDL_QUIT:
            app->is_running = false;
            break;
        default:
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

    if (elapsed_time > 0.05) {
        elapsed_time = 0.05;
    }

    update_camera(&(app->camera), elapsed_time);
    update_scene(&(app->scene), elapsed_time);
}

void render_app(App* app)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    set_view(&(app->camera));
    render_scene(&(app->scene));
    glPopMatrix();

    SDL_GL_SwapWindow(app->window);
}

void destroy_app(App* app)
{
    if (app->gl_context != NULL) {
        SDL_GL_DeleteContext(app->gl_context);
    }

    if (app->window != NULL) {
        SDL_DestroyWindow(app->window);
    }

    SDL_Quit();
}
