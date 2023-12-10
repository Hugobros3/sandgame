#include "sandgame.h"

VoxelType* access_world(World* w, int x, int y) {
    return &(w->voxels[y * w->width + x]);
}

bool in_bounds(World* w, int x, int y) {
    return x >= 0 && y >= 0 && x < w->width && y < w->height;
}
