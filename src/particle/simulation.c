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
            if (constraint->is_outside(constraint, iter_curr)) {
                constraint->apply(constraint, iter_curr);
            }
        }
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
            cm2_vec2 collision_axis = cm2_vec2_sub(iter_1->position, iter_2->position);
            float dist = cm2_vec2_dist(collision_axis);

            float radius_sum = iter_1->radius + iter_2->radius;
            if (dist < radius_sum) {
                cm2_vec2 normal = cm2_vec2_scale(collision_axis, 1.0 / dist);
                float delta = radius_sum - dist;

                iter_1->position = cm2_vec2_add(iter_1->position, cm2_vec2_scale(normal, 0.5 * delta));
                iter_2->position = cm2_vec2_sub(iter_2->position, cm2_vec2_scale(normal, 0.5 * delta));
            }
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
