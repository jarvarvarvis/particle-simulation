#ifndef PARTICLE_H
#define PARTICLE_H

#include "../../thirdparty/c_math2d.h"

typedef struct {
    cm2_vec2 position;
    cm2_vec2 last_position;
    cm2_vec2 acceleration;
    float radius;

    cm2_vec4 color;
} Particle;

Particle particle_new(float x, float y, float radius, float r, float g, float b, float a);
void particle_update_position(Particle *particle, float dt);
void particle_accelerate(Particle *particle, cm2_vec2 acc);

#endif /* PARTICLE_H */
