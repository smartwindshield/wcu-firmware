#ifndef GPS_H
#define GPS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void GPS_Init(void);

void GPSDataUpdate(uint8_t[]);

#ifdef __cplusplus
}
#endif

#endif // GPS_H
