#ifndef SOLVERS_COMMON_H
#define SOLVERS_COMMON_H

#include "solver.h"

void solver_apply_gravity(Solver *solver, ParticleList *list);
void solver_update_positions_and_apply_constraints(Solver *solver, ParticleList *list, float dt);
void solver_solve_particle_collision(Particle *first, Particle *second);

#endif /* SOLVERS_COMMON_H */
