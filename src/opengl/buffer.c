#include "buffer.h"

Buffer buffer_null(GLenum target) {
    return (Buffer) { target, 0 };
}

Buffer buffer_new(GLenum target) {
    GLuint handle;
    glGenBuffers(1, &handle);
    return (Buffer) { target, handle };
}

void buffer_bind(Buffer *buffer) {
    glBindBuffer(buffer->target, buffer->handle);
}

void buffer_delete(Buffer *buffer) {
    glDeleteBuffers(1, &buffer->handle);
}
