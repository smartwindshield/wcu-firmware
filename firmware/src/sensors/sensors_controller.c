#include "sensors/sensors_controller.h"
#include "../../firmware/include/sensors/gps.h"

#include "HAL_I2C.h"

void SensorsController_Init(void) {
    HAL_I2C_Init(SENSORS_I2C_BUS, SENSORS_I2C_BAUD_RATE);
}

void SensorsController_Update(void) {

}

