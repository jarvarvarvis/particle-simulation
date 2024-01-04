#include "simulation.h"

#include <stdlib.h>
#include <float.h>

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

    // If distance is zero, do nothing
    if (fabs(dist) < FLT_EPSILON) {
        return;
    }

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

void solver_delete(Solver *solver) {
    constraint_delete(solver->constraint);
    free(solver->constraint);
}
