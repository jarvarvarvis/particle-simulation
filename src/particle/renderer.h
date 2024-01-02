#ifndef PARTICLE_RENDERER_H
#define PARTICLE_RENDERER_H

#include "../opengl/buffer.h"
#include "../opengl/vao.h"
#include "../opengl/shader.h"

typedef struct {
    Vao vao;
    Buffer vbo, ebo;
} ParticleMesh;

ParticleMesh particle_mesh_new();
void particle_mesh_delete(ParticleMesh *particle_mesh);


typedef struct {
    ParticleMesh particle_mesh;
    ShaderProgram shader_program;
} ParticleRenderer;

ParticleRenderer particle_renderer_new();
void particle_renderer_delete(ParticleRenderer *particle_renderer);

#endif /* PARTICLE_RENDERER_H */
