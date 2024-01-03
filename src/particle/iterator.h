#ifndef PARTICLE_ITERATOR_H
#define PARTICLE_ITERATOR_H

#include "particle.h"

struct ParticleIterator;

typedef Particle* (*ParticleIteratorAdvancer)(struct ParticleIterator *iterator);

struct ParticleIterator {
    Particle *curr;
    void *state;
    ParticleIteratorAdvancer advance;
};

typedef struct ParticleIterator ParticleIterator;

void particle_iterator_delete(ParticleIterator *iterator);

#endif /* PARTICLE_ITERATOR_H */
