#include "sensors/gps.h"

#include "ArduinoToPico.hpp"
#include "HAL_CPP_I2C.hpp"

#include "SparkFun_u-blox_GNSS_Pico_Library.hpp"

static PicoPrintfStream debuggerStream;
static PicoTwoWire twoWireImpl;
static SFE_UBLOX_GNSS gnss;

void GPS_Init(void) {
    int32_t alt;
    twoWireImpl = HAL_CPP_I2C_GetTwoWireImpl();

    debuggerStream.println("[GPS]: Initializing.");

    // Enable the ublox library debugging and connect to GPS chip
    gnss.enableDebugging(debuggerStream);
    gnss.begin(twoWireImpl);

    // GPS chip communication initialized at this point

    alt = gnss.getAltitude();
    debuggerStream.print("[GPS]: GPS Reports an altitude of ");
    debuggerStream.println(alt);
}

void GPS_Update(void){
    // TODO: Determine if GPS data caching is something we need to implement or not.
    // Don't want to flood the I2C bus everytime GPS_GetData() is called
}

GPSData GPS_GetData(void) {
    GPSData data;

    data.altitude = gnss.getAltitude();
    data.latitude = gnss.getLatitude();
    data.longitude = gnss.getLongitude();

    return data;
}
