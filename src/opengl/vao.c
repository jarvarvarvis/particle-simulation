#include "vao.h"

Vao vao_new() {
    GLuint handle;
    glGenVertexArrays(1, &handle);
    return (Vao) { handle };
}

void vao_delete(Vao *vao) {
    glDeleteVertexArrays(1, &vao->handle);
}
