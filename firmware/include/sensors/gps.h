#ifndef GPS_H
#define GPS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct GPSData_T {
    uint32_t altitude;
    // Lat/long in units of 10^-7
    uint32_t latitude;
    uint32_t longitude;

    // Vehicle roll and pitch and yaw in units of degrees
    float roll;
    float pitch;
    float yaw;
    
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} GPSData;

#ifdef __cplusplus
extern "C" {
#endif

void GPS_Init(void);
void GPS_Update(void);

/**
 * Returns true if the GPS driver has successfully communicated with
 * the U-blox chip.
 */ 
bool GPS_HasChipConnectivity(void);

/**
 * Populates all fields except the date and time related ones.
 */
GPSData GPS_GetLocationData(void);
/**
 * Only populates roll, pitch and yaw.
 */
GPSData GPS_GetHighRateLocationData(void);
/**
 * Populates all fields.
 */
GPSData GPS_GetFullData(void);

#ifdef __cplusplus
}
#endif

#endif // GPS_H
