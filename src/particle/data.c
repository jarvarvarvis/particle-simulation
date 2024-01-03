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

Particle particle_new(float x, float y, float radius, float r, float g, float b, float a) {
    Particle particle;
    particle.position = cm2_vec2_new(x, y);
    particle.last_position = cm2_vec2_new(0.0, 0.0);
    particle.acceleration = cm2_vec2_new(0.0, 0.0);
    particle.radius = radius;

    particle.color = cm2_vec4_new(r, g, b, a);
    return particle;
}

ParticleList particle_list_new() {
    ParticleList list;
    list.buffer_len = 0;
    list.buffer_cap = PARTICLE_LIST_INITIAL_CAP;
    list.buffer = (Particle *) malloc(sizeof(Particle) * list.buffer_cap);
    return list;
}

bool particle_list_has_to_grow(ParticleList *particle_list) {
    return particle_list->buffer_len >= particle_list->buffer_cap;
}

void particle_list_push(ParticleList *particle_list, Particle particle) {
    if (particle_list_has_to_grow(particle_list)) {
        particle_list->buffer_cap *= 2; // Grow buffer capacity exponentially
        particle_list->buffer = (Particle *)
            realloc(particle_list->buffer, sizeof(Particle) * particle_list->buffer_cap);
    }

    particle_list->buffer[particle_list->buffer_len] = particle;
    particle_list->buffer_len += 1;
}

void particle_list_upload(ParticleList *particle_list, ParticleGpuData *gpu_data) {
    ParticleGpuDataStagingBuffer *staging_buffer = &gpu_data->staging_buffer;
    particle_gpu_data_staging_buffer_resize(staging_buffer, particle_list->buffer_cap);

    // Copy data from the particle list to the staging buffer
    for (int i = 0; i < particle_list->buffer_len; ++i)  {
        Particle *particle = &particle_list->buffer[i];
        staging_buffer->position_and_radius_buffer[i] =
            cm2_vec4_new(particle->position.x, particle->position.y, 0.0, particle->radius);
        staging_buffer->color_buffer[i] = particle->color;
    }

    // Upload position/radius and color data to the GPU
    buffer_bind(&gpu_data->position_and_radius_buffer);
    buffer_upload_data_static(
        &gpu_data->position_and_radius_buffer,
        staging_buffer->position_and_radius_buffer,
        sizeof(cm2_vec4) * particle_list->buffer_len
    );

    buffer_bind(&gpu_data->color_buffer);
    buffer_upload_data_static(
        &gpu_data->color_buffer,
        staging_buffer->color_buffer,
        sizeof(cm2_vec4) * particle_list->buffer_len
    );

    // Update particle count
    gpu_data->particle_count = particle_list->buffer_len;
}

void particle_list_delete(ParticleList *particle_list) {
    free(particle_list->buffer);
}
