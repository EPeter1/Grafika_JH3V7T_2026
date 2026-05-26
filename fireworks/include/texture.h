#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#include <stdbool.h>

typedef struct TextureUV {
    float u;
    float v;
} TextureUV;

/**
 * Load texture from file and returns with the texture name.
 */
GLuint load_texture(const char* filename, GLint wrap_mode, bool is_mipmapped);
GLuint load_text_texture(TTF_Font* font, const char* text, SDL_Color color, int* width, int* height, int wrap_length);
SDL_Surface* load_surface(const char* filename);

#endif // TEXTURE_H
