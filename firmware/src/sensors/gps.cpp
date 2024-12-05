#include "sensors/gps.h"

#include "ArduinoToPico.hpp"
#include "HAL_CPP_I2C.hpp"

#include "HAL_Debug.h"
#include "HAL_GPIO.h"
#include "HAL_Time.h"
#include "SparkFun_u-blox_GNSS_Pico_Library.hpp"

static const int MAX_GPS_TRIES = 20;
static const uint32_t GPS_DEBUG_STATUS_INTERVAL = 5000;

static const uint8_t HNR_RATE = 25; // 20 Hz update for HNR data on GPS chip
static const uint8_t PVT_RATE = 4; // 4 Hz update for general navigation data on GPS chip
// Max wait for PVT and HNR should be slightly longer than the HNR and PVT periods
static const uint16_t PVT_MAX_WAIT = 300;
static const uint16_t HNR_MAX_WAIT = 80;

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

        // Set the HNR and Navigation frequencies on the chip
        // This tells the chip to gather more readings for us
        gnss.setHNRNavigationRate(HNR_RATE);
        gnss.setNavigationFrequency(PVT_RATE);
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
        state = gnss.getGnssFixOk(HNR_MAX_WAIT);
        esfIns = gnss.getEsfIns(HNR_MAX_WAIT);
        hnrAtt = gnss.getHNRAtt(HNR_MAX_WAIT);

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

    data.latitude = gnss.getLatitude(PVT_MAX_WAIT);
    data.longitude = gnss.getLongitude(PVT_MAX_WAIT);
    
    data.roll = gnss.getHNRroll(HNR_MAX_WAIT);
    data.pitch = gnss.getHNRpitch(HNR_MAX_WAIT);
    data.yaw = gnss.getHNRheading(HNR_MAX_WAIT);

    return data;
}

GPSData GPS_GetHighRateLocationData(void) {
    GPSData data;
    
    data.roll = gnss.getHNRroll(HNR_MAX_WAIT);
    data.pitch = gnss.getHNRpitch(HNR_MAX_WAIT);
    data.yaw = gnss.getHNRheading(HNR_MAX_WAIT);

    return data;
}

GPSData GPS_GetFullData(void) {
    GPSData data;
    
    data.altitude = gnss.getAltitude(PVT_MAX_WAIT);
    data.latitude = gnss.getLatitude(PVT_MAX_WAIT);
    data.longitude = gnss.getLongitude(PVT_MAX_WAIT);

    data.year = gnss.getYear(PVT_MAX_WAIT);
    data.month = gnss.getMonth(PVT_MAX_WAIT);
    data.day = gnss.getDay(PVT_MAX_WAIT);
    data.hour = gnss.getHour(PVT_MAX_WAIT);
    data.minute = gnss.getMinute(PVT_MAX_WAIT);
    data.second = gnss.getSecond(PVT_MAX_WAIT);

    data.roll = gnss.getHNRroll(HNR_MAX_WAIT);
    data.pitch = gnss.getHNRpitch(HNR_MAX_WAIT);
    data.yaw = gnss.getHNRheading(HNR_MAX_WAIT);

    return data;
}
