#include "camera.h"

#include "math4.h"

#include <math.h>

void camera_init(Camera* cam) {
    cam->position_feet = (Vec3){ 8.0f, 6.0f, 8.0f };
    cam->velocity = (Vec3){ 0 };
    cam->yaw = 0.0f;
    cam->pitch = 0.0f;
    cam->on_ground = false;
}

static float clampf(float v, float a, float b) {
    if (v < a) return a;
    if (v > b) return b;
    return v;
}

Vec3 camera_forward_xz(const Camera* cam) {
    float cy = cosf(cam->yaw);
    float sy = sinf(cam->yaw);
    Vec3 f = (Vec3){ sy, 0.0f, -cy };
    return vec3_norm(f);
}

Vec3 camera_right_xz(const Camera* cam) {
    Vec3 f = camera_forward_xz(cam);
    Vec3 r = (Vec3){ -f.z, 0.0f, f.x };
    return vec3_norm(r);
}

void camera_apply_mouse(Camera* cam, int mouse_dx, int mouse_dy, float sensitivity) {
    cam->yaw += (float)mouse_dx * sensitivity;
    cam->pitch -= (float)mouse_dy * sensitivity;
    cam->pitch = clampf(cam->pitch, -1.55f, 1.55f);
}
