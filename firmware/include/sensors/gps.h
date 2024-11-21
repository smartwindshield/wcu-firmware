#ifndef GPS_H
#define GPS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct GPSData_T {
    uint32_t altitude;
    uint32_t latitude;
    uint32_t longitude;

    // Vehicle roll and pitch
    int32_t roll;
    int32_t pitch;
    
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

GPSData GPS_GetData(void);

#ifdef __cplusplus
}
#endif

#endif // GPS_H
