#include "HAL_I2C.h"

#include "hardware/i2c.h"

static const unsigned int DEFAULT_TIMEOUT_US = 1000;

static inline i2c_inst_t* GetI2CInstance(HAL_I2C_Bus bus) {
    switch (bus) {
        case HAL_I2C_BUS_0:
            return &i2c0_inst;
        case HAL_I2C_BUS_1:
            return &i2c1_inst;
        default:
            return &i2c0_inst;
    }
}

void HAL_I2C_Init(HAL_I2C_Bus bus, int baud_rate) {
    i2c_init(GetI2CInstance(bus), baud_rate);
}

void HAL_I2C_Write(HAL_I2C_Bus bus, uint8_t dest_addr, uint8_t *buffer, int buffer_len) {
    assert(NULL != buffer);

    i2c_write_timeout_us(GetI2CInstance(bus), dest_addr, buffer, buffer_len,
                         false, DEFAULT_TIMEOUT_US);
}

int HAL_I2C_Read(HAL_I2C_Bus bus, uint8_t src_addr, uint8_t *buffer, int buffer_len) {
    assert(NULL != buffer);

    return i2c_read_timeout_us(GetI2CInstance(bus), src_addr, buffer, buffer_len,
                               false, DEFAULT_TIMEOUT_US);
}