#include "app.h"

/**
 * Main function
 */
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    App* app = (App*)malloc(sizeof(App));

    if (app == NULL) {
        printf("[ERROR] Application memory allocation failed!\n");
        return 1;
    }

    init_app(app, 1600, 900);
    while (app->is_running) {
        handle_app_events(app);
        update_app(app);
        render_app(app);
    }
    destroy_app(app);
    free(app);

    return 0;
}
