#ifndef HAL_CPP_I2C_HPP
#define HAL_CPP_I2C_HPP

#include "ArduinoToPico.hpp"

/**
 * Get the default Arduino TwoWire implementation for use with 
 * the u-blox ported library
 */
TwoWire HAL_CPP_I2C_GetTwoWireImpl(void);

#endif // HAL_CPP_I2C_HPP
