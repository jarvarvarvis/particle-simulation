#ifndef GRID_RENDERER_H
#define GRID_RENDERER_H

#include "../opengl/buffer.h"
#include "../opengl/vao.h"
#include "../opengl/shader.h"

#include "grid.h"

typedef struct {
    Vao vao;
    Buffer vbo, ebo;
    float grid_width, grid_height;
    float cell_width, cell_height;
    ShaderProgram shader_program;
} GridRenderer;

GridRenderer grid_renderer_new();
GridRenderer grid_renderer_from_particle_grid(ParticleGrid *grid);
void grid_renderer_draw(GridRenderer *grid_renderer);
void grid_renderer_delete(GridRenderer *grid_renderer);

#endif /* GRID_RENDERER_H */
