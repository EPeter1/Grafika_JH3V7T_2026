#include "color.h"

#include <stdio.h>
#include <stdlib.h>

static const Color color_palette[COLOR_COUNT] = {
    [COLOR_BLUE] = {0.2f, 0.4f, 1.0f, 1.0f},
    [COLOR_BROWN] = {0.4f, 0.2f, 0.05f, 1.0f},
    [COLOR_CYAN] = {0.1f, 1.0f, 1.0f, 1.0f},
    [COLOR_GREEN] = {0.15f, 1.0f, 0.2f, 1.0f},
    [COLOR_ORANGE] = {1.0f, 0.5f, 0.0f, 1.0f},
    [COLOR_PINK] = {1.0f, 0.4f, 0.7f, 1.0f},
    [COLOR_RED] = {1.0f, 0.15f, 0.15f, 1.0f},
    [COLOR_SILVER] = {0.75f, 0.78f, 0.85f, 1.0f},
    [COLOR_VIOLET] = {0.5f, 0.0f, 1.0f, 1.0f},
    [COLOR_WHITE] = {1.0f, 1.0f, 1.0f, 1.0f},
    [COLOR_YELLOW] = {1.0f, 0.8f, 0.1f, 1.0f}
};

Color get_color(ColorName name) {
    if (name >= COLOR_COUNT) {
        fprintf(stderr, "[ERROR] COLOR_COUNT is invalid!");
        exit(1);
    }

    return color_palette[name];
}

Color mix_color(Color start, Color end, float t) {
    Color result;

    result.red = start.red + (end.red - start.red) * t;
    result.green = start.green + (end.green - start.green) * t;
    result.blue = start.blue + (end.blue - start.blue) * t;
    result.alpha = start.alpha;

    return result;
}
