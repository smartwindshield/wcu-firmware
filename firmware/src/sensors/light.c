#include "sensors/light.h"
#include "sensors/sensors_controller.h"

#include "HAL_I2C.h"

static const uint8_t DEVICE_ADDRESS = 0x29;
static const uint8_t REG_ALS_CONTR = 0x80;
static const uint8_t REG_ALS_DATA_CH1 = 0x88;
static const uint8_t REG_ALS_DATA_CH0 = 0x8A;

static uint8_t ReadReg(uint8_t regAddr) {
    uint8_t contents;
    HAL_I2C_Write(SENSORS_I2C_BUS, DEVICE_ADDRESS, &regAddr, 1);
    HAL_I2C_Read(SENSORS_I2C_BUS, DEVICE_ADDRESS, &contents, 1);

    return contents;
}

void LightSensor_Init(void) {
    // Send I2C command to turn sensor on
    uint8_t ctrlReg = ReadReg(REG_ALS_CONTR);

    // Set the sensor to "active" mode
    ctrlReg |= 0b00000001;

    HAL_I2C_Write(SENSORS_I2C_BUS, DEVICE_ADDRESS, &ctrlReg, 1);
}

uint16_t LightSensor_GetIntensity(void) {
    // Read all 4 ALS DATA registers (2 for CH1 and 2 for CH0)
    uint8_t dataRegs[4];

    for (int i = 0; i < 4; i++) {
        // The data regs are next to each other, incrementing from DATA_CH1
        dataRegs[i] = ReadReg(REG_ALS_DATA_CH1 + i);
    }

    // Return the ch0 LSB and MSB
    // TODO: determine which channel to use
    // TODO: endianness
    return (((uint16_t) dataRegs[2]) << 8) & dataRegs[3];
}
