#ifndef GRID_H
#define GRID_H

#include "grid_cell.h"
#include "particle.h"

typedef struct {
    ParticleGridCell *cells;
    size_t width, height;
    float cell_width, cell_height;
} ParticleGrid;

ParticleGrid particle_grid_new(size_t width, size_t height, float cell_width, float cell_height);
ParticleGridCell *particle_grid_get_cell(ParticleGrid *grid, size_t x, size_t y);
bool particle_grid_is_particle_inside(ParticleGrid *grid, Particle *particle);
bool particle_grid_get_particle_grid_indices(
    ParticleGrid *grid,
    Particle *particle,
    size_t *cell_x,
    size_t *cell_y
);
ParticleGridCell *particle_grid_get_cell_of(ParticleGrid *grid, Particle *particle);
void particle_grid_insert(ParticleGrid *grid, size_t x, size_t y, long index);
void particle_grid_insert_particle(ParticleGrid *grid, Particle *particle, long index);
void particle_grid_update_moved_particle(ParticleGrid *grid, Particle *particle, long index);
void particle_grid_print(ParticleGrid *grid);
void particle_grid_print_full(ParticleGrid *grid);
void particle_grid_delete(ParticleGrid *grid);

#endif /* GRID_H */
