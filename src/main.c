#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "../thirdparty/c_log.h"

void error_callback(int error, const char *description) {
    c_log(C_LOG_SEVERITY_ERROR, "%s (Code: %d)", description, error);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    c_log(C_LOG_SEVERITY_DEBUG, "Window resize: %d,%d", width, height);
}

int main() {
    if (!glfwInit()) {
        c_log(C_LOG_SEVERITY_ERROR, "Failed to initialize glfw");
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(error_callback);
    GLFWwindow *window = glfwCreateWindow(800, 600, "particle-simulation",
            NULL, NULL);

    if (!window) {
        c_log(C_LOG_SEVERITY_ERROR, "Failed to create glfw window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
