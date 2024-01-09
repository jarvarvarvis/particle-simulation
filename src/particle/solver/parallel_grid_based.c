#include "parallel_grid_based.h"
#include "common.h"
#include "grid_based.h"

#include <stdlib.h>
#include <pthread.h>

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
            solver_grid_based_solve_neighbors(args->list, grid, cell, x, y);
        }
    }

    return NULL;
}

void solver_solve_collisions_with_grid_parallel(
    Solver *solver,
    ParticleList *list,
    ParticleGrid *grid,
    ParallelGridBasedSolverParams *params
) {
    // Limit section count to the grid width
    size_t section_count = params->section_count;
    if (params->section_count > grid->width) {
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

        // Create arguments with the list, grid and current start and end indices
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

    // Wait for all threads to finish
    for (size_t i = 0; i < section_count; ++i) {
        pthread_join(thread_ids[i], NULL);
    }

    free(args);
}


void solver_parallel_grid_based_update(Solver *solver, void *data, float dt) {
    ParallelGridBasedSolverData* solver_data = data;
    ParticleList *list = solver_data->list;
    ParticleGrid *grid = solver_data->grid;
    ParallelGridBasedSolverParams *params = &solver_data->params;

    // Apply gravity to all particles
    solver_apply_gravity(solver, list);

    // Update positions of all particles and apply constraints
    solver_update_positions_and_apply_constraints(solver, list, dt);

    // Clear grid and insert particles into it
    particle_grid_clear(grid);
    particle_grid_insert_all(grid, list);

    // Solve collisions
    solver_solve_collisions_with_grid_parallel(solver, list, grid, params);
}

Solver solver_parallel_grid_based_new(Solver solver_base) {
    solver_base.update = solver_parallel_grid_based_update;
    return solver_base;
}
