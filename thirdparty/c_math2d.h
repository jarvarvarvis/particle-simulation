/*
MIT License

Copyright (c) 2024 jarvarvarvis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _C_MATH2D_HEADER
#define _C_MATH2D_HEADER

#include <string.h>
#include <math.h>

typedef struct { float x, y; } cm2_vec2;
typedef struct { float x, y, z; } cm2_vec3;
typedef struct { float x, y, z, w; } cm2_vec4;

typedef float cm2_mat4[4][4];

static const cm2_mat4 CM2_MAT4_IDENTITY = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1},
};

#ifndef C_MATH2D_DEFINITION

cm2_vec2 cm2_vec2_new(float x, float y);
cm2_vec2 cm2_vec2_dot(cm2_vec2 first, cm2_vec2 second);
float cm2_vec2_length(cm2_vec2 vec);
cm2_vec2 cm2_vec2_additive_inv(cm2_vec2 vec);
cm2_vec2 cm2_vec2_add(cm2_vec2 first, cm2_vec2 second);
cm2_vec2 cm2_vec2_sub(cm2_vec2 first, cm2_vec2 second);
cm2_vec2 cm2_vec2_scale(cm2_vec2 vec, float scalar);

cm2_vec4 cm2_vec4_new(float x, float y, float z, float w);

void cm2_mat4_create_identity(cm2_mat4 out);
void cm2_mat4_create_orthographic(
    cm2_mat4 out,
    float left, float right,
    float bottom, float top,
    float near, float far
);
float *cm2_mat4_value_ptr(cm2_mat4 mat);

#else

cm2_vec2 cm2_vec2_new(float x, float y) {
    cm2_vec2 v;
    v.x = x;
    v.y = y;
    return v;
}

float cm2_vec2_dot(cm2_vec2 first, cm2_vec2 second) {
    return first.x * second.x + first.y * second.y;
}

float cm2_vec2_length(cm2_vec2 vec) {
    return sqrtf(cm2_vec2_dot(vec, vec));
}

cm2_vec2 cm2_vec2_additive_inv(cm2_vec2 vec) {
    cm2_vec2 inv_vec;
    inv_vec.x = -vec.x;
    inv_vec.y = -vec.y;
    return inv_vec;
}

cm2_vec2 cm2_vec2_add(cm2_vec2 first, cm2_vec2 second) {
    cm2_vec2 v;
    v.x = first.x + second.x;
    v.y = first.y + second.y;
    return v;
}

cm2_vec2 cm2_vec2_sub(cm2_vec2 first, cm2_vec2 second) {
    return cm2_vec2_add(first, cm2_vec2_additive_inv(second));
}

cm2_vec2 cm2_vec2_scale(cm2_vec2 vec, float scalar) {
    cm2_vec2 v;
    v.x = vec.x * scalar;
    v.y = vec.y * scalar;
    return v;
}


cm2_vec4 cm2_vec4_new(float x, float y, float z, float w) {
    cm2_vec4 v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    return v;
}


void cm2_mat4_create_identity(cm2_mat4 out) {
    memcpy(out, CM2_MAT4_IDENTITY, 16 * sizeof(float));
}

void cm2_mat4_create_orthographic(
    cm2_mat4 out,
    float left, float right,
    float bottom, float top,
    float near, float far
) {
    cm2_mat4_create_identity(out);

    out[0][0] =  2 / (right - left);
    out[1][1] =  2 / (top - bottom);
    out[2][2] = -2 / (far - near);

    out[0][3] = -(right + left) / (right - left);
    out[1][3] = -(top + bottom) / (top - bottom);
    out[2][3] = -(far + near)   / (far - near);
}

float *cm2_mat4_value_ptr(cm2_mat4 mat) {
    return &mat[0][0];
}

#endif /* C_MATH2D_DEFINITION */

#endif /* _C_MATH2D_HEADER */
