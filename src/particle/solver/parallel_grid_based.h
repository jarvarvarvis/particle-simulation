#ifndef PARALLEL_GRID_BASED_SOLVER_H
#define PARALLEL_GRID_BASED_SOLVER_H

#include "solver.h"

typedef struct {
    size_t section_count;
} ParallelGridBasedSolverParams;

typedef struct {
    ParticleGrid *grid;
    ParticleList *list;
    ParallelGridBasedSolverParams params;
} ParallelGridBasedSolverData;

Solver solver_parallel_grid_based_new(Solver solver_base);

#endif /* PARALLEL_GRID_BASED_SOLVER_H */

