#include "basic.h"
#include "common.h"

void solver_basic_solve_collisions(Solver *solver, ParticleList *list) {
    for (size_t first_idx = 0; first_idx < list->buffer_len; ++first_idx) {
        Particle *first = &list->buffer[first_idx];
        for (size_t second_idx = first_idx + 1; second_idx < list->buffer_len; ++second_idx) {
            Particle *second = &list->buffer[second_idx];
            solver_solve_particle_collision(first, second);
        }
    }
}

void solver_basic_update(Solver *solver, void *data, float dt) {
    BasicSolverData *solver_data = data;
    ParticleList *list = solver_data->list;

    // Apply gravity to all particles
    solver_apply_gravity(solver, list);

    // Update positions of all particles and apply constraints
    solver_update_positions_and_apply_constraints(solver, list, dt);

    // Solve collisions
    solver_basic_solve_collisions(solver, list);
}

Solver solver_basic_new(Solver solver_base) {
    solver_base.update = solver_basic_update;
    return solver_base;
}
