#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <stdbool.h>

#include "particle.h"

struct Constraint;

typedef bool (*OutsideConstraintFn)(struct Constraint *constraint, Particle *particle);
typedef void (*ApplyConstraintFn)(struct Constraint *constraint, Particle *particle);

struct Constraint {
    void *data;
    OutsideConstraintFn is_outside;
    ApplyConstraintFn apply;
};

typedef struct Constraint Constraint;

void constraint_delete(Constraint *constraint);


typedef struct {
    cm2_vec2 center;
    float radius;
} CircularConstraint;

Constraint *circular_constraint_new(cm2_vec2 center, float radius);

#endif /* CONSTRAINT_H */
