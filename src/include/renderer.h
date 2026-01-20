#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "math4.h"
#include "mesh.h"

typedef struct Renderer {
    uint32_t program;
    uint32_t vao;
    uint32_t vbo;
    uint32_t texture_atlas;
    int u_mvp;
    int u_light_dir;
} Renderer;

bool renderer_init(Renderer* r);
void renderer_shutdown(Renderer* r);
void renderer_resize(int width, int height);
void renderer_draw_mesh(Renderer* r, const Mesh* mesh, Mat4 mvp);

