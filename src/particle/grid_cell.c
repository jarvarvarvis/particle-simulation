#include "grid_cell.h"

#include "../../thirdparty/c_log.h"

ParticleGridCell particle_grid_cell_new() {
    ParticleGridCell cell;
    cell.indices_len = 0;
    cell.indices_cap = PARTICLE_GRID_CELL_INITIAL_CAP;
    cell.indices = (long*) malloc(sizeof(long) * cell.indices_cap);

    // Fill initial cells with the empty value
    for (size_t i = 0; i < cell.indices_cap; ++i) {
        cell.indices[i] = PARTICLE_GRID_CELL_EMPTY;
    }

    return cell;
}

void particle_grid_cell_push(ParticleGridCell *cell, long index) {
    // Reallocate the buffer if the capacity is reached
    if (cell->indices_len >= cell->indices_cap) {
        cell->indices_cap *= 2; // Let it grow (exponentially)
        cell->indices = (long*)
            realloc(cell->indices, sizeof(long) * cell->indices_cap);

        // Fill new cells with the empty value
        for (size_t i = cell->indices_cap / 2; i < cell->indices_cap; ++i) {
            cell->indices[i] = PARTICLE_GRID_CELL_EMPTY;
        }
    }

    // Find the next free index
    bool free_index_found = false;
    size_t next_free_index;
    for (size_t i = 0; i < cell->indices_len; ++i) {
        if (cell->indices[i] == PARTICLE_GRID_CELL_EMPTY) {
            next_free_index = i;
            free_index_found = true;
            break;
        }
    }

    // If no free index was found, insert at the end
    if (!free_index_found) next_free_index = cell->indices_len;
    cell->indices[next_free_index] = index;

    // If the index was added at the end of the list, also increment the length counter
    if (!free_index_found) {
        cell->indices_len++;
    }
}

bool particle_grid_cell_find(ParticleGridCell *cell, long index, size_t *found_at_index) {
    for (size_t i = 0; i < cell->indices_len; ++i) {
        if (cell->indices[i] == index) {
            *found_at_index = i;
            return true;
        }
    }

    return false;
}

void particle_grid_cell_remove(ParticleGridCell *cell, long index) {
    size_t removed_index;
    if (!particle_grid_cell_find(cell, index, &removed_index)) {
        return;
    }

    cell->indices[removed_index] = PARTICLE_GRID_CELL_EMPTY;

    // If the index was removed at the end of the list, decrement the length counter
    if (removed_index == cell->indices_len - 1) {
        cell->indices_len--;
    }
}

void particle_grid_cell_delete(ParticleGridCell *cell) {
    free(cell->indices);
}
