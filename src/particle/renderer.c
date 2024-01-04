#include "renderer.h"

#include "../util/io.h"

#include "../../thirdparty/c_log.h"

ParticleMesh particle_mesh_new() {
    ParticleMesh mesh;
    mesh.vao = vao_new();
    mesh.vbo = buffer_new(GL_ARRAY_BUFFER);
    mesh.ebo = buffer_new(GL_ELEMENT_ARRAY_BUFFER);

    float vertices[] = {
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    vao_bind(&mesh.vao);

    // Upload data to vertex buffer object
    buffer_bind(&mesh.vbo);
    buffer_upload_data_static(&mesh.vbo, vertices, sizeof(vertices));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    // Upload data to element buffer object
    buffer_bind(&mesh.ebo);
    buffer_upload_data_static(&mesh.ebo, indices, sizeof(indices));

    return mesh;
}

void particle_mesh_delete(ParticleMesh *particle_mesh) {
    buffer_delete(&particle_mesh->ebo);
    buffer_delete(&particle_mesh->vbo);
    vao_delete(&particle_mesh->vao);
}

ParticleRenderer particle_renderer_new() {
    ParticleRenderer renderer;
    renderer.particle_mesh = particle_mesh_new();
    renderer.shader_program = shader_program_load_from_file("shaders/particle.vert", "shaders/particle.frag");

    // Initialize GPU data
    renderer.gpu_data = particle_gpu_data_new();

    return renderer;
}

void particle_renderer_upload_from_list(ParticleRenderer *particle_renderer, ParticleList *particle_list) {
    particle_list_upload(particle_list, &particle_renderer->gpu_data);
}

void particle_renderer_draw(ParticleRenderer *particle_renderer) {
    particle_gpu_data_bind_buffers(&particle_renderer->gpu_data);

    buffer_bind(&particle_renderer->particle_mesh.ebo);
    vao_bind(&particle_renderer->particle_mesh.vao);
    shader_program_use(&particle_renderer->shader_program);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0,
            particle_renderer->gpu_data.particle_count);
}

void particle_renderer_delete(ParticleRenderer *particle_renderer) {
    particle_mesh_delete(&particle_renderer->particle_mesh);
    shader_program_delete(&particle_renderer->shader_program);
    particle_gpu_data_delete(&particle_renderer->gpu_data);
}
