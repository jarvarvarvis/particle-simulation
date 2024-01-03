#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define C_LOG_DEFINITION
#include "../thirdparty/c_log.h"

#define C_MATH2D_DEFINITION
#include "../thirdparty/c_math2d.h"

#include "opengl/debug.h"
#include "camera/orthographic.h"
#include "particle/renderer.h"
#include "particle/simulation.h"
#include "particle/constraint.h"

typedef struct {
    OrthoCamera camera;
    int window_width, window_height;
} WindowUserData;

void error_callback(int error, const char *description) {
    c_log(C_LOG_SEVERITY_ERROR, "%s (Code: %d)", description, error);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    c_log(C_LOG_SEVERITY_DEBUG, "Window resize: %d,%d", width, height);

    WindowUserData *user_ptr = (WindowUserData *) glfwGetWindowUserPointer(window);
    user_ptr->window_width = width;
    user_ptr->window_height = height;
    ortho_camera_on_window_resize(&user_ptr->camera, width, height);
}

float frand() {
    return (float)rand() / RAND_MAX;
}

float time_diff_ms(struct timespec t0, struct timespec t1)
{
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_nsec - t0.tv_nsec) / 1000000.0f;
}

int main() {
    if (!glfwInit()) {
        c_log(C_LOG_SEVERITY_ERROR, "Failed to initialize glfw");
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    const int WIDTH = 1200, HEIGHT = 800;
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "particle-simulation",
            NULL, NULL);

    if (!window) {
        c_log(C_LOG_SEVERITY_ERROR, "Failed to create glfw window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Set up window user pointer
    WindowUserData user_data = {0};
    user_data.camera = ortho_camera_new(WIDTH, HEIGHT);
    user_data.window_width = WIDTH;
    user_data.window_height = HEIGHT;
    glfwSetWindowUserPointer(window, &user_data);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        c_log(C_LOG_SEVERITY_ERROR, "Failed to initialize GLEW");
        exit(EXIT_FAILURE);
    }

    // Register debug callback
    debug_register_message_callback();

    // Create renderer, particle list and solver
    ParticleRenderer renderer = particle_renderer_new();

    ParticleList particles = particle_list_new();
    ParticleIterator iterator = particle_list_iterate(&particles);

    const float SOLVER_SUB_STEPS = 8;
    const float SOLVER_DT = 0.01;
    Solver solver = solver_new(SOLVER_SUB_STEPS);
    //solver.constraint = circular_constraint_new(cm2_vec2_new(0.0, 0.0), 400.0f);
    solver.constraint = box_constraint_new(
        cm2_vec2_new(-400.0, -400.0),
        cm2_vec2_new(400.0, 400.0)
    );

    // Initialize RNG
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    srand(time.tv_nsec);

    // Particle spawning
    struct timespec start_timer;
    clock_gettime(CLOCK_REALTIME, &start_timer);
    float particle_spawn_time_interval = 30.0;
    int particles_left_to_spawn = 300;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        // Spawn particles
        if (particles_left_to_spawn > 0) {
            struct timespec current_timer;
            clock_gettime(CLOCK_REALTIME, &current_timer);
            float elapsed_millis = time_diff_ms(start_timer, current_timer);
            if (elapsed_millis > particle_spawn_time_interval) {
                // Create random particle
                Particle particle = particle_new(0.0, 0.0, 10.0, frand(), frand(), frand(), 1.0);

                // Add some random velocity in circle around center
                particle.position.x += sinf((float)particles_left_to_spawn * 0.1) * 2.0;
                particle.position.y += cosf((float)particles_left_to_spawn * 0.1) * 2.0;

                // Push the particle to the list and decrease particle counter
                particle_list_push(&particles, particle);
                particles_left_to_spawn--;

                // Reset the clock
                start_timer = current_timer;
            }
        }

        // Update solver and upload data to GPU
        solver_update(&solver, &iterator, SOLVER_DT);
        particle_renderer_upload_from_list(&renderer, &particles);

        // Draw
        shader_program_use(&renderer.shader_program);
        shader_program_set_mat4(&renderer.shader_program, "_MProj", user_data.camera.projection_matrix);
        particle_renderer_draw(&renderer);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    solver_delete(&solver);
    particle_iterator_delete(&iterator);
    particle_list_delete(&particles);
    particle_renderer_delete(&renderer);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
