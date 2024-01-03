#ifndef CAMERA_ORTHOGRAPHIC_H
#define CAMERA_ORTHOGRAPHIC_H

#include "../../thirdparty/../thirdparty/c_math2d.h"

typedef struct {
    cm2_mat4 projection_matrix;
} OrthoCamera;

OrthoCamera ortho_camera_new(int width, int height);
void ortho_camera_on_window_resize(OrthoCamera *camera, int width, int height);

#endif /* CAMERA_ORTHOGRAPHIC_H */
