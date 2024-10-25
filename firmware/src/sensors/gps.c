#include "gps.h"
#include "HAL_GPIO.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "HAL_I2C.h"

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

#define GPS_I2C_ADDRESS 0x42

void GPSDataUpdate(uint8_t gpsData[]){

    HAL_I2C_Read(HAL_I2C_BUS_0, GPS_I2C_ADDRESS, gpsData, 256);

    return 0;
}