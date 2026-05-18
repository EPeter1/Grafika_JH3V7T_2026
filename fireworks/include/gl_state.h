#ifndef GL_STATE_H
#define GL_STATE_H

#include <GL/glew.h>

void set_state_lighting(GLboolean enable);
void set_state_texture_2d(GLboolean enable);
void set_state_depth_test(GLboolean enable);
void set_state_depth_mask(GLboolean enable);
void set_state_blend(GLboolean enable);
void set_state_blend_function(GLenum source, GLenum destination);
void set_state_stencil_test(GLboolean enable);

#endif // GL_STATE_H
