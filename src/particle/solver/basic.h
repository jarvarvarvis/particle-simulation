#ifndef BASIC_SOLVER_H
#define BASIC_SOLVER_H

#include "solver.h"

typedef struct {
    ParticleList *list;
} BasicSolverData;

Solver solver_basic_new(Solver solver_base);

#endif /* BASIC_SOLVER_H */
