#include "grid.h"

#include <stdio.h>

ParticleGrid particle_grid_new(size_t width, size_t height) {
    ParticleGrid grid;
    grid.width = width;
    grid.height = height;
    grid.cells = (ParticleGridCell*) malloc(sizeof(ParticleGridCell) * (width * height));

    // Fill cells
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            grid.cells[y * width + x] = particle_grid_cell_new();
        }
    }

    return grid;
}

ParticleGridCell *particle_grid_get_cell(ParticleGrid *grid, size_t x, size_t y) {
    if (x >= grid->width || y >= grid->height) {
        return NULL;
    }

    return &grid->cells[y * grid->width + x];
}

void particle_grid_insert(ParticleGrid *grid, size_t x, size_t y, long index) {
    ParticleGridCell *cell = particle_grid_get_cell(grid, x, y);
    if (cell == NULL) {
        return;
    }

    particle_grid_cell_push(cell, index);
}

void particle_grid_print(ParticleGrid *grid) {
    for (size_t y = 0; y < grid->height; ++y) {
        for (size_t x = 0; x < grid->width; ++x) {
            ParticleGridCell *cell = particle_grid_get_cell(grid, x, y);
            printf("[%02lu] ", cell->indices_len);
        }
        printf("\n");
    }
}

void particle_grid_delete(ParticleGrid *grid) {
    for (size_t y = 0; y < grid->height; ++y) {
        for (size_t x = 0; x < grid->width; ++x) {
            ParticleGridCell *cell = particle_grid_get_cell(grid, x, y);
            particle_grid_cell_delete(cell);
        }
    }

    free(grid->cells);
}

