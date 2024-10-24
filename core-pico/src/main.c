#include <stdio.h>
#include "pico/stdlib.h"

#include "pico/cyw43_arch.h"

#include "led_controller.h"
#include "comms/bluetooth.h"
#include "sensors/sensors_controller.h"

int main() {
    stdio_init_all();

    LEDController_Init();
    BluetoothComms_Init();
    SensorsController_Init();

    // Flash the network chip LED just to see something
    static int led_on = true;
    for (;;) {
        led_on = !led_on;
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
        sleep_ms(500);

        SensorsController_Update();
        LEDController_Update();
    }

    return 0;
}

