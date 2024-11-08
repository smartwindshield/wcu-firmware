#ifndef GPS_H
#define GPS_H

#include <stdint.h>

typedef struct GPSData_T {
    uint32_t altitude;
    uint32_t latitude;
    uint32_t longitude;
    // TODO: add more fields for GPS data we need
} GPSData;

#ifdef __cplusplus
extern "C" {
#endif

void GPS_Init(void);
void GPS_Update(void);

GPSData GPS_GetData(void);

#ifdef __cplusplus
}
#endif

#endif // GPS_H
