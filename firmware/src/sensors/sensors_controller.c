#include "sensors/sensors_controller.h"

#include "HAL_I2C.h"

static const HAL_I2C_Bus I2C_BUS = HAL_I2C_BUS_0;
static const int I2C_BAUD_RATE = 100000;

void SensorsController_Init(void) {
    HAL_I2C_Init(I2C_BUS, I2C_BAUD_RATE);
}

void SensorsController_Update(void) {

}

