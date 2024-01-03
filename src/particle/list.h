#ifndef PARTICLE_LIST_H
#define PARTICLE_LIST_H

#include "data.h"
#include "particle.h"
#include "iterator.h"


#ifndef PARTICLE_LIST_INITIAL_CAP
#define PARTICLE_LIST_INITIAL_CAP 16
#endif /* PARTICLE_LIST_INITIAL_CAP */

typedef struct {
    Particle *buffer;
    size_t buffer_len, buffer_cap;
} ParticleList;

typedef struct {
    int current_idx;
    ParticleList *list;
} ParticleListIteratorState;

ParticleList particle_list_new();
void particle_list_push(ParticleList *particle_list, Particle particle);
void particle_list_upload(ParticleList *particle_list, ParticleGpuData *gpu_data);
ParticleIterator particle_list_iterate(ParticleList *particle_list);
void particle_list_delete(ParticleList *particle_list);

#endif /* PARTICLE_LIST_H */

