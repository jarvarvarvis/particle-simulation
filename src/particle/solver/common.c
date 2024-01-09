#include "common.h"

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
    float dist = cm2_vec2_length(collision_axis);

    float radius_sum = first->radius + second->radius;
    if (dist < radius_sum) {
        cm2_vec2 normal = cm2_vec2_scale(collision_axis, 1.0 / dist);
        float delta = radius_sum - dist;

        first->position = cm2_vec2_add(first->position, cm2_vec2_scale(normal, 0.5 * delta));
        second->position = cm2_vec2_sub(second->position, cm2_vec2_scale(normal, 0.5 * delta));
    }
}

