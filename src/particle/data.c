#include "data.h"

#include <stdlib.h>
#include <stdbool.h>

#include "../../thirdparty/c_log.h"

ParticleGPUData particle_gpu_data_new() {
    ParticleGPUData data;

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

    return data;
}

void particle_gpu_data_bind_buffers(ParticleGPUData *particle_gpu_data) {
    buffer_bind(&particle_gpu_data->position_and_radius_buffer);
    buffer_bind(&particle_gpu_data->color_buffer);
}

void particle_gpu_data_delete(ParticleGPUData *particle_gpu_data) {
    buffer_delete(&particle_gpu_data->position_and_radius_buffer);
    buffer_delete(&particle_gpu_data->color_buffer);
}

Particle particle_new(float x, float y, float radius, float r, float g, float b, float a) {
    Particle particle;
    particle.position_and_radius = cm2_vec4_new(x, y, 0.0, radius);
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

void particle_list_upload(ParticleList *particle_list, ParticleGPUData *gpu_data) {
    // Create temporary buffers for the particle position/radius and colors
    int buf_size = sizeof(cm2_vec4) * particle_list->buffer_len;

    cm2_vec4 *positions_and_radii = (cm2_vec4 *) malloc(buf_size);
    cm2_vec4 *colors = (cm2_vec4 *) malloc(buf_size);

    for (int i = 0; i < particle_list->buffer_len; ++i)  {
        Particle particle = particle_list->buffer[i];
        positions_and_radii[i] = particle.position_and_radius;
        colors[i] = particle.color;
    }

    // Upload data to the GPU
    buffer_bind(&gpu_data->position_and_radius_buffer);
    buffer_upload_data_static(&gpu_data->position_and_radius_buffer, positions_and_radii, buf_size);
    buffer_bind(&gpu_data->color_buffer);
    buffer_upload_data_static(&gpu_data->color_buffer, colors, buf_size);

    // Free temporary buffers
    free(positions_and_radii);
    free(colors);

    // Update particle count
    gpu_data->particle_count = particle_list->buffer_len;
}

void particle_list_delete(ParticleList *particle_list) {
    free(particle_list->buffer);
}
