#ifndef COLOR_H
#define COLOR_H

typedef enum ColorName {
    COLOR_BLUE,
    COLOR_BROWN,
    COLOR_CYAN,
    COLOR_GREEN,
    COLOR_ORANGE,
    COLOR_PINK,
    COLOR_RED,
    COLOR_SILVER,
    COLOR_VIOLET,
    COLOR_WHITE,
    COLOR_YELLOW,
    COLOR_COUNT
} ColorName;

typedef struct Color {
    float red;
    float green;
    float blue;
    float alpha;
} Color;

Color get_color(ColorName name);
Color mix_color(Color start, Color end, float t);

#endif // COLOR_H
