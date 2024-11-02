#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdbool.h>

enum HAL_Pin {
    HAL_PIN_I2C_SDA,
    HAL_PIN_I2C_SCL,
    HAL_PIN_CAMERA_SCK,
    HAL_PIN_CAMERA_MOSI,
    HAL_PIN_CAMERA_MISO,
    HAL_PIN_CAMERA_CS,
    HAL_PIN_BARO_INT,
    HAL_PIN_GPS_NRST,
    HAL_PIN_IMU_NRST,
    HAL_PIN_IMU_INTN,
    HAL_PIN_LED_DATA,
};

/**
 * Initializes the underlying implementation for GPIO operations.
 */
void HAL_GPIO_Init(void);

/**
 * Set a GPIO pin On or Off.
 */
void HAL_GPIO_SetPin(unsigned int pin, bool state);

/**
 * Get the state of a GPIO (if it's high or low)
 */
bool HAL_GPIO_GetPin(unsigned int pin);

#endif // HAL_GPIO_H
