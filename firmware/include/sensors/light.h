#ifndef SENSORS_LIGHT_H
#define SENSORS_LIGHT_H

#include <stdint.h>

void LightSensor_Init(void);

uint16_t LightSensor_GetIntensity(void);

#endif // SENSORS_LIGHT_H
