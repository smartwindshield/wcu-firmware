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

    // Turn on LED after all modules initialized
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    
    for (;;) {
        SensorsController_Update();
        LEDController_Update();
    }

    return 0;
}

