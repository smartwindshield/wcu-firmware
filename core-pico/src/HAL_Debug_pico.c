#include "HAL_Debug.h"

#include "pico/stdlib.h"

#include <stdio.h>

void HAL_Debug_Printf(const char *fmt, ...) {
    va_list argptr;

    va_start(argptr, fmt);
    vprintf(fmt, argptr);
    va_end(argptr);
}
