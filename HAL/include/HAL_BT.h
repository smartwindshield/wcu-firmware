#ifndef HAL_BT_H
#define HAL_BT_H

#include <stdint.h>
#include <stddef.h>

// Needs to be larger than the biggest ATT_CHARACTERISTIC_.... value from GATT header
#define HAL_BT_MAX_CHARACTERISTICS     32
#define HAL_BT_MAX_CHARACTERISTIC_SIZE 4

/**
 * Initialize the device's bluetooth stack and advertise with the given
 * "device_name"
 *
 * @param profile_data Pointer to the generated profile data from the .GATT file,
 *                     found in the generated GATT header.
 */
void HAL_BT_Init(const char *device_name, const uint8_t *profile_data);

/**
 * Registers a characteristic to the HAL.
 *
 * @param id Should be the ATT_CHARACTERISTIC_... macro given by the autogenerated
 * GATT header file.
 *
 * This id will then be used for setting and getting the value of that characteristic.
 *
 * @returns 0 if successful, nonzero if unsuccessful.
 */
int HAL_BT_RegisterCharacteristic(uint8_t id, size_t size);

/**
 * Sets the value of a GATT characteristic. The next time the value is
 * read by a client it will reflect this new value.
 */
void HAL_BT_SetCharacteristic(uint8_t id, uint8_t *value, size_t size);

/**
 * Get the current value of a GATT characteristic.
 *
 * @param value Buffer to place the value of the characeristic in.
 *              Buffer MUST BE preallocated.
 * 
 * @returns The length of the value, or negative if characteristic doesn't exist.
 */
size_t HAL_BT_GetCharacteristic(uint8_t id, uint8_t *value);

#endif // HAL_BT_H
