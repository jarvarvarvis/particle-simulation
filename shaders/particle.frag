#version 330 core

in vec2 pos;
in vec4 circle_color;

out vec4 frag_color;

void main() {
    float rsq = dot(pos, pos);
    if (rsq > 1)
        discard;
    frag_color = circle_color;
}
