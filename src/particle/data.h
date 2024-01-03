#ifndef PARTICLE_DATA_H
#define PARTICLE_DATA_H

#include "../opengl/buffer.h"

#include "../../thirdparty/c_math2d.h"


#ifndef PARTICLE_LIST_INITIAL_CAP
#define PARTICLE_LIST_INITIAL_CAP 16
#endif /* PARTICLE_LIST_INITIAL_CAP */


#ifndef PARTICLE_GPU_DATA_STAGING_BUFFER_INITIAL_CAP
#define PARTICLE_GPU_DATA_STAGING_BUFFER_INITIAL_CAP 16
#endif /* PARTICLE_GPU_DATA_STAGING_BUFFER_INITIAL_CAP */


typedef struct {
    cm2_vec4 *position_and_radius_buffer;
    cm2_vec4 *color_buffer;
    size_t capacity;
} ParticleGpuDataStagingBuffer;

ParticleGpuDataStagingBuffer particle_gpu_data_staging_buffer_new();
void particle_gpu_data_staging_buffer_resize(ParticleGpuDataStagingBuffer *staging_buffer, size_t new_cap);
void particle_gpu_data_staging_buffer_delete(ParticleGpuDataStagingBuffer *staging_buffer);

typedef struct {
    Buffer position_and_radius_buffer;
    Buffer color_buffer;
    int particle_count;
    ParticleGpuDataStagingBuffer staging_buffer;
} ParticleGpuData;

ParticleGpuData particle_gpu_data_new();
void particle_gpu_data_bind_buffers(ParticleGpuData *particle_gpu_data);
void particle_gpu_data_delete(ParticleGpuData *particle_gpu_data);

typedef struct {
    cm2_vec2 position;
    cm2_vec2 last_position;
    cm2_vec2 acceleration;
    float radius;

    cm2_vec4 color;
} Particle;

Particle particle_new(float x, float y, float radius, float r, float g, float b, float a);

typedef struct {
    Particle *buffer;
    size_t buffer_len, buffer_cap;
} ParticleList;

ParticleList particle_list_new();
void particle_list_push(ParticleList *particle_list, Particle particle);
void particle_list_upload(ParticleList *particle_list, ParticleGpuData *gpu_data);
void particle_list_delete(ParticleList *particle_list);

#endif /* PARTICLE_DATA_H */
