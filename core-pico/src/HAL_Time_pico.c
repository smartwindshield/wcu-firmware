#include "HAL_Time.h"

#include "pico/stdlib.h"
#include "pico/time.h"
#include <pico/types.h>

uint32_t HAL_Time_GetTimeNow(void) {
    uint64_t usSinceBoot = time_us_64();
    absolute_time_t absTime = from_us_since_boot(usSinceBoot);
    
    return to_ms_since_boot(absTime);
}

void HAL_Time_Sleep(uint32_t ms) {
    sleep_ms(ms);
}
