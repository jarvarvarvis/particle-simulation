#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec4 position_and_radius;
layout (location = 2) in vec4 color;

uniform mat4 _MProj;

out vec2 frag_pos;
out vec4 circle_color;

void main() {
    vec3 pos = position_and_radius.xyz;
    float scale = position_and_radius.w;
    mat4 transform = mat4(
        vec4(scale, 0.0,   0.0,   0.0),
        vec4(0.0,   scale, 0.0,   0.0),
        vec4(0.0,   0.0,   scale, 0.0),
        vec4(pos.x, pos.y, pos.z, 1.0)
    );

    gl_Position = _MProj * transform * vec4(v_pos, 1.0);
    frag_pos = v_pos.xy;
    circle_color = color;
}
