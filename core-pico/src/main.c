#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/time.h"

#include "led_controller.h"
#include "comms/bluetooth.h"
#include "sensors/gps.h"
#include "sensors/sensors_controller.h"

#include <stdnoreturn.h>

#define STARTUP_DELAY 1000

static const int ERROR_NO_GPS_LED_PERIOD = 200;

static noreturn void errorHandler(int period) {
    printf("[errorHandler]: Error Handler invoked\n");

    for (;;) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(period);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(period);
    }
}

int main() {
    stdio_init_all();

#if STARTUP_DELAY
    sleep_ms(STARTUP_DELAY); // wait for usb serial, sensors to initialize
#endif

    printf("[main]: Initializing modules...\n");

    LEDController_Init();
    BluetoothComms_Init();
    SensorsController_Init();

    printf("[main]: Modules initialized...\n");

    // Turn on LED after all modules initialized and has GPS connectivity
    if (GPS_HasChipConnectivity()) {
        printf("[main]: GPS Module reports connectivity.\n");
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    } else {
        // Blink LED to show we have no GPS connectivity
        printf("[main]: GPS Module reports no connectivity.\n");
        errorHandler(ERROR_NO_GPS_LED_PERIOD);
    }
    
    for (;;) {
        SensorsController_Update();
        LEDController_Update();
    }

    return 0;
}

