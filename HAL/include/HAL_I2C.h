#ifndef HAL_I2C_H
#define HAL_I2C_H

#include <stdint.h>

typedef enum HAL_I2C_Bus_T {
    HAL_I2C_BUS_0,
    HAL_I2C_BUS_1
} HAL_I2C_Bus;

void HAL_I2C_Init(HAL_I2C_Bus bus, int baud_rate);

void HAL_I2C_Write(HAL_I2C_Bus bus, uint8_t dest_addr, uint8_t *buffer, int buffer_len);

/**
 * Writes to the given address, but uses a ReStart condition to maintain control of the bus once complete.
 */
void HAL_I2C_WriteReStart(HAL_I2C_Bus bus, uint8_t dest_addr, uint8_t *buffer, int buffer_len);

/**
 * Attempt to read "buffer_len" bytes from the I2C device "src_addr".
 *
 * "buffer" must be allocated by the caller of this function, and must
 * be as large as "buffer_len"
 *
 * Returns the number of bytes read from the device, or -1 if no data was read.
 */
int HAL_I2C_Read(HAL_I2C_Bus bus, uint8_t src_addr, uint8_t *buffer, int buffer_len);


#endif // HAL_I2C_H
