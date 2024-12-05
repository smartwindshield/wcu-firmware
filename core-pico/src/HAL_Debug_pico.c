#include "HAL_Debug.h"

#include "wcu_firmware_config.h"

#include "pico/stdlib.h"

#include <stdio.h>

void HAL_Debug_Printf(const char *fmt, ...) {
#ifdef DEBUG_PRINTF_ENABLED
    va_list argptr;

    va_start(argptr, fmt);
    vprintf(fmt, argptr);
    va_end(argptr);
#endif
}
