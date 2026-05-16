#include "texture.h"

#include <SDL2/SDL_endian.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>

GLuint load_texture(char* filename)
{
    SDL_Surface* surface = IMG_Load(filename);
    if (surface == NULL) {
        printf("[ERROR] Failed to load image %s: %s\n", filename, IMG_GetError());
        return 0;
    }

    GLuint texture_name;
    glGenTextures(1, &texture_name);
    glBindTexture(GL_TEXTURE_2D, texture_name);

    GLint internal_format;
    GLenum format;

    if (surface->format->BytesPerPixel == 4) {
        internal_format = GL_RGBA;
        format = GL_RGBA;
    }
    else {
        internal_format = GL_RGB;
        format = GL_RGB;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(surface);
    return texture_name;
}

GLuint load_text_texture(TTF_Font* font, const char* text, SDL_Color color, int* width, int* height) {
    SDL_Surface* temp_surface = TTF_RenderUTF8_Blended(font, text, color);
    if (temp_surface == NULL) {
        printf("[ERROR] Failed to render text: %s\n", TTF_GetError());
        return 0;
    }

    Uint32 pixel_format;
    #if SDL_BYTEORDER == SDL_LIL_ENDIAN
        pixel_format = SDL_PIXELFORMAT_ABGR8888;
    #else
        pixel_format = SDL_PIXELFORMAT_RGBA8888;
    #endif

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(temp_surface, pixel_format, 0);
    SDL_FreeSurface(temp_surface);

    if (surface == NULL) {
        printf("[ERROR] Failed to convert surface: %s\n", SDL_GetError());
        return 0;
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

GLuint load_text_texture_wrapped(TTF_Font* font, const char* text, SDL_Color color, int* width, int* height, int wrap_length) {
    SDL_Surface* temp_surface = TTF_RenderUTF8_Blended_Wrapped(font, text, color, wrap_length);
    if (temp_surface == NULL) {
        printf("[ERROR] Failed to render text: %s\n", TTF_GetError());
        return 0;
    }

    Uint32 pixel_format;
    #if SDL_BYTEORDER == SDL_LIL_ENDIAN
        pixel_format = SDL_PIXELFORMAT_ABGR8888;
    #else
        pixel_format = SDL_PIXELFORMAT_RGBA8888;
    #endif

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(temp_surface, pixel_format, 0);
    SDL_FreeSurface(temp_surface);

    if (surface == NULL) {
        printf("[ERROR] Failed to convert surface: %s\n", SDL_GetError());
        return 0;
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
