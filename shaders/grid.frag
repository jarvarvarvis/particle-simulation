#version 330 core

in vec2 frag_pos;

uniform vec4 grid_scale;

out vec4 frag_color;

void main() {
    vec2 grid_size = grid_scale.xy;
    vec2 cell_size = grid_scale.zw;

    const float FACE = 0.07;
    const float EDGE = 0.15;
    vec2 size = 1.0 / grid_size;   // size of the tile
    vec2 edge = size/8.0;         // size of the edge

    vec2 uv = (frag_pos + 1.0) * 0.5 + (edge / 2.); // Offset by edge/2. to make grid border a little tidier
    uv = sign(vec2(edge) - mod(uv, size));
    frag_color = vec4(FACE - sign(uv.x + uv.y + 2.0) * (FACE - EDGE));
}
