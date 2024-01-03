#ifndef GRID_CELL_H
#define GRID_CELL_H

#include <stdlib.h>
#include <stdbool.h>


#ifndef PARTICLE_GRID_CELL_INITIAL_CAP
#define PARTICLE_GRID_CELL_INITIAL_CAP 16
#endif /* PARTICLE_LIST_INITIAL_CAP */

#define PARTICLE_GRID_CELL_EMPTY -1

typedef struct {
    long *indices;
    size_t indices_len, indices_cap;
} ParticleGridCell;

ParticleGridCell particle_grid_cell_new();
void particle_grid_cell_push(ParticleGridCell *cell, long index);
bool particle_grid_cell_find(ParticleGridCell *cell, long index, size_t *found_at_index);
void particle_grid_cell_remove(ParticleGridCell *cell, long index);
void particle_grid_cell_delete(ParticleGridCell *cell);

#endif /* GRID_H */
