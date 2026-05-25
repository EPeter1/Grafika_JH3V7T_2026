#include "texture.h"

#include <GL/glew.h>
#include <SDL2/SDL_endian.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

GLuint load_texture(const char* filename, GLint wrap_mode, bool is_mipmapped) {
    SDL_Surface* temp_surface = IMG_Load(filename);
    if (temp_surface == NULL) {
        fprintf(stderr, "[ERROR] Failed to load image %s: %s\n", filename, IMG_GetError());
        exit(1);
    }

    bool has_alpha = (temp_surface->format->BytesPerPixel == 4);
    Uint32 pixel_format = has_alpha ? SDL_PIXELFORMAT_RGBA32 : SDL_PIXELFORMAT_RGB24;

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(temp_surface, pixel_format, 0);
    SDL_FreeSurface(temp_surface);

    if (surface == NULL) {
        fprintf(stderr, "[ERROR] Failed to convert surface: %s\n", SDL_GetError());
        exit(1);
    }

    GLuint texture_name;
    glGenTextures(1, &texture_name);
    glBindTexture(GL_TEXTURE_2D, texture_name);

    GLint internal_format = has_alpha ? GL_RGBA : GL_RGB;
    GLenum format = has_alpha ? GL_RGBA : GL_RGB;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);

    if (is_mipmapped) {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(surface);
    return texture_name;
}

GLuint load_text_texture(TTF_Font* font, const char* text, SDL_Color color, int* width, int* height, int wrap_length) {
    SDL_Surface* temp_surface;

    if (wrap_length == 0) {
        temp_surface = TTF_RenderUTF8_Blended(font, text, color);
    }
    else {
        temp_surface = TTF_RenderUTF8_Blended_Wrapped(font, text, color, wrap_length);
    }

    if (temp_surface == NULL) {
        fprintf(stderr, "[ERROR] Failed to render text: %s\n", TTF_GetError());
        exit(1);
    }

    bool has_alpha = (temp_surface->format->BytesPerPixel == 4);
    Uint32 pixel_format = has_alpha ? SDL_PIXELFORMAT_RGBA32 : SDL_PIXELFORMAT_RGB24;

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(temp_surface, pixel_format, 0);
    SDL_FreeSurface(temp_surface);

    if (surface == NULL) {
        fprintf(stderr, "[ERROR] Failed to convert surface: %s\n", SDL_GetError());
        exit(1);
    }

    *width = surface->w;
    *height = surface->h;

    GLuint texture_name;
    glGenTextures(1, &texture_name);
    glBindTexture(GL_TEXTURE_2D, texture_name);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(surface);
    return texture_name;
}
