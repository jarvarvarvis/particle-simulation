#ifndef GRID_H
#define GRID_H

#include "grid_cell.h"

typedef struct {
    ParticleGridCell *cells;
    size_t width, height;
} ParticleGrid;

ParticleGrid particle_grid_new(size_t width, size_t height);
ParticleGridCell *particle_grid_get_cell(ParticleGrid *grid, size_t x, size_t y);
void particle_grid_insert(ParticleGrid *grid, size_t x, size_t y, long index);
void particle_grid_print(ParticleGrid *grid);
void particle_grid_delete(ParticleGrid *grid);

#endif /* GRID_H */
