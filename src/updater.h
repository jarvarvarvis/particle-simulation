#ifndef PARTICLE_UPDATER_H
#define PARTICLE_UPDATER_H

#include "particle/list.h"
#include "particle/grid/grid.h"
#include "particle/solver/solver.h"

#include <time.h>
#include <pthread.h>

typedef struct {
    cm2_vec2 spawn_position;
    cm2_vec2 spawn_velocity;
    float spawn_radius;
    size_t particles_left_to_spawn;
} ParticleEmitter;

ParticleEmitter particle_emitter_new(size_t particle_count, cm2_vec2 spawn_position, cm2_vec2 spawn_velocity, float spawn_radius);
void particle_emitter_spawn_random_colored(ParticleEmitter *emitter, ParticleList *list);

typedef struct {
    ParticleList particle_list;
    ParticleGrid particle_grid;
    Solver solver;

    float particle_spawn_time_interval;
    struct timespec particle_spawn_timer;
    ParticleEmitter *emitters;
    size_t emitter_count;
} ParticleUpdater;

ParticleUpdater particle_updater_new(size_t emitter_count);
void particle_updater_update(ParticleUpdater *updater);
void particle_updater_delete(ParticleUpdater *updater);

#endif /* PARTICLE_UPDATER_H */
