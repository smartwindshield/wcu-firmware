#include "sensors/external_temp.h"
#include "HAL_Debug.h"
#include "HAL_GPIO.h"
#include <stdio.h>
#include "HAL_I2C.h"
#include "wcu_firmware_config.h"

#define SDA_PIN 0
#define SCL_PIN 1

#define EXTERNAL_TEMP_I2C_ADDRESS 0x60

void External_Temp_Init(void){
#ifndef EXTERNAL_TEMP_DISABLED
    HAL_I2C_Init(HAL_I2C_BUS_0, 100000);
    uint8_t config[1] = {0x00};

    HAL_I2C_Write(HAL_I2C_BUS_0, EXTERNAL_TEMP_I2C_ADDRESS, config, 1);

    HAL_Debug_Printf("[EXT Temp]: Initialized.\n");
#endif // EXTERNAL_TEMP_DISABLED
}

//Temperature in Celsius
float External_Temp_GetData(void){
#ifdef EXTERNAL_TEMP_DISABLED
    HAL_Debug_Printf("[EXT Temp]: DISABLED, reporting 68 degrees C\n");
    return 68.0; // Hardcoded value
#else
    int result[2];
    uint8_t read[1] = {0xC1};
    HAL_I2C_Write(HAL_I2C_BUS_0, EXTERNAL_TEMP_I2C_ADDRESS, read, 1);

    uint8_t UpperByte[1];
    uint8_t LowerByte[1];

    result[0] = HAL_I2C_Read(HAL_I2C_BUS_0, EXTERNAL_TEMP_I2C_ADDRESS, UpperByte, 1);
    result[1] = HAL_I2C_Read(HAL_I2C_BUS_0, EXTERNAL_TEMP_I2C_ADDRESS, LowerByte, 1);

    float temp;

    if ((UpperByte[0] & 0x80) == 0x80) {
        temp = (UpperByte[0] * 16) + (LowerByte[0] / 16) - 4096;
    } else {
        temp = (UpperByte[0] * 16) + (LowerByte[0] / 16);
    }

    HAL_Debug_Printf("[EXT Temp]: Reporting %f degrees C (RAW: %X %X, I2C Result: %i %i)\n",
                     temp, UpperByte, LowerByte, result[0], result[1]);

    return temp;
#endif // EXTERNAL_TEMP_DISABLED
}
