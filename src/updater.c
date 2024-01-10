#include "updater.h"

#include "util/math.h"

#include "../thirdparty/c_log.h"

ParticleEmitter particle_emitter_new(size_t particle_count, cm2_vec2 spawn_position, cm2_vec2 spawn_velocity, float spawn_radius) {
    ParticleEmitter emitter;
    emitter.particles_left_to_spawn = particle_count;
    emitter.spawn_position = spawn_position;
    emitter.spawn_velocity = spawn_velocity;
    emitter.spawn_radius = spawn_radius;
    return emitter;
}

void particle_emitter_spawn_random_colored(ParticleEmitter *emitter, ParticleList *list) {
    // Create randomly colored particle
    Particle particle = particle_new(
        emitter->spawn_position.x, emitter->spawn_position.y,
        emitter->spawn_radius,
        randf(), randf(), randf(), 1.0
    );

    // Add velocity to particle
    particle.position = cm2_vec2_add(particle.position, emitter->spawn_velocity);

    // Push the particle to the list
    particle_list_push(list, particle);
}

ParticleUpdater particle_updater_new(size_t emitter_count) {
    ParticleUpdater updater;

    // Allocate emitter list
    updater.emitter_count = emitter_count;
    updater.emitters =
        (ParticleEmitter*) malloc(sizeof(ParticleEmitter) * emitter_count);

    // Initialize timer
    clock_gettime(CLOCK_REALTIME, &updater.particle_spawn_timer);

    return updater;
}

void particle_updater_update(ParticleUpdater *updater) {
    ParticleList *particle_list = &updater->particle_list;

    // Measure elapsed milliseconds since last particle spawn iteration
    struct timespec current_timer;
    clock_gettime(CLOCK_REALTIME, &current_timer);
    float elapsed_millis = time_diff_ms(updater->particle_spawn_timer, current_timer);

    // Spawn particles for each emitter if enough time has passed
    if (elapsed_millis > updater->particle_spawn_time_interval) {
        for (size_t i = 0; i < updater->emitter_count; ++i) {
            ParticleEmitter *emitter = &updater->emitters[i];

            // If there are particles left to spawn, do so
            if (emitter->particles_left_to_spawn > 0) {
                particle_emitter_spawn_random_colored(emitter, particle_list);
                emitter->particles_left_to_spawn--;
            }
        }

        updater->particle_spawn_timer = current_timer;
    }

    // Update the solver
    solver_update(&updater->solver);
}

void particle_updater_delete(ParticleUpdater *updater) {
    free(updater->emitters);

    particle_grid_delete(&updater->particle_grid);
    particle_list_delete(&updater->particle_list);
    solver_delete(&updater->solver);
}
