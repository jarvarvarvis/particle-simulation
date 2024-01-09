#include "constraint.h"

#include <stdlib.h>

void constraint_delete(Constraint *constraint) {
    free(constraint->data);
}


void circular_constraint_apply(struct Constraint *constraint, Particle *particle) {
    CircularConstraint *constraint_data = (CircularConstraint*) constraint->data;
    cm2_vec2 to_obj = cm2_vec2_sub(particle->position, constraint_data->center);
    float dist = cm2_vec2_length(to_obj);

    float radius_diff = constraint_data->radius - particle->radius;
    if (dist > radius_diff) {
        cm2_vec2 normal = cm2_vec2_scale(to_obj, 1.0 / dist);
        cm2_vec2 vec_to_constraint_edge = cm2_vec2_scale(normal, radius_diff);

        particle->position = cm2_vec2_add(constraint_data->center, vec_to_constraint_edge);
    }
}

Constraint *circular_constraint_new(cm2_vec2 center, float radius) {
    CircularConstraint *circular_constraint = (CircularConstraint*) malloc(sizeof(CircularConstraint));
    circular_constraint->center = center;
    circular_constraint->radius = radius;

    Constraint *constraint = (Constraint*) malloc(sizeof(Constraint));
    constraint->data = circular_constraint;
    constraint->apply = circular_constraint_apply;
    return constraint;
}


void box_constraint_apply(struct Constraint *constraint, Particle *particle) {
    BoxConstraint *constraint_data = (BoxConstraint*) constraint->data;

    cm2_vec2 p_pos = particle->position;
    float min_x = constraint_data->min.x + particle->radius;
    float min_y = constraint_data->min.y + particle->radius;
    float max_x = constraint_data->max.x - particle->radius;
    float max_y = constraint_data->max.y - particle->radius;

    // Left edge
    if (p_pos.x < min_x) particle->position.x = min_x;
    // Right edge
    else if (p_pos.x > max_x) particle->position.x = max_x;

    // Top edge
    if (p_pos.y < min_y) particle->position.y = min_y;
    // Bottom edge
    else if (p_pos.y > max_y) particle->position.y = max_y;
}

Constraint *box_constraint_new(cm2_vec2 min, cm2_vec2 max) {
    BoxConstraint *box_constraint = (BoxConstraint*) malloc(sizeof(BoxConstraint));
    box_constraint->min = min;
    box_constraint->max = max;

    Constraint *constraint = (Constraint*) malloc(sizeof(Constraint));
    constraint->data = box_constraint;
    constraint->apply = box_constraint_apply;
    return constraint;
}
