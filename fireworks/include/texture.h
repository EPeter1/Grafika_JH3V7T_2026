#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_ttf.h>

/**
 * Load texture from file and returns with the texture name.
 */
GLuint load_texture(char* filename);

GLuint load_text_texture(TTF_Font* font, const char* text, SDL_Color color, int* width, int* height);
GLuint load_text_texture_wrapped(TTF_Font* font, const char* text, SDL_Color color, int* width, int* height, int wrap_length);

#endif /* TEXTURE_H */
