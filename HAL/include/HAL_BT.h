#ifndef HAL_BT_H
#define HAL_BT_H

#include <stdint.h>

void HAL_BT_Init(uint32_t heartbeatPeriod);

void HAL_BT_RegisterPacketHandler();

#endif // HAL_BT_H
