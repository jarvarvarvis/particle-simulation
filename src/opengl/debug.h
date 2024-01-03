#ifndef DEBUG_H
#define DEBUG_H

#include <GL/glew.h>

const char *debug_get_err_source_name(GLenum source);
const char *debug_get_err_type_name(GLenum type);

void debug_message(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                  const GLchar *message, const void *user_param);

void debug_register_message_callback();

#endif /* DEBUG_H */
