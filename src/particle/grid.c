#include "grid.h"

#include <stdio.h>

#include "../../thirdparty/c_log.h"

ParticleGrid particle_grid_new(size_t width, size_t height, float cell_width, float cell_height) {
    ParticleGrid grid;
    grid.width = width;
    grid.height = height;
    grid.cell_width = cell_width;
    grid.cell_height = cell_height;
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

bool particle_grid_is_particle_inside(ParticleGrid *grid, Particle *particle) {
    // NOTE:
    // The total area that the grid is taking up in the world is equal to
    // (width * cell_width) * (height * cell_height)
    float grid_border_x = (grid->width * grid->cell_width) / 2.0;
    float grid_border_y = (grid->height * grid->cell_height) / 2.0;

    // Since the grid is centered around (0.0, 0.0), we have to check...
    // - that the X coordinate is in the range [-grid_border_x, grid_border_x] and
    // - that the Y coordinate is in the range [-grid_border_y, grid_border_y]

    // Range check - X coordinate
    if (particle->position.x < -grid_border_x || particle->position.x > grid_border_x) {
        return false;
    }

    // Range check - Y coordinate
    if (particle->position.y < -grid_border_y || particle->position.y > grid_border_y) {
        return false;
    }

    return true;
}

bool particle_grid_get_particle_grid_indices(
    ParticleGrid *grid,
    Particle *particle,
    size_t *cell_x,
    size_t *cell_y
) {
    if (!particle_grid_is_particle_inside(grid, particle)) {
        return false;
    }

    float grid_border_x = (grid->width * grid->cell_width) / 2.0;
    float grid_border_y = (grid->height * grid->cell_height) / 2.0;

    // Ranges for particle position components:
    // X: [-width/2, width/2]
    // Y: [-height/2, height/2]

    // Calculate the actual grid indices
    *cell_x = (size_t) floorf((particle->position.x + grid_border_x) / grid->cell_width);
    *cell_y = (size_t) floorf((particle->position.y + grid_border_y) / grid->cell_height);

    return true;
}

void particle_grid_insert(ParticleGrid *grid, size_t x, size_t y, long index) {
    ParticleGridCell *cell = particle_grid_get_cell(grid, x, y);
    if (cell == NULL) {
        return;
    }

    particle_grid_cell_push(cell, index);
}

void particle_grid_insert_particle(ParticleGrid *grid, Particle *particle, long index) {
    size_t cell_x, cell_y;
    if (!particle_grid_get_particle_grid_indices(grid, particle, &cell_x, &cell_y)) {
        return;
    }

    particle_grid_insert(grid, cell_x, cell_y, index);
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

