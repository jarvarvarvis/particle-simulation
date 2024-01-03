#ifndef SIMULATION_H
#define SIMULATION_H

#include "iterator.h"

#include "../../thirdparty/c_math2d.h"

typedef struct {
    cm2_vec2 gravity;
} Solver;

Solver solver_new();
void solver_update(Solver *solver, ParticleIterator *iterator, float dt);

#endif /* SIMULATION_H */
