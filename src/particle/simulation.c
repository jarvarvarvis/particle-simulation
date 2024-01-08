#include "simulation.h"
#include "grid.h"
#include "grid_cell.h"

#include <stdlib.h>
#include <float.h>
#include <pthread.h>

#include "../../thirdparty/c_log.h"

Solver solver_new(size_t sub_steps) {
    Solver solver;
    solver.sub_steps = sub_steps;
    solver.gravity = cm2_vec2_new(0.0, -5000.0);
    solver.constraint = NULL;
    return solver;
}

void solver_apply_gravity(Solver *solver, ParticleList *list) {
    for (size_t i = 0; i < list->buffer_len; ++i) {
        Particle *curr = &list->buffer[i];
        particle_accelerate(curr, solver->gravity);
    }
}

void solver_update_positions_and_apply_constraints(Solver *solver, ParticleList *list, float dt) {
    for (size_t i = 0; i < list->buffer_len; ++i) {
        Particle *curr = &list->buffer[i];
        particle_update_position(curr, dt);

        Constraint *constraint = solver->constraint;
        if (constraint) {
            // If the particle is outside the constraint, move it back
            constraint->apply(constraint, curr);
        }
    }
}

void solver_solve_particle_collision(Particle *first, Particle *second) {
    cm2_vec2 collision_axis = cm2_vec2_sub(first->position, second->position);
    float dist = cm2_vec2_dist(collision_axis);

    float radius_sum = first->radius + second->radius;
    if (dist < radius_sum) {
        cm2_vec2 normal = cm2_vec2_scale(collision_axis, 1.0 / dist);
        float delta = radius_sum - dist;

        first->position = cm2_vec2_add(first->position, cm2_vec2_scale(normal, 0.5 * delta));
        second->position = cm2_vec2_sub(second->position, cm2_vec2_scale(normal, 0.5 * delta));
    }
}

void solver_solve_collisions(Solver *solver, ParticleList *list) {
    for (size_t first_idx = 0; first_idx < list->buffer_len; ++first_idx) {
        Particle *first = &list->buffer[first_idx];
        for (size_t second_idx = first_idx + 1; second_idx < list->buffer_len; ++second_idx) {
            Particle *second = &list->buffer[second_idx];
            solver_solve_particle_collision(first, second);
        }
    }
}

void solver_update(Solver *solver, ParticleList *list, float dt) {
    float sub_dt = dt / (float)solver->sub_steps;

    for (size_t i = 0; i < solver->sub_steps; ++i) {
        // Apply gravity to all particles
        solver_apply_gravity(solver, list);

        // Update positions of all particles and apply constraints
        solver_update_positions_and_apply_constraints(solver, list, sub_dt);

        // Solve collisions
        solver_solve_collisions(solver, list);
    }
}

void solver_solve_grid_cells(
    ParticleList *list,
    ParticleGridCell *cell,
    ParticleGridCell *other_cell
) {
    // Iterate over particles in both cells and solve collisions between them
    for (size_t i = 0; i < cell->buffer_len; ++i) {
        for (size_t j = 0; j < other_cell->buffer_len; ++j) {
            Particle *first = &list->buffer[cell->indices[i]];
            Particle *second = &list->buffer[other_cell->indices[j]];

            // Don't solve collision for same particle
            if (first == second) {
                continue;
            }

            solver_solve_particle_collision(first, second);
        }
    }
}

void solver_solve_grid_cell_neighbors(
    ParticleList *list,
    ParticleGrid *grid,
    ParticleGridCell *cell,
    size_t x, size_t y
) {
    // Get cells in 3x3 grid around the current cell
    for (long dy = -1; dy <= 1; ++dy) {
        for (long dx = -1; dx <= 1; ++dx) {
            // Skip coordinates that will cause an overflow
            if (dx == -1 && x == 0) continue;
            if (dy == -1 && y == 0) continue;

            // Skip positions that are outside of the grid
            size_t other_x = x + dx;
            size_t other_y = y + dy;
            if (!particle_grid_is_position_inside_grid(grid, other_x, other_y)) {
                continue;
            }

            ParticleGridCell *other_cell = particle_grid_cell_at(grid, other_x, other_y);
            solver_solve_grid_cells(list, cell, other_cell);
        }
    }
}

void solver_solve_collisions_with_grid(
    Solver *solver,
    ParticleList *list,
    ParticleGrid *grid
) {
    for (size_t y = 0; y < grid->height; ++y) {
        for (size_t x = 0; x < grid->width; ++x) {
            // Get the current cell and solve collisions with neighbors
            ParticleGridCell *cell = particle_grid_cell_at(grid, x, y);
            solver_solve_grid_cell_neighbors(list, grid, cell, x, y);
        }
    }
}

void solver_update_with_grid(Solver *solver, ParticleList *list, ParticleGrid *grid, float dt) {
    float sub_dt = dt / (float)solver->sub_steps;

    for (size_t i = 0; i < solver->sub_steps; ++i) {
        // Apply gravity to all particles
        solver_apply_gravity(solver, list);

        // Update positions of all particles and apply constraints
        solver_update_positions_and_apply_constraints(solver, list, sub_dt);

        // Clear grid and insert particles into it
        particle_grid_clear(grid);
        particle_grid_insert_all(grid, list);

        // Solve collisions
        solver_solve_collisions_with_grid(solver, list, grid);
    }
}

typedef struct {
    ParticleList *list;
    ParticleGrid *grid;
    size_t start_x, end_x;
} SectionSolverThreadArgs;

void *solver_solve_section(void *argvp) {
    SectionSolverThreadArgs *args = argvp;
    ParticleGrid *grid = args->grid;

    // For the parallel solver, iterating over the columns first is better, since particles are more likely
    // to collect horizontally. By doing so, we are more likely to split up the work between threads evenly.
    //
    // If we were to first iterate over the rows, the last thread might spend its time computing collisions
    // for all the particles (if they have collected at the bottom of the grid).
    for (size_t x = args->start_x; x < args->end_x; ++x) {
        for (size_t y = 0; y < grid->height; ++y) {
            // Get the current cell and solve collisions with neighbors
            ParticleGridCell *cell = particle_grid_cell_at(grid, x, y);
            solver_solve_grid_cell_neighbors(args->list, grid, cell, x, y);
        }
    }

    return NULL;
}

void solver_solve_collisions_with_grid_parallel(
    Solver *solver,
    ParticleList *list,
    ParticleGrid *grid,
    SolverParallelizationArgs *parallelization_args
) {
    // Limit section count to the grid width
    size_t section_count = parallelization_args->section_count;
    if (parallelization_args->section_count > grid->width) {
        section_count = grid->width;
    }

    size_t section_width = grid->width / section_count;

    size_t section_remainder = grid->width % section_count;

    size_t curr_start_x = 0;
    size_t curr_end_x = section_width;

    SectionSolverThreadArgs *args = (SectionSolverThreadArgs*)
        malloc(sizeof(SectionSolverThreadArgs) * section_count);
    pthread_t thread_ids[section_count];

    // Create section arguments
    for (size_t i = 0; i < section_count; ++i) {
        // First consider the easiest case: the split count evenly divides the width of the grid
        // For example: width = 9, section_count = 3
        // One row is divided like the following: # # # | # # # | # # #
        //
        // The case where the split count *doesn't* divide the width evenly is a little more complicated
        // For example: width = 10, section_count = 3
        // A row might then be divided like the following: # # # | # # # | # # # #
        //
        // Or even more complex: width = 20, section_count = 8
        // Naive:  # # | # # | # # | # # | # # | # # | # # | # # # # # #
        // Better: # # # | # # # | # # # | # # # | # # | # # | # # | # #

        // If the section remainder is greater than 0, increment `curr_end_x` for even division of work
        // and then decrement the section remainder
        if (section_remainder > 0) {
            curr_end_x++;
            section_remainder--;
        }

        // Spawn the thread
        args[i].list = list;
        args[i].grid = grid;
        args[i].start_x = curr_start_x;
        args[i].end_x = curr_end_x;

        // The new start value is now `curr_end_x` and the end value is incremented by `section_width`
        curr_start_x = curr_end_x;
        curr_end_x += section_width;
    }

    // Spawn threads
    for (size_t i = 0; i < section_count; ++i) {
        pthread_create(&thread_ids[i], NULL, solver_solve_section, &args[i]);
    }

    // Wait or all threads to finish
    for (size_t i = 0; i < section_count; ++i) {
        pthread_join(thread_ids[i], NULL);
    }

    free(args);
}


void solver_update_parallel_with_grid(
    Solver *solver,
    ParticleList *list,
    ParticleGrid *grid,
    SolverParallelizationArgs *parallelization_args,
    float dt
) {
    float sub_dt = dt / (float)solver->sub_steps;

    for (size_t i = 0; i < solver->sub_steps; ++i) {
        // Apply gravity to all particles
        solver_apply_gravity(solver, list);

        // Update positions of all particles and apply constraints
        solver_update_positions_and_apply_constraints(solver, list, sub_dt);

        // Clear grid and insert particles into it
        particle_grid_clear(grid);
        particle_grid_insert_all(grid, list);

        // Solve collisions
        solver_solve_collisions_with_grid_parallel(solver, list, grid, parallelization_args);
    }
}

void solver_delete(Solver *solver) {
    constraint_delete(solver->constraint);
    free(solver->constraint);
}
