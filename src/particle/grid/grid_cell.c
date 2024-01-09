#include "grid_cell.h"

#include <stdlib.h>

ParticleGridCell particle_grid_cell_new() {
    ParticleGridCell grid_cell;
    grid_cell.indices_len = 0;
    grid_cell.indices_cap = GRID_CELL_INITIAL_CAP;
    grid_cell.indices = (ParticleGridCellIdx*) malloc(sizeof(ParticleGridCellIdx) * grid_cell.indices_cap);

    // Set indices to 0
    for (size_t i = 0; i < grid_cell.indices_cap; ++i) {
        grid_cell.indices[i] = 0;
    }

    return grid_cell;
}

void particle_grid_cell_push(ParticleGridCell *grid_cell, ParticleGridCellIdx idx) {
    if (grid_cell->indices_len >= grid_cell->indices_cap) {
        // Reallocate the particle buffer
        grid_cell->indices_cap *= 2; // Let it grow (exponentially)
        grid_cell->indices = (ParticleGridCellIdx*)
            realloc(grid_cell->indices, sizeof(ParticleGridCellIdx) * grid_cell->indices_cap);
    }

    // Write the particle at the last free position and increment length
    grid_cell->indices[grid_cell->indices_len] = idx;
    grid_cell->indices_len++;
}

void particle_grid_cell_clear(ParticleGridCell *grid_cell) {
    // Set indices to 0
    for (size_t i = 0; i < grid_cell->indices_len; ++i) {
        grid_cell->indices[i] = 0;
    }

    // Reset grid buffer length
    grid_cell->indices_len = 0;
}

void particle_grid_cell_delete(ParticleGridCell *grid_cell) {
    free(grid_cell->indices);
}
