#ifndef VAO_H
#define VAO_H

#include <GL/glew.h>

typedef struct {
    GLuint handle;
} Vao;

Vao vao_new();
void vao_bind(Vao *vao);
void vao_delete(Vao *vao);

#endif /* VAO_H */
