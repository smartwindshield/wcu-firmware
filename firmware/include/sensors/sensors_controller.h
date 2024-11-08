#ifndef SENSORS_CONTROLLER_H
#define SENSORS_CONTROLLER_H

#include "HAL_I2C.h"

static const HAL_I2C_Bus SENSORS_I2C_BUS = HAL_I2C_BUS_0;
static const int SENSORS_I2C_BAUD_RATE = 100 * 1000;

void SensorsController_Init(void);

/**
 * Called once per iteration of main loop.
 */
void SensorsController_Update(void);

#endif // SENSORS_CONTROLLER_H
