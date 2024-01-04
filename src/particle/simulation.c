#include "simulation.h"

#include <stdlib.h>
#include <float.h>

#include "../../thirdparty/c_log.h"
#include "grid.h"

Solver solver_new(size_t sub_steps) {
    Solver solver;
    solver.sub_steps = sub_steps;
    solver.gravity = cm2_vec2_new(0.0, -1000.0);
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
void solver_solve_collisions_with_grid(Solver *solver, ParticleGrid *grid) {
    for (size_t y = 0; y < grid->height; ++y) {
        for (size_t x = 0; x < grid->width; ++x) {
            // Get the current cell
            ParticleGridCell *cell = particle_grid_cell_at(grid, x, y);

            // Also get cells in 3x3 grid around the current cell
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

                    // Iterate over particles in both cells and solve collisions between them
                    for (size_t i = 0; i < other_cell->buffer_len; ++i) {
                        for (size_t j = 0; j < cell->buffer_len; ++j) {
                            Particle *first = other_cell->buffer[i];
                            Particle *second = cell->buffer[j];

                            // Don't solve collision for same particle
                            if (first == second) {
                                continue;
                            }

                            solver_solve_particle_collision(first, second);
                        }
                    }
                }
            }
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
        solver_solve_collisions_with_grid(solver, grid);
    }
}

void solver_delete(Solver *solver) {
    constraint_delete(solver->constraint);
    free(solver->constraint);
}
