#include "math4.h"

#include <math.h>

Vec3 vec3_add(Vec3 a, Vec3 b) { return (Vec3){ a.x + b.x, a.y + b.y, a.z + b.z }; }
Vec3 vec3_sub(Vec3 a, Vec3 b) { return (Vec3){ a.x - b.x, a.y - b.y, a.z - b.z }; }
Vec3 vec3_scale(Vec3 v, float s) { return (Vec3){ v.x * s, v.y * s, v.z * s }; }
float vec3_dot(Vec3 a, Vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
Vec3 vec3_cross(Vec3 a, Vec3 b) { return (Vec3){ a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x }; }
float vec3_len(Vec3 v) { return sqrtf(vec3_dot(v, v)); }
Vec3 vec3_norm(Vec3 v) {
    float l = vec3_len(v);
    if (l < 1e-8f) return (Vec3){ 0 };
    return vec3_scale(v, 1.0f / l);
}

Mat4 mat4_identity(void) {
    Mat4 m = { 0 };
    m.m[0] = 1.0f;
    m.m[5] = 1.0f;
    m.m[10] = 1.0f;
    m.m[15] = 1.0f;
    return m;
}

Mat4 mat4_mul(Mat4 a, Mat4 b) {
    Mat4 r = { 0 };
    for (int c = 0; c < 4; c++) {
        for (int r0 = 0; r0 < 4; r0++) {
            r.m[c * 4 + r0] =
                a.m[0 * 4 + r0] * b.m[c * 4 + 0] +
                a.m[1 * 4 + r0] * b.m[c * 4 + 1] +
                a.m[2 * 4 + r0] * b.m[c * 4 + 2] +
                a.m[3 * 4 + r0] * b.m[c * 4 + 3];
        }
    }
    return r;
}

Mat4 mat4_perspective(float fov_y_radians, float aspect, float z_near, float z_far) {
    float f = 1.0f / tanf(fov_y_radians * 0.5f);
    Mat4 m = { 0 };
    m.m[0] = f / aspect;
    m.m[5] = f;
    m.m[10] = (z_far + z_near) / (z_near - z_far);
    m.m[11] = -1.0f;
    m.m[14] = (2.0f * z_far * z_near) / (z_near - z_far);
    return m;
}

Mat4 mat4_look(Vec3 eye, Vec3 forward, Vec3 up) {
    Vec3 f = vec3_norm(forward);
    Vec3 s = vec3_norm(vec3_cross(f, up));
    Vec3 u = vec3_cross(s, f);

    Mat4 m = mat4_identity();

    m.m[0] = s.x;
    m.m[1] = u.x;
    m.m[2] = -f.x;

    m.m[4] = s.y;
    m.m[5] = u.y;
    m.m[6] = -f.y;

    m.m[8] = s.z;
    m.m[9] = u.z;
    m.m[10] = -f.z;

    m.m[12] = -vec3_dot(s, eye);
    m.m[13] = -vec3_dot(u, eye);
    m.m[14] = vec3_dot(f, eye);

    return m;
}

