#include "window.h"

#include "../../thirdparty/c_log.h"

void window_error_callback(int error, const char *description) {
    c_log(C_LOG_SEVERITY_ERROR, "%s (Code: %d)", description, error);
}

void window_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);

    WindowUserData *user_ptr = (WindowUserData *) glfwGetWindowUserPointer(window);
    user_ptr->window_width = width;
    user_ptr->window_height = height;
    ortho_camera_on_window_resize(&user_ptr->camera, width, height);
}

GLFWwindow *window_create_from_params(WindowParameters parameters) {
    glfwSetErrorCallback(window_error_callback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, parameters.context_version.major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, parameters.context_version.minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE,        parameters.opengl_profile);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,
        parameters.use_opengl_debug_context ? GL_TRUE : GL_FALSE
    );

    GLFWwindow *window = glfwCreateWindow(
        parameters.window_width,
        parameters.window_height,
        parameters.window_title,
        NULL,
        NULL
    );

    if (!window) {
        c_log(C_LOG_SEVERITY_ERROR, "Failed to create GLFW window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetFramebufferSizeCallback(window, window_framebuffer_size_callback);
    return window;
}
