#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#define C_LOG_DEFINITION
#include "../thirdparty/c_log.h"

#define C_MATH2D_DEFINITION
#include "../thirdparty/c_math2d.h"

#include "camera/orthographic.h"
#include "opengl/debug.h"
#include "opengl/window.h"
#include "particle/constraint.h"
#include "particle/grid.h"
#include "particle/grid_renderer.h"
#include "particle/renderer.h"
#include "particle/solver/solver.h"
#include "particle/solver/parallel_grid_based.h"
#include "util/math.h"

int main() {
    if (!glfwInit()) {
        c_log(C_LOG_SEVERITY_ERROR, "Failed to initialize glfw");
        exit(EXIT_FAILURE);
    }

    // Create window
    const int WIDTH = 1200, HEIGHT = 800;
    WindowParameters window_params;
    window_params.context_version = version_new(3, 3);
    window_params.opengl_profile = GLFW_OPENGL_CORE_PROFILE;
    window_params.use_opengl_debug_context = true;
    window_params.window_width = WIDTH;
    window_params.window_height = HEIGHT;
    window_params.window_title = "particle-simulation";
    GLFWwindow *window = window_create_from_params(window_params);
    glfwMakeContextCurrent(window);

    // Set up window user pointer
    WindowUserData user_data = {0};
    user_data.camera = ortho_camera_new(WIDTH, HEIGHT);
    user_data.window_width = WIDTH;
    user_data.window_height = HEIGHT;
    glfwSetWindowUserPointer(window, &user_data);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        c_log(C_LOG_SEVERITY_ERROR, "Failed to initialize GLEW");
        exit(EXIT_FAILURE);
    }

    // Register debug callback
    debug_register_message_callback();

    // Create renderer, particle list and solver
    ParticleRenderer renderer = particle_renderer_new();

    ParticleList particle_list = particle_list_new();
    ParticleGrid particle_grid = particle_grid_new(56, 40, 20, 20);

    ParallelGridBasedSolverData solver_data;
    solver_data.grid = &particle_grid;
    solver_data.list = &particle_list;
    solver_data.params.section_count = 8; // 56 / 8 = 7

    const float SOLVER_DT = 0.004;
    const float SOLVER_SUB_STEPS = 8;
    Solver solver = solver_parallel_grid_based_new(solver_new(SOLVER_DT, SOLVER_SUB_STEPS));
    solver.update_data = &solver_data;

    // Create constraint
    float particle_grid_half_width  = particle_grid.width  * particle_grid.cell_width  / 2.;
    float particle_grid_half_height = particle_grid.height * particle_grid.cell_height / 2.;
    solver.constraint = box_constraint_new(
        cm2_vec2_new(-particle_grid_half_width, -particle_grid_half_height),
        cm2_vec2_new( particle_grid_half_width,  particle_grid_half_height)
    );

    // Create grid renderer
    GridRenderer grid_renderer = grid_renderer_from_particle_grid(&particle_grid);

    // Initialize RNG
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    srand(time.tv_nsec);

    // Particle spawning
    struct timespec start_timer;
    clock_gettime(CLOCK_REALTIME, &start_timer);
    float particle_spawn_time_interval = 1.0;
    int particle_batches_left_to_spawn = 2000;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        // Spawn particles
        if (particle_batches_left_to_spawn > 0) {
            struct timespec current_timer;
            clock_gettime(CLOCK_REALTIME, &current_timer);
            float elapsed_millis = time_diff_ms(start_timer, current_timer);
            if (elapsed_millis > particle_spawn_time_interval) {
                // Create random particle on the left side
                Particle particle_left = particle_new(
                    -particle_grid_half_width + 15.0, particle_grid_half_height - 100,
                    6.0,
                    randf(), randf(), randf(), 1.0
                );

                // Add velocity to particle
                particle_left.position.x += 1.7;
                particle_left.position.y += 0.6;

                // Push the particle to the list
                particle_list_push(&particle_list, particle_left);

                // Create random particle on the right side
                Particle particle_right = particle_new(
                    particle_grid_half_width - 15.0, particle_grid_half_height - 100,
                    6.0,
                    randf(), randf(), randf(), 1.0
                );

                // Add velocity to particle
                particle_right.position.x -= 1.7;
                particle_right.position.y += 0.6;

                // Push the particle to the list
                particle_list_push(&particle_list, particle_right);

                // Decrease counter again and reset clock
                start_timer = current_timer;
                particle_batches_left_to_spawn--;
            }
        }

        // Update title
        char title[50];
        sprintf(title, "particle-simulation - Particles: %lu", particle_list.buffer_len);
        glfwSetWindowTitle(window, title);

        // Update solver and upload data to GPU
        solver_update(&solver);
        particle_renderer_upload_from_list(&renderer, &particle_list);

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
    particle_list_delete(&particle_list);

    grid_renderer_delete(&grid_renderer);
    particle_renderer_delete(&renderer);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
