#ifndef BUFFER_H
#define BUFFER_H

#include <GL/glew.h>

typedef struct {
    GLenum target;
    GLuint handle;
} Buffer;

Buffer buffer_null(GLenum target);
Buffer buffer_new(GLenum target);
void buffer_bind(Buffer *buffer);
void buffer_delete(Buffer *buffer);

#endif /* BUFFER_H */
