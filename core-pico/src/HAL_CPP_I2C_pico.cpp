#include "HAL_CPP_I2C.hpp"

static PicoTwoWire picoTwoWire;

PicoTwoWire HAL_CPP_I2C_GetTwoWireImpl(void) {
    return picoTwoWire;
}
