#include "shader.h"

#include "../util/io.h"

#include "../../thirdparty/c_log.h"

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
        c_log(C_LOG_SEVERITY_ERROR, "Shader compilation failed:\n%s", log);

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

ShaderProgram shader_program_load_from_file(char *vert_path, char *frag_path) {
    // Read shaders from file
    char *vert_source = io_read_file(vert_path);
    Shader vert_shader = shader_new(GL_VERTEX_SHADER);
    shader_compile_source(&vert_shader, vert_source);
    if (!shader_log_status(&vert_shader)) {
        exit(EXIT_FAILURE);
    }

    char *frag_source = io_read_file(frag_path);
    Shader frag_shader = shader_new(GL_FRAGMENT_SHADER);
    shader_compile_source(&frag_shader, frag_source);
    if (!shader_log_status(&vert_shader)) {
        exit(EXIT_FAILURE);
    }

    free(vert_source);
    free(frag_source);

    // Create shader program
    ShaderProgram shader_program = shader_program_new();
    shader_program_attach(&shader_program, &vert_shader);
    shader_program_attach(&shader_program, &frag_shader);
    shader_program_link(&shader_program);
    if (!shader_program_log_status(&shader_program)) {
        exit(EXIT_FAILURE);
    }

    return shader_program;
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
        c_log(C_LOG_SEVERITY_ERROR, "Program linking failed:\n%s", log);

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

void shader_program_set_vec4(ShaderProgram *shader_program, char *name, cm2_vec4 value) {
    GLint location = shader_program_get_uniform_location(shader_program, name);
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void shader_program_set_mat4(ShaderProgram *shader_program, char *name, cm2_mat4 value) {
    GLint location = shader_program_get_uniform_location(shader_program, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, cm2_mat4_value_ptr(value));
}

void shader_program_use(ShaderProgram *shader_program) {
    glUseProgram(shader_program->handle);
}

void shader_program_delete(ShaderProgram *shader_program) {
    glDeleteProgram(shader_program->handle);
}
