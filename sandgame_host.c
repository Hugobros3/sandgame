#include "sandgame.h"

#include <stdlib.h>
#include <math.h>
#include <assert.h>

World* make_world(int w, int h) {
    World* world = calloc(sizeof(World), 1);
    world->width = w;
    world->height = h;
    world->voxels = calloc(w * h * sizeof(*world->voxels), 1);
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < 16; y++) {
            *access_world(world, x, y) = BRICK;
        }
    }
    return world;
}

void add_sand(World* world) {
    for (size_t i = 0; i < 1; i++) {
        int x = rand() % world->width;
        int y = rand() % (world->height - 16) + 16;
        *access_world(world, x, y) = SAND;
    }
}

void tick_world(const World* src, World* dst) {
    assert(src->width == dst->width);
    assert(src->height == dst->height);
#pragma omp parallel for num_threads(8) schedule(static)
    for (int x = 0; x < src->width; x++) {
#pragma omp simd simdlen(4)
        for (int y = 0; y < src->height; y++) {
            if (*access_world(src, x, y) == AIR) {
                if (in_bounds(src, x, y + 1) && *access_world(src, x, y + 1) == SAND) {
                    *access_world(dst, x, y) = SAND;
                } else
                    *access_world(dst, x, y) = AIR;
            } else if (*access_world(src, x, y) == BRICK) {
                *access_world(dst, x, y) = BRICK;
            } else if (*access_world(src, x, y) == SAND) {
                if (in_bounds(src, x, y - 1) && *access_world(src, x, y - 1) == AIR) {
                    *access_world(dst, x, y) = AIR;
                } else
                    *access_world(dst, x, y) = SAND;
            } else __builtin_unreachable();
        }
    }
}