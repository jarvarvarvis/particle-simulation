#include "solver.h"

#include <stdlib.h>
#include <float.h>

#include "../../../thirdparty/c_log.h"

Solver solver_new(float delta_time, size_t sub_steps) {
    Solver solver;
    solver.delta_time = delta_time;
    solver.sub_steps = sub_steps;
    solver.gravity = cm2_vec2_new(0.0, -5000.0);
    solver.constraint = NULL;
    return solver;
}

void solver_update(Solver *solver) {
    float sub_dt = solver->delta_time / (float)solver->sub_steps;

    for (size_t i = 0; i < solver->sub_steps; ++i) {
        solver->update(solver, solver->update_data, sub_dt);
    }
}

void solver_delete(Solver *solver) {
    constraint_delete(solver->constraint);
    free(solver->constraint);
}
