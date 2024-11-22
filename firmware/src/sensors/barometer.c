#include "sensors/barometer.h"
#include "HAL_Debug.h"
#include "HAL_I2C.h"
#include "sensors/sensors_controller.h"
#include "wcu_firmware_config.h"

static const uint8_t BAROMETER_I2C_ADDRESS = 0x5C;
static const float DEFAULT_PRESSURE = 1020.9; // Pressure reported at MSP on 11/22/24

void BarometerInit(void){
#ifndef BAROMETER_DISABLED
    uint8_t config[2] = {0x20, 0x90};

    HAL_I2C_Write(SENSORS_I2C_BUS, BAROMETER_I2C_ADDRESS, config, 2);

    HAL_Debug_Printf("[Barometer]: Initialized.\n");
#else
    HAL_Debug_Printf("[Barometer]: DISABLED, skipping initialization.\n");
#endif // BAROMETER_DISABLED
}

//Pressure in hectopascal
float BarometerGetData(void) {
    float pressure;
    uint8_t data[3];
    uint8_t reg[1] = {0x28 | 0x80};

#ifndef BAROMETER_DISABLED
    HAL_I2C_Write(SENSORS_I2C_BUS, BAROMETER_I2C_ADDRESS, reg, 1);

    HAL_I2C_Read(SENSORS_I2C_BUS, BAROMETER_I2C_ADDRESS, data, 3);

    pressure = ((data[2] << 16) | (data[1] << 8) | data[0]) / 4096.0;

    HAL_Debug_Printf("[Barometer]: Reporting %f hectopascal\n", pressure);
#else
    pressure = DEFAULT_PRESSURE;
    HAL_Debug_Printf("[Barometer]: DISABLED, reporting %f hectopascal\n", pressure);
#endif // BAROMETER_DISABLED

    return pressure;
}
