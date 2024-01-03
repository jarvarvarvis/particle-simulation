#ifndef PARTICLE_ITERATOR_H
#define PARTICLE_ITERATOR_H

#include "particle.h"

struct ParticleIterator;

typedef Particle* (*ParticleIteratorAdvanceFn)(struct ParticleIterator *iterator);
typedef void (*ParticleIteratorResetFn)(struct ParticleIterator *iterator);
typedef struct ParticleIterator (*ParticleIteratorCopyFn)(struct ParticleIterator *iterator);

struct ParticleIterator {
    void *state;
    ParticleIteratorAdvanceFn advance;
    ParticleIteratorCopyFn copy;
    ParticleIteratorResetFn reset;
};

typedef struct ParticleIterator ParticleIterator;

void particle_iterator_delete(ParticleIterator *iterator);

#endif /* PARTICLE_ITERATOR_H */
