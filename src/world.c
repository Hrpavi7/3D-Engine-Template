#include "world.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static bool in_bounds(const World* world, int x, int y, int z) {
    return x >= 0 && x < world->w && y >= 0 && y < world->h && z >= 0 && z < world->d;
}

bool world_init(World* world, int w, int h, int d) {
    memset(world, 0, sizeof(*world));
    world->w = w;
    world->h = h;
    world->d = d;
    size_t n = (size_t)w * (size_t)h * (size_t)d;
    world->blocks = (uint8_t*)malloc(n);
    if (!world->blocks) return false;
    memset(world->blocks, 0, n);
    return true;
}

void world_shutdown(World* world) {
    if (!world) return;
    free(world->blocks);
    world->blocks = NULL;
    world->w = world->h = world->d = 0;
}

BlockType world_get(const World* world, int x, int y, int z) {
    if (!world || !world->blocks) return BLOCK_AIR;
    if (!in_bounds(world, x, y, z)) return BLOCK_AIR;
    size_t idx = (size_t)x + (size_t)world->w * ((size_t)z + (size_t)world->d * (size_t)y);
    return (BlockType)world->blocks[idx];
}

void world_set(World* world, int x, int y, int z, BlockType t) {
    if (!world || !world->blocks) return;
    if (!in_bounds(world, x, y, z)) return;
    size_t idx = (size_t)x + (size_t)world->w * ((size_t)z + (size_t)world->d * (size_t)y);
    world->blocks[idx] = (uint8_t)t;
}

bool world_is_solid(BlockType t) {
    return t != BLOCK_AIR;
}

void world_generate_flat(World* world) {
    for (int z = 0; z < world->d; z++) {
        for (int x = 0; x < world->w; x++) {
            float nx = (float)x / (float)world->w;
            float nz = (float)z / (float)world->d;
            float h = 6.0f + 2.0f * sinf(nx * 6.28318f * 1.5f) * cosf(nz * 6.28318f * 1.25f);
            int top = (int)floorf(h);
            if (top < 1) top = 1;
            if (top > world->h - 2) top = world->h - 2;

            for (int y = 0; y <= top; y++) {
                if (y == top) {
                    world_set(world, x, y, z, BLOCK_GRASS);
                } else if (y >= top - 3) {
                    world_set(world, x, y, z, BLOCK_DIRT);
                } else {
                    world_set(world, x, y, z, BLOCK_STONE);
                }
            }
        }
    }
}

typedef struct DynFloats {
    float* data;
    size_t count;
    size_t cap;
} DynFloats;

static void df_push(DynFloats* a, float v) {
    if (a->count + 1 > a->cap) {
        size_t new_cap = a->cap ? a->cap * 2 : 4096;
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

static int tile_for_face(BlockType t, int face) {
    if (t == BLOCK_GRASS) {
        if (face == 2) return 0;
        if (face == 3) return 2;
        return 1;
    }
    if (t == BLOCK_DIRT) return 2;
    if (t == BLOCK_STONE) return 3;
    return 3;
}

static void add_face(DynFloats* a, float x, float y, float z, int face, int tile_x) {
    float u00, v00, u10, v10, u11, v11, u01, v01;
    tile_uv(tile_x, 0.0f, 0.0f, &u00, &v00);
    tile_uv(tile_x, 1.0f, 0.0f, &u10, &v10);
    tile_uv(tile_x, 1.0f, 1.0f, &u11, &v11);
    tile_uv(tile_x, 0.0f, 1.0f, &u01, &v01);

    if (face == 0) {
        push_vertex(a, x + 1, y + 0, z + 0, u00, v00, 1, 0, 0);
        push_vertex(a, x + 1, y + 1, z + 0, u01, v01, 1, 0, 0);
        push_vertex(a, x + 1, y + 1, z + 1, u11, v11, 1, 0, 0);
        push_vertex(a, x + 1, y + 0, z + 0, u00, v00, 1, 0, 0);
        push_vertex(a, x + 1, y + 1, z + 1, u11, v11, 1, 0, 0);
        push_vertex(a, x + 1, y + 0, z + 1, u10, v10, 1, 0, 0);
        return;
    }
    if (face == 1) {
        push_vertex(a, x + 0, y + 0, z + 1, u00, v00, -1, 0, 0);
        push_vertex(a, x + 0, y + 1, z + 1, u01, v01, -1, 0, 0);
        push_vertex(a, x + 0, y + 1, z + 0, u11, v11, -1, 0, 0);
        push_vertex(a, x + 0, y + 0, z + 1, u00, v00, -1, 0, 0);
        push_vertex(a, x + 0, y + 1, z + 0, u11, v11, -1, 0, 0);
        push_vertex(a, x + 0, y + 0, z + 0, u10, v10, -1, 0, 0);
        return;
    }
    if (face == 2) {
        push_vertex(a, x + 0, y + 1, z + 0, u00, v00, 0, 1, 0);
        push_vertex(a, x + 0, y + 1, z + 1, u01, v01, 0, 1, 0);
        push_vertex(a, x + 1, y + 1, z + 1, u11, v11, 0, 1, 0);
        push_vertex(a, x + 0, y + 1, z + 0, u00, v00, 0, 1, 0);
        push_vertex(a, x + 1, y + 1, z + 1, u11, v11, 0, 1, 0);
        push_vertex(a, x + 1, y + 1, z + 0, u10, v10, 0, 1, 0);
        return;
    }
    if (face == 3) {
        push_vertex(a, x + 1, y + 0, z + 0, u00, v00, 0, -1, 0);
        push_vertex(a, x + 1, y + 0, z + 1, u01, v01, 0, -1, 0);
        push_vertex(a, x + 0, y + 0, z + 1, u11, v11, 0, -1, 0);
        push_vertex(a, x + 1, y + 0, z + 0, u00, v00, 0, -1, 0);
        push_vertex(a, x + 0, y + 0, z + 1, u11, v11, 0, -1, 0);
        push_vertex(a, x + 0, y + 0, z + 0, u10, v10, 0, -1, 0);
        return;
    }
    if (face == 4) {
        push_vertex(a, x + 0, y + 0, z + 0, u00, v00, 0, 0, -1);
        push_vertex(a, x + 0, y + 1, z + 0, u01, v01, 0, 0, -1);
        push_vertex(a, x + 1, y + 1, z + 0, u11, v11, 0, 0, -1);
        push_vertex(a, x + 0, y + 0, z + 0, u00, v00, 0, 0, -1);
        push_vertex(a, x + 1, y + 1, z + 0, u11, v11, 0, 0, -1);
        push_vertex(a, x + 1, y + 0, z + 0, u10, v10, 0, 0, -1);
        return;
    }
    if (face == 5) {
        push_vertex(a, x + 1, y + 0, z + 1, u00, v00, 0, 0, 1);
        push_vertex(a, x + 1, y + 1, z + 1, u01, v01, 0, 0, 1);
        push_vertex(a, x + 0, y + 1, z + 1, u11, v11, 0, 0, 1);
        push_vertex(a, x + 1, y + 0, z + 1, u00, v00, 0, 0, 1);
        push_vertex(a, x + 0, y + 1, z + 1, u11, v11, 0, 0, 1);
        push_vertex(a, x + 0, y + 0, z + 1, u10, v10, 0, 0, 1);
        return;
    }
}

Mesh world_build_mesh(const World* world) {
    DynFloats verts = { 0 };

    for (int y = 0; y < world->h; y++) {
        for (int z = 0; z < world->d; z++) {
            for (int x = 0; x < world->w; x++) {
                BlockType t = world_get(world, x, y, z);
                if (!world_is_solid(t)) continue;

                BlockType nxp = world_get(world, x + 1, y, z);
                BlockType nxn = world_get(world, x - 1, y, z);
                BlockType nyp = world_get(world, x, y + 1, z);
                BlockType nyn = world_get(world, x, y - 1, z);
                BlockType nzp = world_get(world, x, y, z + 1);
                BlockType nzn = world_get(world, x, y, z - 1);

                if (!world_is_solid(nxp)) add_face(&verts, (float)x, (float)y, (float)z, 0, tile_for_face(t, 0));
                if (!world_is_solid(nxn)) add_face(&verts, (float)x, (float)y, (float)z, 1, tile_for_face(t, 1));
                if (!world_is_solid(nyp)) add_face(&verts, (float)x, (float)y, (float)z, 2, tile_for_face(t, 2));
                if (!world_is_solid(nyn)) add_face(&verts, (float)x, (float)y, (float)z, 3, tile_for_face(t, 3));
                if (!world_is_solid(nzn)) add_face(&verts, (float)x, (float)y, (float)z, 4, tile_for_face(t, 4));
                if (!world_is_solid(nzp)) add_face(&verts, (float)x, (float)y, (float)z, 5, tile_for_face(t, 5));
            }
        }
    }

    Mesh mesh = { 0 };
    mesh.vertices = verts.data;
    mesh.vertex_count = verts.count / 8;
    return mesh;
}

