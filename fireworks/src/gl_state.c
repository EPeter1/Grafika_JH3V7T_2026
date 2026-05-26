#include "gl_state.h"

#include <GL/glew.h>

#include <stdbool.h>

typedef struct BlendState {
    bool enabled;
    GLenum source;
    GLenum destination;
} BlendState;

typedef struct GLState {
    bool lighting;
    bool texture_2d;
    bool depth_test;
    bool depth_mask;
    bool stencil_test;
    BlendState blend;
} GLState;

static GLState current_state;

static void set_capability(bool* cache, GLenum capability, bool enable) {
    if (*cache != enable) {
        *cache = enable;

        if (enable) {
            glEnable(capability);
        }
        else {
            glDisable(capability);
        }
    }
}

void set_gl_state_lighting(bool enable) {
    set_capability(&current_state.lighting, GL_LIGHTING, enable);
}

void set_gl_state_texture_2d(bool enable) {
    set_capability(&current_state.texture_2d, GL_TEXTURE_2D, enable);
}

void set_gl_state_depth_test(bool enable) {
    set_capability(&current_state.depth_test, GL_DEPTH_TEST, enable);
}

void set_gl_state_depth_mask(bool enable) {
    if (current_state.depth_mask != enable) {
        current_state.depth_mask = enable;
        glDepthMask(enable ? GL_TRUE : GL_FALSE);
    }
}

void set_gl_state_stencil_test(bool enable) {
    set_capability(&current_state.stencil_test, GL_STENCIL_TEST, enable);
}

void set_gl_state_blend(bool enable) {
    set_capability(&current_state.blend.enabled, GL_BLEND, enable);
}

void set_gl_state_blend_function(GLenum source, GLenum destination) {
    if (current_state.blend.source != source || current_state.blend.destination != destination) {
        current_state.blend.source = source;
        current_state.blend.destination = destination;

        glBlendFunc(source, destination);
    }
}
