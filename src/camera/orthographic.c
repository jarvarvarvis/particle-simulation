#include "orthographic.h"

OrthoCamera ortho_camera_new(int width, int height) {
    float half_width = (float)width / 2.0;
    float half_height = (float)height / 2.0;

    OrthoCamera camera;
    cm2_mat4_create_orthographic(camera.projection_matrix,
            -half_width, half_width,
            -half_height, half_height,
            -100.0, 100.0);

    return camera;
}

void ortho_camera_on_window_resize(OrthoCamera *camera, int width, int height) {
    float half_width = (float)width / 2.0;
    float half_height = (float)height / 2.0;

    cm2_mat4_create_orthographic(camera->projection_matrix,
            -half_width, half_width,
            -half_height, half_height,
            -100.0, 100.0);
}
