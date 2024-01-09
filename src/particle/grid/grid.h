#ifndef PARTICLE_GRID_H
#define PARTICLE_GRID_H

#include "grid_cell.h"
#include "../list.h"
#include "../particle.h"

#include <stdbool.h>

typedef struct {
    ParticleGridCell *cells;
    size_t width, height;
    float cell_width, cell_height;
} ParticleGrid;

ParticleGrid particle_grid_new(size_t width, size_t height, float cell_width, float cell_height);
bool particle_grid_is_particle_inside_grid(ParticleGrid *grid, Particle *particle);
bool particle_grid_index_from_position(
    ParticleGrid *grid,
    Particle *particle,
    size_t *cell_x, size_t *cell_y
);
bool particle_grid_is_position_inside_grid(ParticleGrid *grid, size_t cell_x, size_t cell_y);
ParticleGridCell *particle_grid_cell_at(ParticleGrid *grid, size_t cell_x, size_t cell_y);
bool particle_grid_insert_index_for_particle(ParticleGrid *grid, Particle *particle, ParticleGridCellIdx idx);
void particle_grid_insert_all(ParticleGrid *grid, ParticleList *list);
void particle_grid_clear(ParticleGrid *grid);
void particle_grid_print_basic(ParticleGrid *grid);
void particle_grid_print_with_first_particle_pos(ParticleGrid *grid, ParticleList *list);
void particle_grid_delete(ParticleGrid *grid);

#endif /* PARTICLE_GRID_H */
