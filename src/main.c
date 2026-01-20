#include "app.h"
#include "camera.h"
#include "gl_loader.h"
#include "math4.h"
#include "renderer.h"
#include "world.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef struct DynFloats {
    float* data;
    size_t count;
    size_t cap;
} DynFloats;

static void df_push(DynFloats* a, float v) {
    if (a->count + 1 > a->cap) {
        size_t new_cap = a->cap ? a->cap * 2 : 1024;
        float* p = (float*)realloc(a->data, new_cap * sizeof(float));
        if (!p) return;
        a->data = p;
        a->cap = new_cap;
    }
    a->data[a->count++] = v;
}

static void push_vertex(DynFloats* a, float px, float py, float pz, float u, float v, float nx, float ny, float nz) {
    df_push(a, px);
    df_push(a, py);
    df_push(a, pz);
    df_push(a, u);
    df_push(a, v);
    df_push(a, nx);
    df_push(a, ny);
    df_push(a, nz);
}

static void tile_uv(int tile_x, float local_u, float local_v, float* out_u, float* out_v) {
    const float atlas_w = 64.0f;
    const float atlas_h = 16.0f;
    const float tile = 16.0f;
    const float pad = 0.5f;

    float u0 = (tile_x * tile + pad) / atlas_w;
    float v0 = (0.0f + pad) / atlas_h;
    float u1 = ((tile_x + 1) * tile - pad) / atlas_w;
    float v1 = ((0.0f + 1.0f) * tile - pad) / atlas_h;

    *out_u = u0 + (u1 - u0) * local_u;
    *out_v = v0 + (v1 - v0) * local_v;
}

static void add_face(DynFloats* a, float x, float y, float z, int face, int tile_x, float sx, float sy, float sz) {
    float u00, v00, u10, v10, u11, v11, u01, v01;
    tile_uv(tile_x, 0.0f, 0.0f, &u00, &v00);
    tile_uv(tile_x, 1.0f, 0.0f, &u10, &v10);
    tile_uv(tile_x, 1.0f, 1.0f, &u11, &v11);
    tile_uv(tile_x, 0.0f, 1.0f, &u01, &v01);

    if (face == 0) {
        push_vertex(a, x + sx, y + 0,  z + 0,  u00, v00, 1, 0, 0);
        push_vertex(a, x + sx, y + sy, z + 0,  u01, v01, 1, 0, 0);
        push_vertex(a, x + sx, y + sy, z + sz, u11, v11, 1, 0, 0);
        push_vertex(a, x + sx, y + 0,  z + 0,  u00, v00, 1, 0, 0);
        push_vertex(a, x + sx, y + sy, z + sz, u11, v11, 1, 0, 0);
        push_vertex(a, x + sx, y + 0,  z + sz, u10, v10, 1, 0, 0);
        return;
    }
    if (face == 1) {
        push_vertex(a, x + 0,  y + 0,  z + sz, u00, v00, -1, 0, 0);
        push_vertex(a, x + 0,  y + sy, z + sz, u01, v01, -1, 0, 0);
        push_vertex(a, x + 0,  y + sy, z + 0,  u11, v11, -1, 0, 0);
        push_vertex(a, x + 0,  y + 0,  z + sz, u00, v00, -1, 0, 0);
        push_vertex(a, x + 0,  y + sy, z + 0,  u11, v11, -1, 0, 0);
        push_vertex(a, x + 0,  y + 0,  z + 0,  u10, v10, -1, 0, 0);
        return;
    }
    if (face == 2) {
        push_vertex(a, x + 0,  y + sy, z + 0,  u00, v00, 0, 1, 0);
        push_vertex(a, x + 0,  y + sy, z + sz, u01, v01, 0, 1, 0);
        push_vertex(a, x + sx, y + sy, z + sz, u11, v11, 0, 1, 0);
        push_vertex(a, x + 0,  y + sy, z + 0,  u00, v00, 0, 1, 0);
        push_vertex(a, x + sx, y + sy, z + sz, u11, v11, 0, 1, 0);
        push_vertex(a, x + sx, y + sy, z + 0,  u10, v10, 0, 1, 0);
        return;
    }
    if (face == 3) {
        push_vertex(a, x + sx, y + 0,  z + 0,  u00, v00, 0, -1, 0);
        push_vertex(a, x + sx, y + 0,  z + sz, u01, v01, 0, -1, 0);
        push_vertex(a, x + 0,  y + 0,  z + sz, u11, v11, 0, -1, 0);
        push_vertex(a, x + sx, y + 0,  z + 0,  u00, v00, 0, -1, 0);
        push_vertex(a, x + 0,  y + 0,  z + sz, u11, v11, 0, -1, 0);
        push_vertex(a, x + 0,  y + 0,  z + 0,  u10, v10, 0, -1, 0);
        return;
    }
    if (face == 4) {
        push_vertex(a, x + 0,  y + 0,  z + 0,  u00, v00, 0, 0, -1);
        push_vertex(a, x + 0,  y + sy, z + 0,  u01, v01, 0, 0, -1);
        push_vertex(a, x + sx, y + sy, z + 0,  u11, v11, 0, 0, -1);
        push_vertex(a, x + 0,  y + 0,  z + 0,  u00, v00, 0, 0, -1);
        push_vertex(a, x + sx, y + sy, z + 0,  u11, v11, 0, 0, -1);
        push_vertex(a, x + sx, y + 0,  z + 0,  u10, v10, 0, 0, -1);
        return;
    }
    if (face == 5) {
        push_vertex(a, x + sx, y + 0,  z + sz, u00, v00, 0, 0, 1);
        push_vertex(a, x + sx, y + sy, z + sz, u01, v01, 0, 0, 1);
        push_vertex(a, x + 0,  y + sy, z + sz, u11, v11, 0, 0, 1);
        push_vertex(a, x + sx, y + 0,  z + sz, u00, v00, 0, 0, 1);
        push_vertex(a, x + 0,  y + sy, z + sz, u11, v11, 0, 0, 1);
        push_vertex(a, x + 0,  y + 0,  z + sz, u10, v10, 0, 0, 1);
        return;
    }
}

static Mesh make_hand_mesh(void) {
    DynFloats verts = { 0 };
    float sx = 0.20f, sy = 0.20f, sz = 0.20f;
    int tile = 2;
    add_face(&verts, -sx * 0.5f, -sy * 0.5f, -sz * 0.5f, 0, tile, sx, sy, sz);
    add_face(&verts, -sx * 0.5f, -sy * 0.5f, -sz * 0.5f, 1, tile, sx, sy, sz);
    add_face(&verts, -sx * 0.5f, -sy * 0.5f, -sz * 0.5f, 2, tile, sx, sy, sz);
    add_face(&verts, -sx * 0.5f, -sy * 0.5f, -sz * 0.5f, 3, tile, sx, sy, sz);
    add_face(&verts, -sx * 0.5f, -sy * 0.5f, -sz * 0.5f, 4, tile, sx, sy, sz);
    add_face(&verts, -sx * 0.5f, -sy * 0.5f, -sz * 0.5f, 5, tile, sx, sy, sz);
    Mesh m = { 0 };
    m.vertices = verts.data;
    m.vertex_count = verts.count / 8;
    return m;
}

static Mat4 make_model(Vec3 pos, Vec3 right, Vec3 up, Vec3 forward) {
    Mat4 m = mat4_identity();
    m.m[0] = right.x;
    m.m[4] = right.y;
    m.m[8] = right.z;
    m.m[1] = up.x;
    m.m[5] = up.y;
    m.m[9] = up.z;
    m.m[2] = forward.x;
    m.m[6] = forward.y;
    m.m[10] = forward.z;
    m.m[12] = pos.x;
    m.m[13] = pos.y;
    m.m[14] = pos.z;
    return m;
}

static bool aabb_hits_world(const World* world, float minx, float miny, float minz, float maxx, float maxy, float maxz) {
    int x0 = (int)floorf(minx);
    int y0 = (int)floorf(miny);
    int z0 = (int)floorf(minz);
    int x1 = (int)floorf(maxx);
    int y1 = (int)floorf(maxy);
    int z1 = (int)floorf(maxz);

    for (int z = z0; z <= z1; z++) {
        for (int y = y0; y <= y1; y++) {
            for (int x = x0; x <= x1; x++) {
                BlockType t = world_get(world, x, y, z);
                if (world_is_solid(t)) return true;
            }
        }
    }
    return false;
}

static void move_player(World* world, Camera* cam, const AppInput* in, float dt) {
    const float player_radius = 0.3f;
    const float player_height = 1.8f;
    const float eye_height = 1.62f;

    const float mouse_sensitivity = 0.0022f;
    camera_apply_mouse(cam, in->mouse_dx, in->mouse_dy, mouse_sensitivity);

    float speed = in->keys[VK_SHIFT] ? 8.0f : 5.0f;
    Vec3 f = camera_forward_xz(cam);
    Vec3 r = camera_right_xz(cam);
    Vec3 move = (Vec3){ 0 };

    if (in->keys['W']) move = vec3_add(move, f);
    if (in->keys['S']) move = vec3_sub(move, f);
    if (in->keys['D']) move = vec3_add(move, r);
    if (in->keys['A']) move = vec3_sub(move, r);

    if (vec3_len(move) > 0.001f) move = vec3_norm(move);

    cam->velocity.x = move.x * speed;
    cam->velocity.z = move.z * speed;

    if (cam->on_ground && in->keys[VK_SPACE]) {
        cam->velocity.y = 7.0f;
        cam->on_ground = false;
    }

    cam->velocity.y += -20.0f * dt;
    if (cam->velocity.y < -60.0f) cam->velocity.y = -60.0f;

    Vec3 p = cam->position_feet;

    p.x += cam->velocity.x * dt;
    {
        float minx = p.x - player_radius;
        float maxx = p.x + player_radius;
        float miny = p.y;
        float maxy = p.y + player_height;
        float minz = p.z - player_radius;
        float maxz = p.z + player_radius;
        if (aabb_hits_world(world, minx, miny, minz, maxx, maxy, maxz)) {
            p.x -= cam->velocity.x * dt;
            cam->velocity.x = 0.0f;
        }
    }

    p.z += cam->velocity.z * dt;
    {
        float minx = p.x - player_radius;
        float maxx = p.x + player_radius;
        float miny = p.y;
        float maxy = p.y + player_height;
        float minz = p.z - player_radius;
        float maxz = p.z + player_radius;
        if (aabb_hits_world(world, minx, miny, minz, maxx, maxy, maxz)) {
            p.z -= cam->velocity.z * dt;
            cam->velocity.z = 0.0f;
        }
    }

    float dy = cam->velocity.y * dt;
    p.y += dy;
    cam->on_ground = false;
    {
        float minx = p.x - player_radius;
        float maxx = p.x + player_radius;
        float miny = p.y;
        float maxy = p.y + player_height;
        float minz = p.z - player_radius;
        float maxz = p.z + player_radius;

        if (aabb_hits_world(world, minx, miny, minz, maxx, maxy, maxz)) {
            p.y -= dy;
            if (dy < 0.0f) cam->on_ground = true;
            cam->velocity.y = 0.0f;
        }
    }

    if (p.y < 1.0f) {
        p.y = 1.0f;
        cam->velocity.y = 0.0f;
        cam->on_ground = true;
    }

    cam->position_feet = p;

    (void)eye_height;
}

static Mat4 camera_view_proj(const Camera* cam, int width, int height) {
    const float eye_height = 1.62f;
    Vec3 eye = (Vec3){ cam->position_feet.x, cam->position_feet.y + eye_height, cam->position_feet.z };

    float cy = cosf(cam->yaw);
    float sy = sinf(cam->yaw);
    float cp = cosf(cam->pitch);
    float sp = sinf(cam->pitch);

    Vec3 forward = (Vec3){ sy * cp, sp, -cy * cp };
    forward = vec3_norm(forward);

    Mat4 view = mat4_look(eye, forward, (Vec3){ 0.0f, 1.0f, 0.0f });
    float aspect = (height > 0) ? ((float)width / (float)height) : 1.0f;
    Mat4 proj = mat4_perspective(70.0f * (3.14159265f / 180.0f), aspect, 0.05f, 300.0f);
    return mat4_mul(proj, view);
}

int main(void) {
    AppWindow* win = NULL;
    AppWindowDesc desc = { "Minecraft C (Voxel)", 1280, 720 };
    if (!app_window_create(&win, desc)) {
        return 1;
    }
    app_window_set_cursor_locked(win, true);

    if (!app_window_make_gl_current(win)) {
        app_window_destroy(win);
        return 1;
    }

    if (!gl_loader_init()) {
        app_window_destroy(win);
        return 1;
    }

    Renderer renderer;
    if (!renderer_init(&renderer)) {
        app_window_destroy(win);
        return 1;
    }

    World world;
    if (!world_init(&world, 64, 24, 64)) {
        renderer_shutdown(&renderer);
        app_window_destroy(win);
        return 1;
    }
    world_generate_flat(&world);
    Mesh mesh = world_build_mesh(&world);

    Camera cam;
    camera_init(&cam);
    cam.position_feet = (Vec3){ 12.0f, 8.0f, 12.0f };

    AppInput input;
    memset(&input, 0, sizeof(input));

    double prev = app_time_seconds();
    while (!input.quit_requested) {
        app_window_poll(win, &input);
        if (input.keys[VK_ESCAPE]) break;
        if (!input.has_focus) {
            input.mouse_dx = 0;
            input.mouse_dy = 0;
        }

        double now = app_time_seconds();
        float dt = (float)(now - prev);
        prev = now;
        if (dt > 0.05f) dt = 0.05f;

        move_player(&world, &cam, &input, dt);

        renderer_resize(input.width, input.height);

        glClearColor(0.52f, 0.75f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Mat4 vp = camera_view_proj(&cam, input.width, input.height);
        renderer_draw_mesh(&renderer, &mesh, vp);

        app_window_swap_buffers(win);
    }

    mesh_free(&mesh);
    world_shutdown(&world);
    renderer_shutdown(&renderer);
    app_window_destroy(win);
    return 0;
}
