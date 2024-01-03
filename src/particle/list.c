#include "list.h"

#include <stdlib.h>
#include <stdbool.h>

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

Particle *particle_list_iterator_advance(ParticleIterator *iterator) {
    ParticleListIteratorState *list_iter = (ParticleListIteratorState *) iterator->state;
    ParticleList *list = list_iter->list;
    if (list_iter->current_idx >= list->buffer_len) {
        return NULL;
    }

    Particle *particle = &list->buffer[list_iter->current_idx];
    list_iter->current_idx++;
    return particle;
}

ParticleIterator particle_list_iterator_copy(ParticleIterator *iterator) {
    ParticleListIteratorState *list_iter = (ParticleListIteratorState *) iterator->state;
    return particle_list_iterate(list_iter->list);
}

void particle_list_iterator_reset(ParticleIterator *iterator) {
    ParticleListIteratorState *list_iter = (ParticleListIteratorState *) iterator->state;
    list_iter->current_idx = 0;
}

ParticleIterator particle_list_iterate(ParticleList *particle_list) {
    ParticleListIteratorState *iterator_state = (ParticleListIteratorState *)
        malloc(sizeof(ParticleListIteratorState));
    iterator_state->current_idx = 0;
    iterator_state->list = particle_list;

    ParticleIterator iterator;
    iterator.state = iterator_state;
    iterator.advance = particle_list_iterator_advance;
    iterator.copy = particle_list_iterator_copy;
    iterator.reset = particle_list_iterator_reset;
    return iterator;
}

void particle_list_delete(ParticleList *particle_list) {
    free(particle_list->buffer);
}
