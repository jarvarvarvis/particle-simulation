#ifndef PARTICLE_LIST_H
#define PARTICLE_LIST_H

#include "data.h"
#include "particle.h"


#ifndef PARTICLE_LIST_INITIAL_CAP
#define PARTICLE_LIST_INITIAL_CAP 16
#endif /* PARTICLE_LIST_INITIAL_CAP */

typedef struct {
    Particle *buffer;
    size_t buffer_len, buffer_cap;
} ParticleList;

ParticleList particle_list_new();
void particle_list_push(ParticleList *particle_list, Particle particle);
void particle_list_upload(ParticleList *particle_list, ParticleGpuData *gpu_data);
void particle_list_delete(ParticleList *particle_list);

#endif /* PARTICLE_LIST_H */

