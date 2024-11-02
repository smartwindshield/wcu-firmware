#include "sensors/barometer.h"
#include "HAL_GPIO.h"
#include <stdio.h>
#include "HAL_I2C.h"

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

#define GPS_I2C_ADDRESS 0x5C

void BarometerDataUpdate(uint8_t barometerData[]){

    HAL_I2C_Read(HAL_I2C_BUS_0, GPS_I2C_ADDRESS, barometerData, 256);

}
