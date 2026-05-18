#include "gl_state.h"

#include <GL/glew.h>

typedef struct BlendState {
    GLboolean enabled;
    GLenum source;
    GLenum destination;
} BlendState;

typedef struct GLState {
    GLboolean lighting;
    GLboolean texture_2d;
    GLboolean depth_test;
    GLboolean depth_mask;
    GLboolean stencil_test;
    BlendState blend;
} GLState;

static GLState gl_state;

static void set_capability(GLboolean* cache, GLenum capability, GLboolean enable) {
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

void set_state_lighting(GLboolean enable) {
    set_capability(&gl_state.lighting, GL_LIGHTING, enable);
}

void set_state_texture_2d(GLboolean enable) {
    set_capability(&gl_state.texture_2d, GL_TEXTURE_2D, enable);
}

void set_state_depth_test(GLboolean enable) {
    set_capability(&gl_state.depth_test, GL_DEPTH_TEST, enable);
}

void set_state_depth_mask(GLboolean enable) {
    if (gl_state.depth_mask != enable) {
        gl_state.depth_mask = enable;
        glDepthMask(enable);
    }
}

void set_state_blend(GLboolean enable) {
    set_capability(&gl_state.blend.enabled, GL_BLEND, enable);
}

void set_state_blend_function(GLenum source, GLenum destination) {
    if (gl_state.blend.source != source || gl_state.blend.destination != destination) {
        gl_state.blend.source = source;
        gl_state.blend.destination = destination;

        glBlendFunc(source, destination);
    }
}

void set_state_stencil_test(GLboolean enable) {
    set_capability(&gl_state.stencil_test, GL_STENCIL_TEST, enable);
}
