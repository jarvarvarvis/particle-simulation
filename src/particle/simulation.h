#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdbool.h>

#include "constraint.h"
#include "iterator.h"

#include "../../thirdparty/c_math2d.h"

typedef struct {
    size_t sub_steps;
    cm2_vec2 gravity;
    Constraint *constraint;
} Solver;

Solver solver_new(size_t sub_steps);
void solver_solve_particle_collision(Particle *first, Particle *second);
void solver_update(Solver *solver, ParticleIterator *iterator, float dt);
void solver_delete(Solver *solver);

#endif /* SIMULATION_H */
