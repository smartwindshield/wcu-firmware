#ifndef HAL_GPIO_PICO_H
#define HAL_GPIO_PICO_H

#include "HAL_GPIO.h"

#include "pico/types.h"

#define HAL_GPIO_PICO_I2C_INTERNAL_PULLUPS 0

static const uint HAL_TO_PICO_PIN_MAPPINGS[] = {
    0, // I2C_SDA
    1, // I2C_SCL
    2, // CAM_SCK
    3, // CAM_MOSI
    4, // CAM_MISO
    5, // CAM_CS
    6, // BARO_INT
    7, // GPS_NRST
    8, // IMU_NRST
    9, // IMU_INTN
    15 // LED_DATA_3v3
};
static const size_t HAL_TO_PICO_PIN_MAPPINGS_SIZE = sizeof(HAL_TO_PICO_PIN_MAPPINGS);

#endif // HAL_GPIO_PICO_H
