#include "sensors/gps.h"
#include "HAL_GPIO.h"
#include <stdio.h>
#include "HAL_I2C.h"
#include "HAL_CPP_I2C.hpp"

#include "SparkFun_u-blox_GNSS_Pico_Library.hpp"

#define GPS_I2C_ADDRESS 0x42

static SFE_UBLOX_GNSS myGNSS;

void GPS_Init(void) {
    // Try to connect
    TwoWire twoWireImpl = HAL_CPP_I2C_GetTwoWireImpl();

    myGNSS.begin(twoWireImpl);
    myGNSS.setI2CAddress(GPS_I2C_ADDRESS);
    myGNSS.setI2COutput(COM_TYPE_UBX);
    myGNSS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);

    long lat = myGNSS.getLatitude();

    long lon = myGNSS.getLongitude();

    printf("Latitude: %ld\n", lat);

    printf("Longitude: %ld\n", lon);

    return;

    // do stuff with gnss
}

void GPSDataUpdate(uint8_t gpsData[]){

    //HAL_I2C_Read(HAL_I2C_BUS_0, (uint8_t) GPS_I2C_ADDRESS, gpsData, 256);

}
