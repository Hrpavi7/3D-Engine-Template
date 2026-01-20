#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct Mesh {
    float* vertices;
    size_t vertex_count;
} Mesh;

void mesh_free(Mesh* mesh);

