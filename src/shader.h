#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/gl.h>

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
void shader_program_delete(ShaderProgram *shader_program);

#endif /* SHADER_H */
