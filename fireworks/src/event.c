#include "event.h"

#include "app.h"
#include "camera.h"
#include "firework_pattern.h"
#include "fireworks.h"
#include "ui.h"
#include "utils.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>

void handle_menu_events(App* app, SDL_Event* event, int item_count, int start_y, void (*get_size)(int, int*, int*)) {
    int width;
    SDL_GL_GetDrawableSize(app->window, &width, NULL);

    if (app->is_help_shown) {
        if (event->type == SDL_KEYDOWN || event->type == SDL_MOUSEBUTTONDOWN) {
            app->is_help_shown = false;
        }

        return;
    }

    if (app->is_confirmed) {
        if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.scancode == SDL_SCANCODE_Y) {
                execute_confirmed_action(app);
            }
            if (event->key.keysym.scancode == SDL_SCANCODE_N) {
                app->is_confirmed = false;
            }
        }
        if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
            handle_confirm_mouse_event(app, event->button.x, event->button.y);
        }

        return;
    }

    if (event->type == SDL_MOUSEMOTION) {
        app->menu_selection = get_menu_selection_by_mouse(event->motion.x, event->motion.y, width, item_count, start_y, get_size);
    }

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        if (app->menu_selection != -1) {
            activate_menu_item(app);
        }
    }

    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.scancode) {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
                if (app->menu_selection <= 0) {
                    app->menu_selection = item_count - 1;
                }
                else {
                    app->menu_selection--;
                }
                break;

            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                app->menu_selection = (app->menu_selection + 1) % item_count;
                break;

            case SDL_SCANCODE_RETURN:
                if (app->menu_selection != -1) {
                    activate_menu_item(app);
                }
                break;

            case SDL_SCANCODE_ESCAPE:
                if (app->current_state == STATE_PAUSED) {
                    app->current_state = STATE_SIMULATION;
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                }
                break;

            default:
                break;
        }
    }
}

void handle_settings_events(App* app, SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                app->current_state = app->previous_state;
                break;

            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
                if (app->menu_selection <= 0) {
                    app->menu_selection = SETTINGS_MENU_COUNT - 1;
                }
                else {
                    app->menu_selection--;
                }
                break;

            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
                app->menu_selection = (app->menu_selection + 1) % SETTINGS_MENU_COUNT;
                break;

            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
                if (app->menu_selection == 0) {
                    app->scene.global_brightness = clamp(app->scene.global_brightness - 0.05f, 0.0f, 1.0f);
                }
                else {
                    app->scene.particle_intensity = clamp(app->scene.particle_intensity - 0.05f, 0.0f, 1.0f);
                }
                break;

            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
                if (app->menu_selection == 0) {
                    app->scene.global_brightness = clamp(app->scene.global_brightness + 0.05f, 0.0f, 1.0f);
                }
                else {
                    app->scene.particle_intensity = clamp(app->scene.particle_intensity + 0.05f, 0.0f, 1.0f);
                }
                break;

            default:
                break;
        }
    }

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        handle_settings_mouse_event(app, event->button.x, event->button.y);
    }
    else if (event->type == SDL_MOUSEMOTION && (event->motion.state & SDL_BUTTON_LMASK)) {
        if (app->menu_selection != -1) {
            handle_settings_mouse_event(app, event->motion.x, event->motion.y);
        }
    }
}

void handle_simulation_events(App* app, SDL_Event* event) {
    switch (event->type) {
        case SDL_KEYDOWN:
            switch (event->key.keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    app->current_state = STATE_PAUSED;
                    app->menu_selection = 0;
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    break;
                case SDL_SCANCODE_F1:
                    app->is_help_shown = !app->is_help_shown;
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

                default:
                    try_launch_firework(app, event->key.keysym.scancode);
                    break;
            }
            break;

        case SDL_KEYUP:
            switch (event->key.keysym.scancode) {
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

        case SDL_MOUSEMOTION: {
            float sensitivity = 0.1f;
            rotate_camera(&(app->camera), -event->motion.xrel * sensitivity, -event->motion.yrel * sensitivity);
            break;
        }

        case SDL_QUIT:
            app->is_running = false;
            break;

        default:
            break;
    }
}

void activate_menu_item(App* app) {
    if (app->current_state == STATE_MAIN_MENU) {
        switch ((MainMenuItem)app->menu_selection) {
            case MAIN_MENU_START:
                app->current_state = STATE_SIMULATION;
                SDL_SetRelativeMouseMode(SDL_TRUE);
                break;

            case MAIN_MENU_SETTINGS:
                app->previous_state = STATE_MAIN_MENU;
                app->current_state = STATE_SETTINGS;
                app->menu_selection = 0;
                break;

            case MAIN_MENU_HELP:
                app->is_help_shown = true;
                break;

            case MAIN_MENU_EXIT:
                app->is_confirmed = true;
                break;

            default:
                break;
        }
    }
    else if (app->current_state == STATE_PAUSED) {
        switch ((PauseMenuItem)app->menu_selection) {
            case PAUSE_MENU_RESUME:
                app->current_state = STATE_SIMULATION;
                SDL_SetRelativeMouseMode(SDL_TRUE);
                break;

            case PAUSE_MENU_SETTINGS:
                app->previous_state = STATE_PAUSED;
                app->current_state = STATE_SETTINGS;
                app->menu_selection = 0;
                break;

            case PAUSE_MENU_HELP:
                app->is_help_shown = true;
                break;

            case PAUSE_MENU_EXIT_TO_MENU:
            case PAUSE_MENU_EXIT_APP:
                app->is_confirmed = true;
                break;

            default:
                break;
        }
    }
}

void execute_confirmed_action(App* app) {
    if (app->current_state == STATE_PAUSED && (PauseMenuItem)app->menu_selection == PAUSE_MENU_EXIT_TO_MENU) {
        app->current_state = STATE_MAIN_MENU;
        app->menu_selection = 0;
    }
    else {
        app->is_running = false;
    }

    app->is_confirmed = false;
}

bool is_mouse_over_item(int mouse_x, int mouse_y, int item_x, int item_y, int item_width, int item_height) {
    return (mouse_x >= item_x &&
            mouse_x <= item_x + item_width &&
            mouse_y >= item_y &&
            mouse_y <= item_y + item_height);
}

int get_menu_selection_by_mouse(int mouse_x, int mouse_y, int screen_width, int item_count, int start_y, void (*get_size)(int, int*, int*)) {
    for (int i = 0; i < item_count; i++) {
        int item_width;
        int item_height;
        get_size(i, &item_width, &item_height);

        int item_x = screen_width / 2 - item_width / 2;
        int item_y = start_y + i * 60;

        if (is_mouse_over_item(mouse_x, mouse_y, item_x, item_y, item_width, item_height)) {
            return i;
        }
    }

    return -1;
}

void handle_settings_mouse_event(App* app, int x, int y) {
    int width;
    SDL_GL_GetDrawableSize(app->window, &width, NULL);

    int bar_width = 200;
    int bar_x_start = width / 2 + 50;
    int current_hover = -1;

    for (int i = 0; i < SETTINGS_MENU_COUNT; i++) {
        int text_y = 300 + i * 100;

        if (y >= text_y && y <= text_y + 40) {
            current_hover = i;

            float new_value = (float)(x - bar_x_start) / bar_width;
            new_value = clamp(new_value, 0.0f, 1.0f);

            float* target_property = (i == 0) ? &app->scene.global_brightness : &app->scene.particle_intensity;
            *target_property = new_value;

            app->menu_selection = i;
        }
    }

    app->menu_selection = current_hover;
}

void handle_confirm_mouse_event(App* app, int mouse_x, int mouse_y) {
    int width;
    int height;
    SDL_GL_GetDrawableSize(app->window, &width, &height);

    int button_width = 120;
    int button_height = 50;
    int button_y = height / 2;
    int spacing = 40;

    int yes_x = width / 2 - button_width - spacing;
    if (is_mouse_over_item(mouse_x, mouse_y, yes_x, button_y, button_width, button_height)) {
        execute_confirmed_action(app);
    }

    int no_x = width / 2 + spacing;
    if (is_mouse_over_item(mouse_x, mouse_y, no_x, button_y, button_width, button_height)) {
        app->is_confirmed = false;
    }
}

void try_launch_firework(App* app, SDL_Scancode scancode) {
    struct { SDL_Scancode code; FireworkPattern pattern; } patterns[] = {
        { SDL_SCANCODE_SPACE, PATTERN_PEONY },
        { SDL_SCANCODE_U,     PATTERN_COMET },
        { SDL_SCANCODE_I,     PATTERN_CROSSETTE },
        { SDL_SCANCODE_O,     PATTERN_RING },
        { SDL_SCANCODE_P,     PATTERN_WILLOW },
        { SDL_SCANCODE_H,     PATTERN_PALM },
        { SDL_SCANCODE_J,     PATTERN_FISH },
        { SDL_SCANCODE_K,     PATTERN_STROBE },
        { SDL_SCANCODE_L,     PATTERN_GHOST },
        { SDL_SCANCODE_B,     PATTERN_TOURBILLION },
        { SDL_SCANCODE_N,     PATTERN_NISHIKI_KAMURO },
        { SDL_SCANCODE_M,     PATTERN_CHRYSANTHEMUM }
    };

    int count = sizeof(patterns) / sizeof(patterns[0]);
    for (int i = 0; i < count; i++) {
        if (patterns[i].code == scancode) {
            launch_firework(&(app->scene), patterns[i].pattern);
            break;
        }
    }
}
