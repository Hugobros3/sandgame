#ifndef SANDGAME_H
#define SANDGAME_H

#include <stdbool.h>

typedef enum {
    AIR,
    BRICK,
    SAND,
} VoxelType;

typedef struct {
    int width, height;
    VoxelType* voxels;
} World;

World* make_world(int, int);
void add_sand(World*);
void tick_world(const World* src, World* dst);

bool in_bounds(World*, int, int);
VoxelType* access_world(World*, int, int);

#endif
