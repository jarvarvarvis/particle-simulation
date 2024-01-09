#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdbool.h>

#include "../constraint.h"
#include "../grid.h"
#include "../list.h"

#include "../../../thirdparty/c_math2d.h"

struct Solver;

typedef void (*SolverUpdateFn)(struct Solver *solver, void *data, float dt);

struct Solver {
    float delta_time;
    size_t sub_steps;

    cm2_vec2 gravity;
    Constraint *constraint;

    void *update_data;
    SolverUpdateFn update;
};

typedef struct Solver Solver;


Solver solver_new(float delta_time, size_t sub_steps);
void solver_update(Solver *solver);
void solver_delete(Solver *solver);

#endif /* SIMULATION_H */
