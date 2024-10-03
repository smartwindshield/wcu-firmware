#include <stdio.h>
#include "pico/stdlib.h"

#include "led_controller.h"

int main() {
    stdio_init_all();

    LEDController_Init();

    return 0;
}

