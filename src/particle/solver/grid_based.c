#include "grid_based.h"
#include "common.h"

void solver_grid_based_solve_grid_cells(
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

void solver_grid_based_solve_neighbors(
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
            solver_grid_based_solve_grid_cells(list, cell, other_cell);
        }
    }
}

void solver_grid_based_solve_collisions_with_grid(
    Solver *solver,
    ParticleList *list,
    ParticleGrid *grid
) {
    for (size_t y = 0; y < grid->height; ++y) {
        for (size_t x = 0; x < grid->width; ++x) {
            // Get the current cell and solve collisions with neighbors
            ParticleGridCell *cell = particle_grid_cell_at(grid, x, y);
            solver_grid_based_solve_neighbors(list, grid, cell, x, y);
        }
    }
}

void solver_grid_based_update(Solver *solver, void *data, float dt) {
    GridBasedSolverData *solver_data = data;
    ParticleList *list = solver_data->list;
    ParticleGrid *grid = solver_data->grid;

    // Apply gravity to all particles
    solver_apply_gravity(solver, list);

    // Update positions of all particles and apply constraints
    solver_update_positions_and_apply_constraints(solver, list, dt);

    // Clear grid and insert particles into it
    particle_grid_clear(grid);
    particle_grid_insert_all(grid, list);

    // Solve collisions
    solver_grid_based_solve_collisions_with_grid(solver, list, grid);
}

Solver solver_grid_based_new(Solver solver_base) {
    solver_base.update = solver_grid_based_update;
    return solver_base;
}
