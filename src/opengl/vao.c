#include "vao.h"

Vao vao_new() {
    Vao vao = {0};
    glGenVertexArrays(1, &vao.handle);
    return vao;
}

void vao_bind(Vao *vao) {
    glBindVertexArray(vao->handle);
}

void vao_delete(Vao *vao) {
    glDeleteVertexArrays(1, &vao->handle);
}
