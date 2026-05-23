#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

typedef struct VertexAttribute {
    const GLuint location;
    const char* name;
} VertexAttribute;

GLuint create_shader(const char* vert_path, const char* frag_path, const VertexAttribute* attributes);

#endif // SHADER_H
