#include "grid_renderer.h"

GridRenderer grid_renderer_new() {
    GridRenderer renderer;
    renderer.vao = vao_new();
    renderer.vbo = buffer_new(GL_ARRAY_BUFFER);
    renderer.ebo = buffer_new(GL_ELEMENT_ARRAY_BUFFER);

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

    vao_bind(&renderer.vao);

    // Upload data to vertex buffer object
    buffer_bind(&renderer.vbo);
    buffer_upload_data_static(&renderer.vbo, vertices, sizeof(vertices));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    // Upload data to element buffer object
    buffer_bind(&renderer.ebo);
    buffer_upload_data_static(&renderer.ebo, indices, sizeof(indices));

    // Load shader
    renderer.shader_program = shader_program_load_from_file("shaders/grid.vert", "shaders/grid.frag");

    return renderer;
}

GridRenderer grid_renderer_from_particle_grid(ParticleGrid *grid) {
    GridRenderer grid_renderer = grid_renderer_new();
    grid_renderer.grid_width = (float)grid->width;
    grid_renderer.grid_height = (float)grid->height;
    grid_renderer.cell_width = (float)grid->cell_width;
    grid_renderer.cell_height = (float)grid->cell_height;

    return grid_renderer;
}

void grid_renderer_draw(GridRenderer *grid_renderer) {
    buffer_bind(&grid_renderer->ebo);
    vao_bind(&grid_renderer->vao);
    shader_program_use(&grid_renderer->shader_program);


    cm2_vec4 grid_scale = cm2_vec4_new(
        grid_renderer->grid_width, grid_renderer->grid_height,
        grid_renderer->cell_width, grid_renderer->cell_height
    );
    shader_program_set_vec4(&grid_renderer->shader_program, "grid_scale", grid_scale);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}

void grid_renderer_delete(GridRenderer *grid_renderer) {
    buffer_delete(&grid_renderer->ebo);
    buffer_delete(&grid_renderer->vbo);
    vao_delete(&grid_renderer->vao);
    shader_program_delete(&grid_renderer->shader_program);
}
