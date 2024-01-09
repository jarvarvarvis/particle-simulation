#ifndef GRID_BASED_SOLVER_H
#define GRID_BASED_SOLVER_H

#include "solver.h"

typedef struct {
    ParticleGrid *grid;
    ParticleList *list;
} GridBasedSolverData;

Solver solver_grid_based_new(Solver solver_base);
void solver_grid_based_solve_neighbors(
    ParticleList *list,
    ParticleGrid *grid,
    ParticleGridCell *cell,
    size_t x, size_t y
);

#endif /* GRID_BASED_SOLVER_H */
