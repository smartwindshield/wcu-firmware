#include "sensors/power_delivery.h"
#include "HAL_GPIO.h"
#include <stdint.h>
#include "HAL_I2C.h"

#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

#define GPS_I2C_ADDRESS 0x21

void PowerDeliveryDataUpdate(uint8_t powerDeliveryData[]){

    HAL_I2C_Read(HAL_I2C_BUS_0, GPS_I2C_ADDRESS, powerDeliveryData, 256);

}
