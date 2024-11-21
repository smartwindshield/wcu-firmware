#include "sensors/external_temp.h"
#include "HAL_GPIO.h"
#include <stdio.h>
#include "HAL_I2C.h"

#define SDA_PIN 0
#define SCL_PIN 1

#define GPS_I2C_ADDRESS 0x60

void External_Temp_Init(void){
    HAL_I2C_Init(HAL_I2C_BUS_0, 100000);
    uint8_t config[1] = {0x00};

    HAL_I2C_Write(HAL_I2C_BUS_0, GPS_I2C_ADDRESS, config, 1);
}

//Temperature in Celsius
float External_Temp_GetData(void){
    uint8_t read[1] = {0xC1};
    HAL_I2C_Write(HAL_I2C_BUS_0, GPS_I2C_ADDRESS, read, 1);

    uint8_t UpperByte[1];
    uint8_t LowerByte[1];

    HAL_I2C_Read(HAL_I2C_BUS_0, GPS_I2C_ADDRESS, UpperByte, 1);
    HAL_I2C_Read(HAL_I2C_BUS_0, GPS_I2C_ADDRESS, LowerByte, 1);

    float temp;

    if ((UpperByte[0] & 0x80) == 0x80) {
        temp = (UpperByte[0] * 16) + (LowerByte[0] / 16) - 4096;
    } else {
        temp = (UpperByte[0] * 16) + (LowerByte[0] / 16);
    }

    return temp;
}