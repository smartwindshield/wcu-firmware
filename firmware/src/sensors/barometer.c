#include "sensors/barometer.h"
#include "HAL_Debug.h"
#include "HAL_GPIO.h"
#include <stdio.h>
#include "HAL_I2C.h"

#define SDA_PIN 0
#define SCL_PIN 1

#define GPS_I2C_ADDRESS 0x5C

void BarometerInit(void){
    HAL_I2C_Init(HAL_I2C_BUS_0, 100000);
    uint8_t config[2] = {0x20, 0x90};

    HAL_I2C_Write(HAL_I2C_BUS_0, GPS_I2C_ADDRESS, config, 2);
}

float BarometerGetData(void){
    uint8_t reg[1] = {0x28 | 0x80};

    HAL_I2C_Write(HAL_I2C_BUS_0, GPS_I2C_ADDRESS, reg, 1);

    uint8_t data[3];

    HAL_I2C_Read(HAL_I2C_BUS_0, GPS_I2C_ADDRESS, data, 3);

    float pressure = ((data[2] << 16) | (data[1] << 8) | data[0]) / 4096.0;

    HAL_Debug_Printf("[Barometer]: Barometer reports %f hectopascal\n", pressure);

    return pressure;
}
