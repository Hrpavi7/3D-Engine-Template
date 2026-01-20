#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "mesh.h"

typedef enum BlockType {
    BLOCK_AIR = 0,
    BLOCK_GRASS = 1,
    BLOCK_DIRT = 2,
    BLOCK_STONE = 3
} BlockType;

typedef struct World {
    int w;
    int h;
    int d;
    uint8_t* blocks;
} World;

bool world_init(World* world, int w, int h, int d);
void world_shutdown(World* world);

BlockType world_get(const World* world, int x, int y, int z);
void world_set(World* world, int x, int y, int z, BlockType t);

void world_generate_flat(World* world);
bool world_is_solid(BlockType t);

Mesh world_build_mesh(const World* world);
