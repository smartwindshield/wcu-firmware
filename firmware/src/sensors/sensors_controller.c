#include "sensors/sensors_controller.h"
#include "sensors/gps.h"
#include "sensors/barometer.h"

#include "HAL_I2C.h"

void SensorsController_Init(void) {
    HAL_I2C_Init(SENSORS_I2C_BUS, SENSORS_I2C_BAUD_RATE);
    GPS_Init();
    BarometerInit();
}

void SensorsController_Update(void) {

}

