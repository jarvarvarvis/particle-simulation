#ifndef PARTICLE_DATA_H
#define PARTICLE_DATA_H

#include "../opengl/buffer.h"

#include "../../thirdparty/c_math2d.h"

typedef struct {
    Buffer position_and_radius_buffer;
    Buffer color_buffer;
    int particle_count;
} ParticleGPUData;

ParticleGPUData particle_gpu_data_new();
void particle_gpu_data_bind_buffers(ParticleGPUData *particle_gpu_data);
void particle_gpu_data_delete(ParticleGPUData *particle_gpu_data);

typedef struct {
    cm2_vec4 position_and_radius;
    cm2_vec4 color;
} Particle;

Particle particle_new(float x, float y, float radius, float r, float g, float b, float a);

#ifndef PARTICLE_LIST_INITIAL_CAP
#define PARTICLE_LIST_INITIAL_CAP 16
#endif /* PARTICLE_LIST_INITIAL_CAP */

typedef struct {
    Particle *buffer;
    size_t buffer_len, buffer_cap;
} ParticleList;

ParticleList particle_list_new();
void particle_list_push(ParticleList *particle_list, Particle particle);
void particle_list_upload(ParticleList *particle_list, ParticleGPUData *gpu_data);
void particle_list_delete(ParticleList *particle_list);

#endif /* PARTICLE_DATA_H */
