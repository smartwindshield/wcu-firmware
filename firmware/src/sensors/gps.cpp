#include "sensors/gps.h"

#include "ArduinoToPico.hpp"
#include "HAL_CPP_I2C.hpp"

#include "HAL_Debug.h"
#include "HAL_GPIO.h"
#include "HAL_Time.h"
#include "SparkFun_u-blox_GNSS_Pico_Library.hpp"

static const int MAX_GPS_TRIES = 20;

static PicoPrintfStream debuggerStream;
static PicoTwoWire twoWireImpl;
static SFE_UBLOX_GNSS gnss;

static bool gnssSuccess;

void GPS_Init(void) {
    int32_t alt;
    twoWireImpl = HAL_CPP_I2C_GetTwoWireImpl();

    debuggerStream.println("[GPS]: Initializing.");

    HAL_GPIO_Init();

    // Enable the GPS chip
    HAL_GPIO_SetPin(HAL_PIN_GPS_NRST, true);

    // Enable the ublox library debugging and connect to GPS chip
    //gnss.enableDebugging(debuggerStream);
    gnssSuccess = gnss.begin(twoWireImpl);

    for(int i = 0; i < MAX_GPS_TRIES && !gnssSuccess; i++) {
        HAL_Time_Sleep(1000);
        HAL_Debug_Printf("[GPS]: Retrying connectivity (try %i of %i)\n", i, MAX_GPS_TRIES);
        gnssSuccess = gnss.begin(twoWireImpl);
    }
}

void GPS_Update(void){
    // TODO: Determine if GPS data caching is something we need to implement or not.
    // Don't want to flood the I2C bus everytime GPS_GetData() is called
}

bool GPS_HasChipConnectivity(void) {
    return gnssSuccess;
}

GPSData GPS_GetData(void) {
    GPSData data;

    bool state = gnss.getGnssFixOk();
    HAL_Debug_Printf("[GPS]: GNSS Fix Status: %i\n", state);

    data.altitude = gnss.getAltitude();
    data.latitude = gnss.getLatitude();
    data.longitude = gnss.getLongitude();

    data.year = gnss.getYear();
    data.month = gnss.getMonth();
    data.day = gnss.getDay();
    data.hour = gnss.getHour();
    data.minute = gnss.getMinute();
    data.second = gnss.getSecond();

    data.roll = gnss.getATTroll();
    data.pitch = gnss.getATTpitch();
    data.yaw = gnss.getATTheading(); // TODO: heading same as yaw? probably

    return data;
}
