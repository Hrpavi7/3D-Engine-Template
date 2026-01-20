#include "mesh.h"

#include <stdlib.h>

void mesh_free(Mesh* mesh) {
    if (!mesh) return;
    free(mesh->vertices);
    mesh->vertices = NULL;
    mesh->vertex_count = 0;
}

