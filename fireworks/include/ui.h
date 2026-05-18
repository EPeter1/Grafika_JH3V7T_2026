#ifndef UI_H
#define UI_H

#include "app.h"
#include "color.h"

#include <GL/glew.h>
#include <SDL2/SDL_ttf.h>

#include <stdbool.h>

#define SETTINGS_MENU_COUNT 2
#define CONFIRM_MENU_COUNT 3

typedef enum MainMenuItem {
    MAIN_MENU_START,
    MAIN_MENU_SETTINGS,
    MAIN_MENU_HELP,
    MAIN_MENU_EXIT,
    MAIN_MENU_COUNT
} MainMenuItem;

typedef enum PauseMenuItem {
    PAUSE_MENU_RESUME,
    PAUSE_MENU_SETTINGS,
    PAUSE_MENU_HELP,
    PAUSE_MENU_EXIT_TO_MENU,
    PAUSE_MENU_EXIT_APP,
    PAUSE_MENU_COUNT
} PauseMenuItem;

typedef struct Label {
    GLuint texture_id;
    int width;
    int height;
} Label;

Label* get_main_menu_labels();
Label* get_pause_menu_labels();
void get_main_menu_label_size(int index, int* width, int* height);
void get_pause_menu_label_size(int index, int* width, int* height);
void init_user_interface(TTF_Font* font);
void init_ui_rendering(int width, int height);
void draw_rectangle(GLuint texture_id, int x, int y, int width, int height, Color color);
void render_background(GLuint texture_id);
void render_menu(Label* labels, int item_count, int selection, bool is_confirmed, int start_y);
void render_settings(int selection, float brightness, float intensity);
void render_confirm_dialog(int screen_width, int screen_height);
void render_help_overlay(App* app);
void render_dim_overlay();
void destroy_user_interface();

#endif // UI_H
