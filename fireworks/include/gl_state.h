#ifndef GL_STATE_H
#define GL_STATE_H

#include <GL/glew.h>

#include <stdbool.h>

void set_gl_state_lighting(bool enable);
void set_gl_state_texture_2d(bool enable);
void set_gl_state_depth_test(bool enable);
void set_gl_state_depth_mask(bool enable);
void set_gl_state_stencil_test(bool enable);
void set_gl_state_blend(bool enable);
void set_gl_state_blend_function(GLenum source, GLenum destination);

#endif // GL_STATE_H
