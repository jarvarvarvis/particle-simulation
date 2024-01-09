#include "grid.h"
#include "grid_cell.h"

#include <stdio.h>
#include <stdlib.h>

ParticleGrid particle_grid_new(size_t width, size_t height, float cell_width, float cell_height) {
    ParticleGrid grid;
    grid.width = width;
    grid.height = height;
    grid.cell_width = cell_width;
    grid.cell_height = cell_height;
    grid.cells = (ParticleGridCell*) malloc(sizeof(ParticleGridCell) * (width * height));

    for (size_t i = 0; i < width * height; ++i) {
        grid.cells[i] = particle_grid_cell_new();
    }

    return grid;
}

bool particle_grid_is_particle_inside_grid(ParticleGrid *grid, Particle *particle) {
    // The grid has the dimensions (width * cell_width, height * cell_height) in world-space.
    // Since the camera and therefore the grid is centered around (0,0), the valid coordinate
    // ranges for the particle are as follows:
    // - X coordinate: -width  * cell_width  / 2 .. width  * cell_width  / 2
    // - Y coordinate: -height * cell_height / 2 .. height * cell_height / 2
    float half_world_width = (grid->width * grid->cell_width) / 2.;
    float half_world_height = (grid->height * grid->cell_height) / 2.;

    float particle_x = particle->position.x;
    float particle_y = particle->position.y;

    bool inside_grid_x = (particle_x > -half_world_width) && (particle_x < half_world_width);
    bool inside_grid_y = (particle_y > -half_world_height) && (particle_y < half_world_height);
    return inside_grid_x && inside_grid_y;
}

bool particle_grid_index_from_position(
    ParticleGrid *grid,
    Particle *particle,
    size_t *cell_x, size_t *cell_y
) {
    if (!particle_grid_is_particle_inside_grid(grid, particle)) {
        return false;
    }

    float half_world_width = (grid->width * grid->cell_width) / 2.;
    float half_world_height = (grid->height * grid->cell_height) / 2.;

    float particle_x = particle->position.x;
    float particle_y = -particle->position.y;

    // To convert the particle position to a valid index, we first have to convert each
    // coordinate to a valid range:
    // For x, this is (-width/2, width/2) and for y, this is (height/2, -height/2).
    // For that, we simply add half of the width / height to the x / y component.
    //
    // Since the actual size of the grid in the world is (width * cell_width, height * cell_height),
    // we then also divide by the cell_width / cell_height to get the proper range.
    // Calculating the index is then simply a matter of taking the floor (important!)
    // and casting to size_t.
    *cell_x = (size_t) floorf((particle_x + half_world_width) / grid->cell_width);
    *cell_y = (size_t) floorf((particle_y + half_world_height) / grid->cell_height);

    return true;
}

bool particle_grid_is_position_inside_grid(ParticleGrid *grid, size_t cell_x, size_t cell_y) {
    return cell_x < grid->width && cell_y < grid->height;
}

ParticleGridCell *particle_grid_cell_at(ParticleGrid *grid, size_t cell_x, size_t cell_y) {
    if (!particle_grid_is_position_inside_grid(grid, cell_x, cell_y)) {
        return NULL;
    }

    return &grid->cells[cell_y * grid->width + cell_x];
}

bool particle_grid_insert_index_for_particle(ParticleGrid *grid, Particle *particle, ParticleGridCellIdx idx) {
    size_t cell_x, cell_y;
    if (!particle_grid_index_from_position(grid, particle, &cell_x, &cell_y)) {
        return false;
    }

    ParticleGridCell *cell = particle_grid_cell_at(grid, cell_x, cell_y);
    particle_grid_cell_push(cell, idx);

    return true;
}

void particle_grid_insert_all(ParticleGrid *grid, ParticleList *list) {
    for (ParticleGridCellIdx idx = 0; idx < list->buffer_len; ++idx) {
        Particle *particle = &list->buffer[idx];
        particle_grid_insert_index_for_particle(grid, particle, idx);
    }
}

void particle_grid_clear(ParticleGrid *grid) {
    for (size_t i = 0; i < grid->width * grid->height; ++i) {
        ParticleGridCell *cell = &grid->cells[i];
        particle_grid_cell_clear(cell);
    }
}

void particle_grid_print_basic(ParticleGrid *grid) {
    for (size_t y = 0; y < grid->height; ++y) {
        for (size_t x = 0; x < grid->width; ++x) {
            ParticleGridCell *cell = &grid->cells[y * grid->width + x];
            if (cell->indices_len == 0) {
                printf("[  ] ");
            } else {
                printf("[%02lu] ", cell->indices_len);
            }
        }
        printf("\n");
    }
}

void particle_grid_print_with_first_particle_pos(ParticleGrid *grid, ParticleList *list) {
    for (size_t y = 0; y < grid->height; ++y) {
        for (size_t x = 0; x < grid->width; ++x) {
            ParticleGridCell *cell = &grid->cells[y * grid->width + x];
            Particle *particle = NULL;
            if (cell->indices_len > 0) {
                ParticleGridCellIdx idx = cell->indices[0];
                particle = &list->buffer[idx];
            }

            if (particle != NULL) {
                printf("[%8.2f,%8.2f] ", particle->position.x, particle->position.y);
            } else {
                printf("[        ,        ] ");
            }
        }
        printf("\n");
    }
}

void particle_grid_delete(ParticleGrid *grid) {
    for (size_t i = 0; i < grid->width * grid->height; ++i) {
        ParticleGridCell *cell = &grid->cells[i];
        particle_grid_cell_delete(cell);
    }

    free(grid->cells);
}
