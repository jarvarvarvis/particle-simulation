#version 330 core

layout (location = 0) in vec3 v_pos;

uniform vec4 grid_scale;
uniform mat4 _MProj;

out vec2 frag_pos;

void main() {
    vec2 grid_size = grid_scale.xy / 2.;
    vec2 cell_size = grid_scale.zw;

    gl_Position = _MProj * vec4(v_pos.xy * grid_size * cell_size, 0.0, 1.0);
    frag_pos = v_pos.xy;
}
