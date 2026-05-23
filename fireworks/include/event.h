#ifndef EVENT_H
#define EVENT_H

#include "app.h"
#include "fireworks.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_scancode.h>

void handle_menu_events(App* app, SDL_Event* event, int item_count, int start_y, void (*get_size)(int, int*, int*));
void handle_settings_events(App* app, SDL_Event* event);
void handle_simulation_events(App* app, SDL_Event* event);
void activate_menu_item(App* app);
void execute_confirmed_action(App* app);
bool is_mouse_over_item(int mouse_x, int mouse_y, int item_x, int item_y, int item_width, int item_height);
int get_menu_selection_by_mouse(int mouse_x, int mouse_y, int screen_width, int item_count, int start_y, void (*get_size)(int, int*, int*));
void handle_settings_mouse_event(App* app, int x, int y);
void handle_confirm_mouse_event(App* app, int mouse_x, int mouse_y);
void try_launch_firework(Firework* fireworks, SDL_Scancode scancode);

#endif // EVENT_H
