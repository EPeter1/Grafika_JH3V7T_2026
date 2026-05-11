#include "texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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
