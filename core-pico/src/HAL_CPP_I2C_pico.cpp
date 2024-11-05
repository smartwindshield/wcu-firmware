#include "HAL_CPP_I2C.hpp"

static PicoI2C picoTwoWire;

TwoWire HAL_CPP_I2C_GetTwoWireImpl(void) {
    return picoTwoWire;
}
