#version 330 core

in vec2 frag_pos;
in vec4 circle_color;

out vec4 frag_color;

void main() {
    float rsq = dot(frag_pos, frag_pos);
    if (rsq > 1.0)
        discard;
    frag_color = min(1.0, 1.0 - rsq + 0.35) * circle_color;
}
