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

float *cm2_mat4_value_ptr(cm2_mat4 mat);
void cm2_mat4_create_identity(cm2_mat4 mat);
void cm2_mat4_create_orthographic(
    cm2_mat4 mat,
    float left, float right,
    float bottom, float top,
    float near, float far
);

#else

float *cm2_mat4_value_ptr(cm2_mat4 mat) {
    return &mat[0][0];
}

void cm2_mat4_create_identity(cm2_mat4 mat) {
    memcpy(mat, CM2_MAT4_IDENTITY, 16 * sizeof(float));
}

void cm2_mat4_create_orthographic(
    cm2_mat4 mat,
    float left, float right,
    float bottom, float top,
    float near, float far
) {
    cm2_mat4_create_identity(mat);

    mat[0][0] =  2 / (right - left);
    mat[1][1] =  2 / (top - bottom);
    mat[2][2] = -2 / (far - near);

    mat[0][3] = -(right + left) / (right - left);
    mat[1][3] = -(top + bottom) / (top - bottom);
    mat[2][3] = -(far + near)   / (far - near);
}

#endif /* C_MATH2D_DEFINITION */

#endif /* _C_MATH2D_HEADER */
