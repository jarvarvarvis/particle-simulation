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
#include "particle/grid/grid.h"
#include "particle/grid/grid_renderer.h"
#include "particle/renderer.h"
#include "particle/solver/solver.h"
#include "particle/solver/parallel_grid_based.h"
#include "updater.h"
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

    // Create renderer and particle updater
    ParticleRenderer renderer = particle_renderer_new();
    ParticleUpdater particle_updater = particle_updater_new(3);
    particle_updater.particle_list = particle_list_new();
    particle_updater.particle_grid = particle_grid_new(56, 40, 20, 20);

    // Create solver data
    ParallelGridBasedSolverData solver_data;
    solver_data.grid = &particle_updater.particle_grid;
    solver_data.list = &particle_updater.particle_list;
    solver_data.params.section_count = 8; // 56 / 8 = 7

    // Create solver
    const float SOLVER_DT = 0.005;
    const float SOLVER_SUB_STEPS = 8;
    particle_updater.solver = solver_parallel_grid_based_new(solver_new(SOLVER_DT, SOLVER_SUB_STEPS));
    particle_updater.solver.update_data = &solver_data;

    // Create constraint
    particle_updater.solver.constraint = box_constraint_fit_grid(&particle_updater.particle_grid);

    // Initialize RNG
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    srand(time.tv_nsec);

    // Create emitters
    particle_updater.particle_spawn_time_interval = 1.0;

    float grid_half_w = (particle_updater.particle_grid.width  * particle_updater.particle_grid.cell_width ) / 2.;
    float grid_half_h = (particle_updater.particle_grid.height * particle_updater.particle_grid.cell_height) / 2.;
    particle_updater.emitters[0] = particle_emitter_new(
        1600,
        cm2_vec2_new(-grid_half_w + 10.0, grid_half_h - 10.0),
        cm2_vec2_new(1.3, -0.1),
        5.0
    );
    particle_updater.emitters[1] = particle_emitter_new(
        1600,
        cm2_vec2_new(0.0, grid_half_h - 10.0),
        cm2_vec2_new(0.01, -3.0),
        8.0
    );
    particle_updater.emitters[2] = particle_emitter_new(
        1600,
        cm2_vec2_new(grid_half_w - 20.0, grid_half_h - 10.0),
        cm2_vec2_new(-1.4, -0.8),
        6.0
    );

    // Create grid renderer
    GridRenderer grid_renderer = grid_renderer_from_particle_grid(&particle_updater.particle_grid);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        // Update title
        char title[50];
        sprintf(title, "particle-simulation - Particles: %lu", particle_updater.particle_list.buffer_len);
        glfwSetWindowTitle(window, title);

        // Upload data to GPU
        particle_updater_update(&particle_updater);
        particle_renderer_upload_from_list(&renderer, &particle_updater.particle_list);

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

    particle_updater_delete(&particle_updater);

    grid_renderer_delete(&grid_renderer);
    particle_renderer_delete(&renderer);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
