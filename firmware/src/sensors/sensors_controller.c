#include "sensors/sensors_controller.h"
#include "HAL_Time.h"
#include "sensors/external_temp.h"
#include "sensors/gps.h"
#include "sensors/light.h"
#include "sensors/barometer.h"

#include "wcu_firmware_config.h"

#include "HAL_I2C.h"
#include "HAL_Debug.h"

#include <math.h>

static const uint32_t GPS_HIGHRATE_REFRESH_INTERVAL = 200;
static const uint32_t GPS_REFRESH_INTERVAL = 2000;
static const uint32_t GPS_DATETIME_REFRESH_INTERVAL = 1000 * 60; // Refresh time every 60 seconds
static const uint32_t SENSORS_REFRESH_INTERVAL = 5000; 

static LocationData locationCache;
static DatetimeData datetimeCache;

static double pressureCache;
static double temperatureCache;

static void refreshGpsData(uint32_t currentTime) {
    static uint32_t lastLocationRefresh = 0;
    static uint32_t lastDatetimeRefresh = 0;
    static uint32_t lastHighRateRefresh = 0;
    GPSData gps;

    GPS_Update();

    // Refresh location, datetime caches
    if(currentTime - lastHighRateRefresh >= GPS_HIGHRATE_REFRESH_INTERVAL) {
#ifdef DATETIME_PROVIDER_ONBOARD_GPS
        if (lastDatetimeRefresh == 0 || currentTime - lastDatetimeRefresh >= GPS_DATETIME_REFRESH_INTERVAL) {
            gps = GPS_GetFullData();

            datetimeCache.year = gps.year;
            datetimeCache.month = gps.month;
            datetimeCache.day = gps.day;
            datetimeCache.hour = gps.hour;
            datetimeCache.minute = gps.minute;
            datetimeCache.second = gps.second;

            HAL_Debug_Printf("[SensorsController]: Datetime (GPS) Cache Update: %i/%i/%i %i:%i:%i\n", 
                         datetimeCache.year, datetimeCache.month, datetimeCache.day,
                         datetimeCache.hour, datetimeCache.minute, datetimeCache.second);

            lastDatetimeRefresh = currentTime;
        }
#else
        if (0) { }
#endif // DATETIME_PROVIDER_ONBOARD_GPS
       
        /*
         * Latitude/longitude and yaw/pitch refresh
         */
        else if (currentTime - lastLocationRefresh >= GPS_REFRESH_INTERVAL) {
            gps = GPS_GetLocationData();
            
            locationCache.latitude = (double) gps.latitude / (double) pow(10, 7);
            locationCache.longitude = (double) gps.longitude / (double) pow(10, 7);
            locationCache.yaw = gps.yaw;
            locationCache.pitch = gps.pitch;

            lastLocationRefresh = currentTime;

            HAL_Debug_Printf("[SensorsController]: Location (GPS) Data Cache Update: ");
            HAL_Debug_Printf("(Lat/Long: %f and %f) (Yaw/Pitch: %f and %f)\n",
                             locationCache.latitude, locationCache.longitude,
                             locationCache.yaw, locationCache.pitch);
        } else {
            // Just high rate data (yaw and pitch) refresh
            gps = GPS_GetHighRateLocationData();

            locationCache.yaw = gps.yaw;
            locationCache.pitch = gps.pitch;
        }

        if (currentTime - lastHighRateRefresh >= GPS_HIGHRATE_REFRESH_INTERVAL + 50) {
            HAL_Debug_Printf("[SensorsController]: HighRate Refresh Interval Exceeded: %i ms\n",
                             currentTime - lastHighRateRefresh);
        }

        lastHighRateRefresh = currentTime;
    }
}

void SensorsController_Init(void) {
    HAL_I2C_Init(SENSORS_I2C_BUS, SENSORS_I2C_BAUD_RATE);

#ifdef LOCATION_PROVIDER_ONBOARD_GPS
    GPS_Init();
#else 
    HAL_Debug_Printf("[SensorsController]: No Location or Datetime provider configured!\n");
#endif

    LightSensor_Init();
    BarometerInit();

    SensorsController_Update();
}

void SensorsController_Update(void) {
    static uint32_t lastSensorRefresh = 0;
    uint32_t currentTime = HAL_Time_GetTimeNow();
#ifdef LOCATION_PROVIDER_ONBOARD_GPS
    refreshGpsData(currentTime);
#else 
    // Dummy Location and datetime data
#endif

    // Refresh temperature and pressure caches
    if (currentTime - lastSensorRefresh >= SENSORS_REFRESH_INTERVAL) {
        pressureCache = BarometerGetData() / 10; // Convert to kPa
        temperatureCache = External_Temp_GetData() + 273; // Convert to kelvin

        lastSensorRefresh = currentTime;
    }
}

LocationData SensorsController_GetLocationData(void) {
    return locationCache;
}

DatetimeData SensorsController_GetDatetimeData(void) {
    return datetimeCache;
}

double SensorsController_GetPressureKpa(void) {
    return pressureCache;
}
double SensorsController_GetTemperatureKelvin(void) {
    return temperatureCache;
}
