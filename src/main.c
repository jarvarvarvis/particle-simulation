#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "../thirdparty/c_log.h"

void error_callback(int error, const char *description) {
    c_log(C_LOG_SEVERITY_ERROR, "%s (Code: %d)", description, error);
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

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
