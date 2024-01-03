#include "iterator.h"

#include <stdlib.h>

void particle_iterator_delete(ParticleIterator *iterator) {
    free(iterator->state);
}
