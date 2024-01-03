#include "simulation.h"

#include <stdlib.h>

Solver solver_new(size_t sub_steps) {
    Solver solver;
    solver.sub_steps = sub_steps;
    solver.gravity = cm2_vec2_new(0.0, -1000.0);
    solver.constraint = NULL;
    return solver;
}

void solver_apply_gravity(Solver *solver, ParticleIterator *iterator) {
    iterator->reset(iterator);
    Particle *iter_curr;

    while ((iter_curr = iterator->advance(iterator))) {
        particle_accelerate(iter_curr, solver->gravity);
    }
}

void solver_update_positions_and_apply_constraints(Solver *solver, ParticleIterator *iterator, float dt) {
    iterator->reset(iterator);
    Particle *iter_curr;

    while ((iter_curr = iterator->advance(iterator))) {
        particle_update_position(iter_curr, dt);

        Constraint *constraint = solver->constraint;
        if (constraint) {
            // If the particle is outside the constraint, move it back
            constraint->apply(constraint, iter_curr);
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

void solver_solve_collisions(Solver *solver, ParticleIterator *iterator) {
    iterator->reset(iterator);
    Particle *iter_1;

    // Create copy of iterator
    ParticleIterator iterator_copy = iterator->copy(iterator);

    int particles_handled = 0;
    while ((iter_1 = iterator->advance(iterator))) {
        particles_handled++;

        iterator_copy.reset(&iterator_copy);

        // Advance the second iterator so that only non-handled particles are left
        for (int i = 0; i < particles_handled; ++i) {
            iterator_copy.advance(&iterator_copy);
        }

        Particle *iter_2;
        while ((iter_2 = iterator_copy.advance(&iterator_copy))) {
            solver_solve_particle_collision(iter_1, iter_2);
        }
    }

    particle_iterator_delete(&iterator_copy);
}

void solver_update(Solver *solver, ParticleIterator *iterator, float dt) {
    float sub_dt = dt / (float)solver->sub_steps;

    for (size_t i = 0; i < solver->sub_steps; ++i) {
        // Apply gravity to all particles
        solver_apply_gravity(solver, iterator);

        // Update positions of all particles and apply constraints
        solver_update_positions_and_apply_constraints(solver, iterator, sub_dt);

        // Solve collisions
        solver_solve_collisions(solver, iterator);
    }
}

void solver_delete(Solver *solver) {
    constraint_delete(solver->constraint);
    free(solver->constraint);
}
