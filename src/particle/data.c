#include "data.h"

#include <stdlib.h>
#include <stdbool.h>

#include "../../thirdparty/c_log.h"

ParticleGpuDataStagingBuffer particle_gpu_data_staging_buffer_new() {
    ParticleGpuDataStagingBuffer staging_buffer;
    size_t capacity = PARTICLE_GPU_DATA_STAGING_BUFFER_INITIAL_CAP;
    staging_buffer.position_and_radius_buffer = (cm2_vec4 *) malloc(sizeof(cm2_vec4) * capacity);
    staging_buffer.color_buffer = (cm2_vec4 *) malloc(sizeof(cm2_vec4) * capacity);
    staging_buffer.capacity = capacity;

    c_log(C_LOG_SEVERITY_DEBUG, "Staging buffer allocation: size = %lu", capacity);

    return staging_buffer;
}

void particle_gpu_data_staging_buffer_resize(ParticleGpuDataStagingBuffer *staging_buffer, size_t new_cap) {
    if (new_cap <= staging_buffer->capacity) {
        // Don't reallocate if the new capacity is less than the current capacity
        return;
    }

    c_log(C_LOG_SEVERITY_DEBUG, "Staging buffer reallocation: size = %lu", new_cap);

    staging_buffer->position_and_radius_buffer = (cm2_vec4 *)
        realloc(staging_buffer->position_and_radius_buffer, sizeof(cm2_vec4) * new_cap);
    staging_buffer->color_buffer = (cm2_vec4 *)
        realloc(staging_buffer->color_buffer, sizeof(cm2_vec4) * new_cap);
    staging_buffer->capacity = new_cap;
}

void particle_gpu_data_staging_buffer_delete(ParticleGpuDataStagingBuffer *staging_buffer) {
    free(staging_buffer->position_and_radius_buffer);
    free(staging_buffer->color_buffer);
}

ParticleGpuData particle_gpu_data_new() {
    ParticleGpuData data;

    // Set vertex attribute pointer for position and radius buffer
    data.position_and_radius_buffer = buffer_new(GL_ARRAY_BUFFER);
    buffer_bind(&data.position_and_radius_buffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    // Set vertex attribute pointer for color buffer
    data.color_buffer = buffer_new(GL_ARRAY_BUFFER);
    buffer_bind(&data.color_buffer);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), NULL);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // Create staging buffer
    data.staging_buffer = particle_gpu_data_staging_buffer_new();

    return data;
}

void particle_gpu_data_bind_buffers(ParticleGpuData *particle_gpu_data) {
    buffer_bind(&particle_gpu_data->position_and_radius_buffer);
    buffer_bind(&particle_gpu_data->color_buffer);
}

void particle_gpu_data_delete(ParticleGpuData *particle_gpu_data) {
    buffer_delete(&particle_gpu_data->position_and_radius_buffer);
    buffer_delete(&particle_gpu_data->color_buffer);
    particle_gpu_data_staging_buffer_delete(&particle_gpu_data->staging_buffer);
}
