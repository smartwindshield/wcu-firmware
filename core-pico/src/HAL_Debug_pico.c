#include "HAL_Debug.h"

#include "pico/stdlib.h"

#include <stdio.h>

void HAL_Debug_Printf(const char *fmt, void *stuff, ...) {
    va_list argptr;

    va_start(argptr, stuff);
    vprintf(fmt, argptr);
    va_end(argptr);
}
