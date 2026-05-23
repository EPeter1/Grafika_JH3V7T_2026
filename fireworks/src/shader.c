#include "shader.h"

#include <GL/glew.h>

#include <stdio.h>
#include <stdlib.h>

static char* read_shader(const char* file_path) {
    FILE* file = fopen(file_path, "rb");

    if (!file) {
        fprintf(stderr, "[ERROR] Could not open file: %s\n", file_path);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long int file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(file_size + 1);

    if (!buffer) {
        fprintf(stderr, "[ERROR] Memory allocation failed!\n");

        fclose(file);
        exit(1);
    }

    fread(buffer, sizeof(char), file_size, file);
    buffer[file_size] = '\0';

    fclose(file);
    return buffer;
}

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint id = glCreateShader(type);

    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        char message[1024];
        glGetShaderInfoLog(id, 1024, &length, message);
        fprintf(stderr, "[ERROR] Failed to compile %s shader!\n", type == GL_VERTEX_SHADER ? "vertex" : "fragment");
        fprintf(stderr, "%s\n", message);

        glDeleteShader(id);
        exit(1);
    }

    return id;
}

GLuint create_shader(const char* vert_path, const char* frag_path, const VertexAttribute* attributes) {
    char* vertex_source = read_shader(vert_path);
    char* fragment_source = read_shader(frag_path);

    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    for (int i = 0; attributes[i].name != NULL; i++) {
        glBindAttribLocation(program, attributes[i].location, attributes[i].name);
    }

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    free(vertex_source);
    free(fragment_source);

    return program;
}
