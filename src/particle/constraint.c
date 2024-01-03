#include "constraint.h"

#include <stdlib.h>

void constraint_delete(Constraint *constraint) {
    free(constraint->data);
}

bool circular_constraint_is_outside(struct Constraint *constraint, Particle *particle) {
    CircularConstraint *constraint_data = (CircularConstraint*) constraint->data;
    cm2_vec2 to_obj = cm2_vec2_sub(particle->position, constraint_data->center);
    float dist = cm2_vec2_dist(to_obj);

    return dist > (constraint_data->radius - particle->radius);
}

void circular_constraint_apply(struct Constraint *constraint, Particle *particle) {
    CircularConstraint *constraint_data = (CircularConstraint*) constraint->data;
    cm2_vec2 to_obj = cm2_vec2_sub(particle->position, constraint_data->center);
    float dist = cm2_vec2_dist(to_obj);

    cm2_vec2 normal = cm2_vec2_scale(to_obj, 1.0 / dist);
    cm2_vec2 vec_to_constraint_edge = cm2_vec2_scale(normal, constraint_data->radius - particle->radius);

    particle->position = cm2_vec2_add(constraint_data->center, vec_to_constraint_edge);
}

Constraint *circular_constraint_new(cm2_vec2 center, float radius) {
    CircularConstraint *circular_constraint = (CircularConstraint*) malloc(sizeof(CircularConstraint));
    circular_constraint->center = center;
    circular_constraint->radius = radius;

    Constraint *constraint = (Constraint*) malloc(sizeof(Constraint));
    constraint->data = circular_constraint;
    constraint->is_outside = circular_constraint_is_outside;
    constraint->apply = circular_constraint_apply;
    return constraint;
}
