#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

typedef struct Vector3_T {
    uint32_t x;
    uint32_t y;
    uint32_t z;
} Vector3;

typedef struct Vector3D_T {
    double x;
    double y;
    double z;
} Vector3D;

typedef struct Vector2D_T {
    double x;
    double y;
} Vector2D;

#endif
