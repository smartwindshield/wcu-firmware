#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>

/**
 * Windshield height and width stored in mm.
 */
static const uint32_t STORAGE_ID_WINDSHIELD_HEIGHT = 0;
static const uint32_t STORAGE_ID_WINDSHIELD_WIDTH = 1;
static const uint32_t STORAGE_ID_WINDSHIELD_DISTANCE_X = 2;
static const uint32_t STORAGE_ID_WINDSHIELD_DISTANCE_Y = 3;
static const uint32_t STORAGE_ID_WINDSHIELD_ANGLE = 4;

void Storage_Init(void);

uint32_t Storage_Retrieve(uint32_t id);

void Storage_Store(uint32_t id, uint32_t value);

#endif // STORAGE_H
