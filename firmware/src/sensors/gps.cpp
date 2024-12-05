#include "sensors/gps.h"

#include "ArduinoToPico.hpp"
#include "HAL_CPP_I2C.hpp"

#include "HAL_Debug.h"
#include "HAL_GPIO.h"
#include "HAL_Time.h"
#include "SparkFun_u-blox_GNSS_Pico_Library.hpp"

static const int MAX_GPS_TRIES = 20;
static const uint32_t GPS_DEBUG_STATUS_INTERVAL = 5000;

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

    if (gnssSuccess) {
        gnss.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
    }
}

void GPS_Update(void){
    static uint32_t lastCheck = 0;
    bool state;
    bool esfIns;
    bool hnrAtt;

    uint32_t currentTime = HAL_Time_GetTimeNow();

    if (currentTime - lastCheck >= GPS_DEBUG_STATUS_INTERVAL) {
#if 1
        state = gnss.getGnssFixOk();
        esfIns = gnss.getEsfIns();
        hnrAtt = gnss.getHNRAtt();

        HAL_Debug_Printf("[GPS]: GNSS Fix Status: %i, ESF/HNR Status: %i/%i\n", 
                         state, esfIns, hnrAtt);
#endif
        lastCheck = currentTime;
    }
}

bool GPS_HasChipConnectivity(void) {
    return gnssSuccess;
}

GPSData GPS_GetLocationData(void) {
    GPSData data;
    
    data.altitude = gnss.getAltitude();
    data.latitude = gnss.getLatitude();
    data.longitude = gnss.getLongitude();
    
    data.roll = gnss.getHNRroll();
    data.pitch = gnss.getHNRpitch();
    data.yaw = gnss.getHNRheading(); // TODO: heading same as yaw? probably

    return data;
}

GPSData GPS_GetHighRateLocationData(void) {
    GPSData data;
    
    data.roll = gnss.getHNRroll();
    data.pitch = gnss.getHNRpitch();
    data.yaw = gnss.getHNRheading(); // TODO: heading same as yaw? probably

    return data;
}

GPSData GPS_GetFullData(void) {
    GPSData data;
    
    data.altitude = gnss.getAltitude();
    data.latitude = gnss.getLatitude();
    data.longitude = gnss.getLongitude();

    data.year = gnss.getYear();
    data.month = gnss.getMonth();
    data.day = gnss.getDay();
    data.hour = gnss.getHour();
    data.minute = gnss.getMinute();
    data.second = gnss.getSecond();

    data.roll = gnss.getHNRroll();
    data.pitch = gnss.getHNRpitch();
    data.yaw = gnss.getHNRheading(); // TODO: heading same as yaw? probably

    return data;
}
