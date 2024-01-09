#ifndef PARTICLE_GRID_CELL_H
#define PARTICLE_GRID_CELL_H

#include "../particle.h"

#include <stdint.h>

#ifndef GRID_CELL_INITIAL_CAP
#define GRID_CELL_INITIAL_CAP 4
#endif /* GRID_CELL_INITIAL_CAP */

typedef uint16_t ParticleGridCellIdx;

typedef struct {
    ParticleGridCellIdx *indices;
    size_t buffer_len, buffer_cap;
} ParticleGridCell;

ParticleGridCell particle_grid_cell_new();
void particle_grid_cell_push(ParticleGridCell *grid_cell, ParticleGridCellIdx idx);
void particle_grid_cell_clear(ParticleGridCell *grid_cell);
void particle_grid_cell_delete(ParticleGridCell *grid_cell);

#endif /* PARTICLE_GRID_CELL_H */
