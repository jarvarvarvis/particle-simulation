#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <stdbool.h>

#include "../camera/orthographic.h"
#include "../util/version.h"

typedef struct {
    OrthoCamera camera;
    int window_width, window_height;
} WindowUserData;

void window_error_callback(int error, const char *description);
void window_framebuffer_size_callback(GLFWwindow *window, int width, int height);

typedef struct {
    Version context_version;
    int opengl_profile;
    bool use_opengl_debug_context;

    int window_width, window_height;
    const char *window_title;
} WindowParameters;

GLFWwindow *window_create_from_params(WindowParameters parameters);

#endif /* WINDOW_H */
