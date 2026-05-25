#include "ui.h"

#include "app.h"
#include "camera.h"
#include "color.h"
#include "gl_state.h"
#include "texture.h"

#include <GL/glew.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>

#include <stdbool.h>

static const char* MAIN_MENU_TITLES[] = {"Start Simulation", "Settings", "Help", "Exit"};
static const char* PAUSE_MENU_TITLES[] = {"Resume", "Settings", "Help", "Exit to Menu", "Exit Application"};
static const char* SETTINGS_TITLES[] = {"Global Brightness", "Particle Intensity"};
static const char* CONFIRM_TITLES[] = {"Are you sure?", "Yes", "No"};

static Label main_menu_labels[MAIN_MENU_COUNT];
static Label pause_menu_labels[PAUSE_MENU_COUNT];
static Label settings_labels[SETTINGS_MENU_COUNT];
static Label confirm_labels[CONFIRM_MENU_COUNT];
static Label help_label;

static const Color COLOR_NO_HIGHLIGHT = {1.0f, 1.0f, 1.0f, 1.0f};
static const Color COLOR_HIGHLIGHT = {1.0f, 1.0f, 0.0f, 1.0f};
static const Color COLOR_BACKGROUND = {0.3f, 0.3f, 0.3f, 1.0f};
static const Color COLOR_OVERLAY = {0.0f, 0.0f, 0.0f, 0.5f};

Label* get_main_menu_labels() {
    return main_menu_labels;
}

Label* get_pause_menu_labels() {
    return pause_menu_labels;
}

void get_main_menu_label_size(int index, int* width, int* height) {
    if (index >= 0 && index < MAIN_MENU_COUNT) {
        if (width) *width = main_menu_labels[index].width;
        if (height) *height = main_menu_labels[index].height;
    }
}

void get_pause_menu_label_size(int index, int* width, int* height) {
    if (index >= 0 && index < PAUSE_MENU_COUNT) {
        *width = pause_menu_labels[index].width;
        *height = pause_menu_labels[index].height;
    }
}

void init_user_interface(TTF_Font* font) {
    SDL_Color color_white = {255, 255, 255, 255};

    for (int i = 0; i < MAIN_MENU_COUNT; i++) {
        main_menu_labels[i].texture_id = load_text_texture(font, MAIN_MENU_TITLES[i], color_white, &main_menu_labels[i].width, &main_menu_labels[i].height, 0);
    }

    for (int i = 0; i < PAUSE_MENU_COUNT; i++) {
        pause_menu_labels[i].texture_id = load_text_texture(font, PAUSE_MENU_TITLES[i], color_white, &pause_menu_labels[i].width, &pause_menu_labels[i].height, 0);
    }

    for (int i = 0; i < SETTINGS_MENU_COUNT; i++) {
        settings_labels[i].texture_id = load_text_texture(font, SETTINGS_TITLES[i], color_white, &settings_labels[i].width, &settings_labels[i].height, 0);
    }

    for (int i = 0; i < CONFIRM_MENU_COUNT; i++) {
        confirm_labels[i].texture_id = load_text_texture(font, CONFIRM_TITLES[i], color_white, &confirm_labels[i].width, &confirm_labels[i].height, 0);
    }

    const char* help_text =
        "Help: F1\n"
        "Pause: Esc\n"
        "Movement (x, y plane): W, A, S, D\n"
        "Movement (z plane): Q, E\n"
        "Rotation: Mouse\n"
        "Firework effects: Space, U, I, O, P, H, J, K, L, B, N, M\n"
    ;

    help_label.texture_id = load_text_texture(font, help_text, color_white, &help_label.width, &help_label.height, 800);
}

void init_ui_rendering(int width, int height) {
    set_orthogonal_view(width, height);

    set_state_lighting(GL_FALSE);
    set_state_depth_test(GL_FALSE);
    set_state_texture_2d(GL_TRUE);
    set_state_blend(GL_TRUE);
    set_state_blend_function(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void draw_rectangle(GLuint texture_id, int x, int y, int width, int height, Color color) {
    glColor4f(color.red, color.green, color.blue, color.alpha);

    if (texture_id != 0) {
        set_state_texture_2d(GL_TRUE);
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }
    else {
        set_state_texture_2d(GL_FALSE);
    }

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f((float)x, (float)y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f((float)x + width, (float)y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f((float)x + width, (float)y + height);
        glTexCoord2f(0.0f, 1.0f); glVertex2f((float)x, (float)y + height);
    glEnd();
}

void render_background(GLuint texture_id) {
    if (texture_id == 0) {
        return;
    }

    int width;
    int height;
    SDL_GL_GetDrawableSize(SDL_GL_GetCurrentWindow(), &width, &height);

    init_ui_rendering(width, height);

    draw_rectangle(texture_id, 0, 0, width, height, COLOR_NO_HIGHLIGHT);

    restore_perspective_view();
}

void render_menu(Label* labels, int item_count, int selection, bool is_confirmed, int start_y) {
    int width;
    int height;
    SDL_GL_GetDrawableSize(SDL_GL_GetCurrentWindow(), &width, &height);

    init_ui_rendering(width, height);

    for (int i = 0; i < item_count; i++) {
        Color item_color = (i == selection) ? COLOR_HIGHLIGHT : COLOR_NO_HIGHLIGHT;

        int text_x = width / 2 - labels[i].width / 2;
        int text_y = start_y + i * 60;

        draw_rectangle(labels[i].texture_id, text_x, text_y, labels[i].width, labels[i].height, item_color);
    }

    if (is_confirmed) {
        render_confirm_dialog(width, height);
    }

    restore_perspective_view();
}

void render_settings(int selection, float brightness, float intensity) {
    int width;
    int height;
    SDL_GL_GetDrawableSize(SDL_GL_GetCurrentWindow(), &width, &height);

    init_ui_rendering(width, height);

    float values[2] = { brightness, intensity };

    for (int i = 0; i < SETTINGS_MENU_COUNT; i++) {
        Color item_color = (i == selection) ? COLOR_HIGHLIGHT : COLOR_NO_HIGHLIGHT;

        int text_x = width / 2 - 300;
        int text_y = 300 + i * 100;

        draw_rectangle(settings_labels[i].texture_id, text_x, text_y, settings_labels[i].width, settings_labels[i].height, item_color);

        int bar_width = 200;
        int bar_height = 20;
        int bar_x = width / 2 + 50;
        int bar_y = text_y + 5;

        set_state_texture_2d(GL_FALSE);
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(bar_x, bar_y);
            glVertex2f(bar_x + bar_width, bar_y);
            glVertex2f(bar_x + bar_width, bar_y + bar_height);
            glVertex2f(bar_x, bar_y + bar_height);
        glEnd();
        set_state_texture_2d(GL_TRUE);

        int fill_width = (int)(bar_width * values[i]);
        Color bar_color = (i == selection) ? COLOR_HIGHLIGHT : (Color){0.7f, 0.7f, 0.0f, 1.0f};

        draw_rectangle(0, bar_x, bar_y, fill_width, bar_height, bar_color);
    }

    restore_perspective_view();
}

void render_confirm_dialog(int screen_width, int screen_height) {
    draw_rectangle(0, 0, 0, screen_width, screen_height, COLOR_OVERLAY);

    int text_x = screen_width / 2 - confirm_labels[0].width / 2;
    int text_y = screen_height / 2 - 100;
    draw_rectangle(confirm_labels[0].texture_id, text_x, text_y, confirm_labels[0].width, confirm_labels[0].height, COLOR_NO_HIGHLIGHT);

    int button_width = 120;
    int button_height = 50;
    int spacing = 40;
    int button_y = screen_height / 2;

    int yes_x = screen_width / 2 - button_width - spacing;
    int yes_text_x = yes_x + (button_width - confirm_labels[1].width) / 2;
    int yes_text_y = button_y + (button_height - confirm_labels[1].height) / 2;

    draw_rectangle(0, yes_x, button_y, button_width, button_height, COLOR_BACKGROUND);
    draw_rectangle(confirm_labels[1].texture_id, yes_text_x, yes_text_y, confirm_labels[1].width, confirm_labels[1].height, COLOR_NO_HIGHLIGHT);

    int no_x = screen_width / 2 + spacing;
    int no_text_x = no_x + (button_width - confirm_labels[2].width) / 2;
    int no_text_y = button_y + (button_height - confirm_labels[2].height) / 2;

    draw_rectangle(0, no_x, button_y, button_width, button_height, COLOR_BACKGROUND);
    draw_rectangle(confirm_labels[2].texture_id, no_text_x, no_text_y, confirm_labels[2].width, confirm_labels[2].height, COLOR_NO_HIGHLIGHT);
}

void render_help_overlay(App* app) {
    int width;
    int height;
    SDL_GL_GetDrawableSize(app->window, &width, &height);

    init_ui_rendering(width, height);

    draw_rectangle(0, 0, 0, width, height, COLOR_OVERLAY);

    int text_x = width / 2 - help_label.width / 2;
    int text_y = height / 2 - help_label.height / 2;
    draw_rectangle(help_label.texture_id, text_x, text_y, help_label.width, help_label.height, COLOR_NO_HIGHLIGHT);

    restore_perspective_view();
}

void render_dim_overlay() {
    int width;
    int height;
    SDL_GL_GetDrawableSize(SDL_GL_GetCurrentWindow(), &width, &height);

    init_ui_rendering(width, height);

    draw_rectangle(0, 0, 0, width, height, COLOR_OVERLAY);

    restore_perspective_view();
}

void destroy_user_interface() {
    for (int i = 0; i < MAIN_MENU_COUNT; i++) {
        glDeleteTextures(1, &main_menu_labels[i].texture_id);
    }

    for (int i = 0; i < PAUSE_MENU_COUNT; i++) {
        glDeleteTextures(1, &pause_menu_labels[i].texture_id);
    }

    for (int i = 0; i < SETTINGS_MENU_COUNT; i++) {
        glDeleteTextures(1, &settings_labels[i].texture_id);
    }

    for (int i = 0; i < CONFIRM_MENU_COUNT; i++) {
        glDeleteTextures(1, &confirm_labels[i].texture_id);
    }

    glDeleteTextures(1, &help_label.texture_id);
}
