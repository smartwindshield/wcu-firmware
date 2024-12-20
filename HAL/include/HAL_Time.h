#ifndef HAL_TIME_H
#define HAL_TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Returns the number milliseconds since the start of the program.
 */
uint32_t HAL_Time_GetTimeNow();

/**
 * Sleep for the specified number of milliseconds.
 */
void HAL_Time_Sleep(uint32_t ms);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HAL_TIME_H
