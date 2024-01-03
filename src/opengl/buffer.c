#include "buffer.h"

Buffer buffer_new(GLenum target) {
    Buffer buffer = {0};
    buffer.target = target;
    glGenBuffers(1, &buffer.handle);
    return buffer;
}

void buffer_bind(Buffer *buffer) {
    glBindBuffer(buffer->target, buffer->handle);
}

void buffer_upload_data_static(Buffer *buffer, void *data, size_t size) {
    glBufferData(buffer->target, size, data, GL_STATIC_DRAW);
}

void buffer_delete(Buffer *buffer) {
    glDeleteBuffers(1, &buffer->handle);
}
