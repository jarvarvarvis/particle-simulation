#include "simulation.h"

#include <stdlib.h>

Solver solver_new() {
    Solver solver;
    solver.gravity = cm2_vec2_new(0.0, -1000.0);
    solver.constraint = NULL;
    return solver;
}

void solver_update(Solver *solver, ParticleIterator *iterator, float dt) {
    Particle *iter_curr;

    // Apply gravity to all particles
    iterator->reset(iterator);
    while ((iter_curr = iterator->advance(iterator))) {
        particle_accelerate(iter_curr, solver->gravity);
    }

    // Update positions of all particles and apply constraint
    iterator->reset(iterator);
    while ((iter_curr = iterator->advance(iterator))) {
        particle_update_position(iter_curr, dt);

        Constraint *constraint = solver->constraint;
        if (constraint) {
            // If the particle is outside the constraint, move it back
            if (constraint->is_outside(constraint, iter_curr)) {
                constraint->apply(constraint, iter_curr);
            }
        }
    }
}

void solver_delete(Solver *solver) {
    constraint_delete(solver->constraint);
    free(solver->constraint);
}
