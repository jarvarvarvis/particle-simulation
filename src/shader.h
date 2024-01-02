#ifndef SHADER_H
#define SHADER_H

#include <stdbool.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include "../thirdparty/c_math2d.h"

typedef struct {
    GLuint handle;
} Shader;

Shader shader_new(GLenum type);
void shader_compile_source(Shader *shader, char *source);
int shader_log_status(Shader *shader);
void shader_delete(Shader *shader);

typedef struct {
    GLuint handle;
} ShaderProgram;

ShaderProgram shader_program_new();

void shader_program_attach(ShaderProgram *shader_program, Shader *shader);
void shader_program_link(ShaderProgram *shader_program);
int shader_program_log_status(ShaderProgram *shader_program);

GLint shader_program_get_uniform_location(ShaderProgram *shader_program, char *name);
void shader_program_set_bool(ShaderProgram *shader_program, char *name, bool value);
void shader_program_set_int(ShaderProgram *shader_program, char *name, int value);
void shader_program_set_float(ShaderProgram *shader_program, char *name, float value);
void shader_program_set_mat4(ShaderProgram *shader_program, char *name, cm2_mat4 value);

void shader_program_delete(ShaderProgram *shader_program);

#endif /* SHADER_H */
