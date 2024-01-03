#include "simulation.h"

#include <stdlib.h>

#include "../../thirdparty/c_log.h"

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

void solver_solve_grid_cell_collisions(ParticleList *list, ParticleGridCell *first, ParticleGridCell *second) {
    for (size_t first_idx = 0; first_idx < first->indices_len; ++first_idx) {
        // If the current index in the first cell is empty (fragmentation can happen!), continue
        long first_particle_idx = first->indices[first_idx];
        if (first_particle_idx == PARTICLE_GRID_CELL_EMPTY) {
            continue;
        }

        for (size_t second_idx = 0; second_idx < second->indices_len; ++second_idx) {
            // If the current index in the second cell is empty (fragmentation can happen!), continue
            long second_particle_idx = second->indices[second_idx];
            if (second_particle_idx == PARTICLE_GRID_CELL_EMPTY) {
                continue;
            }

            // Actually access the buffer
            Particle *first_particle = &list->buffer[first_particle_idx];
            Particle *second_particle = &list->buffer[second_particle_idx];

            // Skip equal particles
            if (first_particle == second_particle) {
                continue;
            }

            solver_solve_particle_collision(first_particle, second_particle);
        }
    }
}

void solver_solve_collisions_with_grid(Solver *solver, ParticleList *list, ParticleGrid *grid) {
    // Skip top and bottom rows
    for (size_t y = 1; y < grid->height - 1; ++y) {
        // Skip left and right columns
        for (size_t x = 1; x < grid->width - 1; ++x) {
            ParticleGridCell *first_cell = particle_grid_get_cell(grid, x, y);

            // Iterate on all surround cells, including the current one
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    ParticleGridCell *second_cell = particle_grid_get_cell(grid, x + dx, y + dy);

                    solver_solve_grid_cell_collisions(list, first_cell, second_cell);
                }
            }
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

void solver_update_with_grid(Solver *solver, ParticleList *list, ParticleGrid *grid, float dt) {
    float sub_dt = dt / (float)solver->sub_steps;

    for (size_t i = 0; i < solver->sub_steps; ++i) {
        // Apply gravity to all particles
        solver_apply_gravity(solver, list);

        // Update positions of all particles and apply constraints
        solver_update_positions_and_apply_constraints(solver, list, sub_dt);

        // Update particle grid positions
        for (size_t i = 0; i < list->buffer_len; ++i) {
            Particle *particle = &list->buffer[i];
            particle_grid_update_moved_particle(grid, particle, i);
        }

        // Solve collisions
        solver_solve_collisions_with_grid(solver, list, grid);
    }
}

void solver_delete(Solver *solver) {
    constraint_delete(solver->constraint);
    free(solver->constraint);
}
