#include "simulation.h"

Solver solver_new() {
    Solver solver;
    solver.gravity = cm2_vec2_new(0.0, -1000.0);
    return solver;
}

void solver_update(Solver *solver, ParticleIterator *iterator, float dt) {
    Particle *iter_curr;

    // Apply gravity to all particles
    iterator->reset(iterator);
    while ((iter_curr = iterator->advance(iterator))) {
        particle_accelerate(iter_curr, solver->gravity);
    }

    // Update positions of all particles
    iterator->reset(iterator);
    while ((iter_curr = iterator->advance(iterator))) {
        particle_update_position(iter_curr, dt);
    }
}
