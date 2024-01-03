#include "particle.h"

Particle particle_new(float x, float y, float radius, float r, float g, float b, float a) {
    Particle particle;
    particle.position = cm2_vec2_new(x, y);
    particle.last_position = particle.position;
    particle.acceleration = cm2_vec2_new(0.0, 0.0);
    particle.radius = radius;

    particle.color = cm2_vec4_new(r, g, b, a);
    return particle;
}

void particle_update_position(Particle *particle, float dt) {
    cm2_vec2 velocity = cm2_vec2_sub(particle->position, particle->last_position);
    particle->last_position = particle->position;

    cm2_vec2 offset = cm2_vec2_add(velocity, cm2_vec2_scale(particle->acceleration, dt * dt));
    particle->position = cm2_vec2_add(particle->position, offset);

    particle->acceleration = cm2_vec2_new(0.0, 0.0);
}

void particle_accelerate(Particle *particle, cm2_vec2 acc) {
    particle->acceleration = cm2_vec2_add(particle->acceleration, acc);
}
