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

    // Read shaders from file
    char *vert_source = io_read_file("shaders/particle.vert");
    Shader vert_shader = shader_new(GL_VERTEX_SHADER);
    shader_compile_source(&vert_shader, vert_source);

    c_log(C_LOG_SEVERITY_DEBUG, "Vertex shader source:\n%s", vert_source);

    char *frag_source = io_read_file("shaders/particle.frag");
    Shader frag_shader = shader_new(GL_FRAGMENT_SHADER);
    shader_compile_source(&frag_shader, frag_source);

    c_log(C_LOG_SEVERITY_DEBUG, "Fragment shader source:\n%s", frag_source);

    // Create shader program
    ShaderProgram shader_program = shader_program_new();
    shader_program_attach(&shader_program, &vert_shader);
    shader_program_attach(&shader_program, &frag_shader);
    shader_program_link(&shader_program);
    renderer.shader_program = shader_program;

    return renderer;
}

void particle_renderer_delete(ParticleRenderer *particle_renderer) {
    particle_mesh_delete(&particle_renderer->particle_mesh);
    shader_program_delete(&particle_renderer->shader_program);
}
