#include "shader.h"

#include "../thirdparty/c_log.h"

Shader shader_new(GLenum type) {
    GLuint handle = glCreateShader(type);
    return (Shader) { handle };
}

void shader_compile_source(Shader *shader, char *source) {
    glShaderSource(shader->handle, 1, (const char**) &source, NULL);
    glCompileShader(shader->handle);
}

int shader_log_status(Shader *shader) {
    int success;
    glGetShaderiv(shader->handle, GL_COMPILE_STATUS, &success);

    if (!success) {
        // Get log length
        int log_length;
        glGetShaderiv(shader->handle, GL_INFO_LOG_LENGTH, &log_length);

        // Get the log string
        char *log = malloc((log_length + 1) * sizeof(char));
        glGetShaderInfoLog(shader->handle, log_length, NULL, log);
        log[log_length] = '\0';

        // Output the log message
        c_log(C_LOG_SEVERITY_ERROR, "Shader compilation failed: %s", log);

        free(log);
        return success;
    }

    return success;
}

void shader_delete(Shader *shader) {
    glDeleteShader(shader->handle);
}


ShaderProgram shader_program_new() {
    GLuint handle = glCreateProgram();
    return (ShaderProgram) { handle };
}

void shader_program_attach(ShaderProgram *shader_program, Shader *shader) {
    glAttachShader(shader_program->handle, shader->handle);
}

void shader_program_link(ShaderProgram *shader_program) {
    glLinkProgram(shader_program->handle);
}

int shader_program_log_status(ShaderProgram *shader_program) {
    int success;
    glGetProgramiv(shader_program->handle, GL_LINK_STATUS, &success);

    if (!success) {
        // Get log length
        int log_length = 0;
        glGetProgramiv(shader_program->handle, GL_INFO_LOG_LENGTH, &log_length);

        // Get the log string
        char *log = malloc((log_length + 1) * sizeof(char));
        glGetProgramInfoLog(shader_program->handle, log_length, NULL, log);
        log[log_length] = '\0';

        // Output the log message
        c_log(C_LOG_SEVERITY_ERROR, "Program linking failed: %s", log);

        free(log);
        return success;
    }

    return success;
}

GLint shader_program_get_uniform_location(ShaderProgram *shader_program, char *name) {
    return glGetUniformLocation(shader_program->handle, name);
}

void shader_program_set_bool(ShaderProgram *shader_program, char *name, bool value) {
    GLint location = shader_program_get_uniform_location(shader_program, name);
    glUniform1i(location, value);
}

void shader_program_set_int(ShaderProgram *shader_program, char *name, int value) {
    GLint location = shader_program_get_uniform_location(shader_program, name);
    glUniform1i(location, value);
}

void shader_program_set_float(ShaderProgram *shader_program, char *name, float value) {
    GLint location = shader_program_get_uniform_location(shader_program, name);
    glUniform1i(location, value);
}

void shader_program_delete(ShaderProgram *shader_program) {
    glDeleteProgram(shader_program->handle);
}
