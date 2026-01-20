#pragma once

#include <stdbool.h>

typedef struct Vec3 {
    float x;
    float y;
    float z;
} Vec3;

typedef struct Camera {
    Vec3 position_feet;
    Vec3 velocity;
    float yaw;
    float pitch;
    bool on_ground;
} Camera;

void camera_init(Camera* cam);
Vec3 camera_forward_xz(const Camera* cam);
Vec3 camera_right_xz(const Camera* cam);
void camera_apply_mouse(Camera* cam, int mouse_dx, int mouse_dy, float sensitivity);
