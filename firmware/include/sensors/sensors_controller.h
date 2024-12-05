#ifndef SENSORS_CONTROLLER_H
#define SENSORS_CONTROLLER_H

#include "HAL_I2C.h"

static const HAL_I2C_Bus SENSORS_I2C_BUS = HAL_I2C_BUS_0;
static const int SENSORS_I2C_BAUD_RATE = 100 * 1000;

typedef struct LocationData_T {
    // Lat/long in units of degrees
    double latitude;
    double longitude;

    // Vehicle pitch and yaw in units of degrees
    double pitch;
    double yaw;
} LocationData;

typedef struct DatetimeData_T {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} DatetimeData;

void SensorsController_Init(void);

/**
 * Called once per iteration of main loop.
 */
void SensorsController_Update(void);

LocationData SensorsController_GetLocationData(void);
DatetimeData SensorsController_GetDatetimeData(void);

double SensorsController_GetPressureKpa(void);
double SensorsController_GetTemperatureKelvin(void);

#endif // SENSORS_CONTROLLER_H
