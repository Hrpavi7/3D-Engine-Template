#pragma once

#include "camera.h"

typedef struct Mat4 {
    float m[16];
} Mat4;

Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
Vec3 vec3_scale(Vec3 v, float s);
float vec3_dot(Vec3 a, Vec3 b);
Vec3 vec3_cross(Vec3 a, Vec3 b);
float vec3_len(Vec3 v);
Vec3 vec3_norm(Vec3 v);

Mat4 mat4_identity(void);
Mat4 mat4_mul(Mat4 a, Mat4 b);
Mat4 mat4_perspective(float fov_y_radians, float aspect, float z_near, float z_far);
Mat4 mat4_look(Vec3 eye, Vec3 forward, Vec3 up);

