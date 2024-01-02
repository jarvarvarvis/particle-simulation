#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in mat4 transform;
layout (location = 5) in vec4 color;

uniform mat4 projection;

out vec2 pos;
out vec4 circle_color;

void main() {
    gl_Position = projection * transform * vec4(v_pos, 1.0);
    circle_color = color;
    pos = v_pos.xy;
}
