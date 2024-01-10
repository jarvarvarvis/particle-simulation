#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include <stdbool.h>

#include "particle.h"
#include "grid/grid.h"

struct Constraint;

typedef void (*ApplyConstraintFn)(struct Constraint *constraint, Particle *particle);

struct Constraint {
    void *data;
    ApplyConstraintFn apply;
};

typedef struct Constraint Constraint;

void constraint_delete(Constraint *constraint);


typedef struct {
    cm2_vec2 center;
    float radius;
} CircularConstraint;

Constraint *circular_constraint_new(cm2_vec2 center, float radius);


typedef struct {
    cm2_vec2 min;
    cm2_vec2 max;
} BoxConstraint;

Constraint *box_constraint_new(cm2_vec2 min, cm2_vec2 max);
Constraint *box_constraint_fit_grid(ParticleGrid *grid);

#endif /* CONSTRAINT_H */
