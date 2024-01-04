#include "grid_cell.h"

#include <stdlib.h>

ParticleGridCell particle_grid_cell_new() {
    ParticleGridCell grid_cell;
    grid_cell.buffer_len = 0;
    grid_cell.buffer_cap = GRID_CELL_INITIAL_CAP;
    grid_cell.buffer = (Particle **) malloc(sizeof(Particle*) * grid_cell.buffer_cap);

    // Set pointers to NULL
    for (size_t i = 0; i < grid_cell.buffer_cap; ++i) {
        grid_cell.buffer[i] = NULL;
    }

    return grid_cell;
}

void particle_grid_cell_push(ParticleGridCell *grid_cell, Particle *particle) {
    if (grid_cell->buffer_len >= grid_cell->buffer_cap) {
        // Reallocate the particle buffer
        grid_cell->buffer_cap *= 2; // Let it grow (exponentially)
        grid_cell->buffer = (Particle **)
            realloc(grid_cell->buffer, sizeof(Particle*) * grid_cell->buffer_cap);
    }

    // Write the particle at the last free position and increment length
    grid_cell->buffer[grid_cell->buffer_len] = particle;
    grid_cell->buffer_len++;
}

void particle_grid_cell_clear(ParticleGridCell *grid_cell) {
    for (size_t i = 0; i < grid_cell->buffer_len; ++i) {
        grid_cell->buffer[i] = NULL;
    }

    // Reset grid buffer length
    grid_cell->buffer_len = 0;
}

void particle_grid_cell_delete(ParticleGridCell *grid_cell) {
    free(grid_cell->buffer);
}
