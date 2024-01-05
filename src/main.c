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
#include "particle/grid.h"
#include "particle/grid_renderer.h"
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
    ParticleGrid particle_grid = particle_grid_new(56, 40, 20, 20);

    const float SOLVER_SUB_STEPS = 8;
    const float SOLVER_DT = 0.004;
    Solver solver = solver_new(SOLVER_SUB_STEPS);
    SolverParallelizationArgs parallelization_args;

    // Split grid into 8 sections (section width = 56 / 8 = 7)
    parallelization_args.section_count = 8;

    // Create constraint
    float particle_grid_half_width  = particle_grid.width  * particle_grid.cell_width  / 2.;
    float particle_grid_half_height = particle_grid.height * particle_grid.cell_height / 2.;
    solver.constraint = box_constraint_new(
        cm2_vec2_new(-particle_grid_half_width, -particle_grid_half_height),
        cm2_vec2_new( particle_grid_half_width,  particle_grid_half_height)
    );

    // Create grid and grid renderer
    GridRenderer grid_renderer = grid_renderer_new();
    grid_renderer.grid_width = (float)particle_grid.width;
    grid_renderer.grid_height = (float)particle_grid.height;
    grid_renderer.cell_width = (float)particle_grid.cell_width;
    grid_renderer.cell_height = (float)particle_grid.cell_height;

    // Initialize RNG
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    srand(time.tv_nsec);

    // Particle spawning
    struct timespec start_timer;
    clock_gettime(CLOCK_REALTIME, &start_timer);
    float particle_spawn_time_interval = 1.0;
    int particle_batches_left_to_spawn = 3500;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        // Spawn particles
        if (particle_batches_left_to_spawn > 0) {
            struct timespec current_timer;
            clock_gettime(CLOCK_REALTIME, &current_timer);
            float elapsed_millis = time_diff_ms(start_timer, current_timer);
            if (elapsed_millis > particle_spawn_time_interval) {
                // Create random particle
                Particle particle = particle_new(
                    -particle_grid_half_width + 20.0, particle_grid_half_height - 200.0,
                    7.0,
                    frand(), frand(), frand(), 1.0
                );

                // Add velocity to particle
                particle.position.x += 2.0;
                particle.position.y -= 0.2 + sinf(particle_batches_left_to_spawn * 0.05) * 0.2;

                // Push the particle to the list
                particle_list_push(&particles, particle);

                // Decrease counter and reset the clock
                start_timer = current_timer;
                particle_batches_left_to_spawn--;
            }
        }

        // Update title
        char title[50];
        sprintf(title, "particle-simulation - Particles: %lu", particles.buffer_len);
        glfwSetWindowTitle(window, title);

        // Update solver and upload data to GPU
        solver_update_parallel_with_grid(&solver, &particles, &particle_grid, &parallelization_args, SOLVER_DT);
        particle_renderer_upload_from_list(&renderer, &particles);

        // Draw grid
        shader_program_use(&grid_renderer.shader_program);
        shader_program_set_mat4(&grid_renderer.shader_program, "_MProj", user_data.camera.projection_matrix);
        grid_renderer_draw(&grid_renderer);

        // Draw particles
        shader_program_use(&renderer.shader_program);
        shader_program_set_mat4(&renderer.shader_program, "_MProj", user_data.camera.projection_matrix);
        particle_renderer_draw(&renderer);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    solver_delete(&solver);
    particle_grid_delete(&particle_grid);
    particle_list_delete(&particles);

    grid_renderer_delete(&grid_renderer);
    particle_renderer_delete(&renderer);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
