#include "HAL_GPIO.h"
#include "HAL_GPIO_pico.h"

#include "hardware/gpio.h"

#include <stddef.h>

void HAL_GPIO_Init(void) {
    static bool initialized = false;

    // GPIO pins setup
    if (!initialized) {
        gpio_init(HAL_TO_PICO_PIN_MAPPINGS[HAL_PIN_BARO_INT]);
        gpio_init(HAL_TO_PICO_PIN_MAPPINGS[HAL_PIN_GPS_NRST]);
        gpio_init(HAL_TO_PICO_PIN_MAPPINGS[HAL_PIN_IMU_NRST]);
        gpio_init(HAL_TO_PICO_PIN_MAPPINGS[HAL_PIN_IMU_INTN]);
        gpio_init(HAL_TO_PICO_PIN_MAPPINGS[HAL_PIN_LED_DATA]);

        gpio_set_dir(HAL_TO_PICO_PIN_MAPPINGS[HAL_PIN_BARO_INT], GPIO_IN);
        gpio_set_dir(HAL_TO_PICO_PIN_MAPPINGS[HAL_PIN_GPS_NRST], GPIO_OUT);
        gpio_set_dir(HAL_TO_PICO_PIN_MAPPINGS[HAL_PIN_IMU_INTN], GPIO_IN);
        gpio_set_dir(HAL_TO_PICO_PIN_MAPPINGS[HAL_PIN_IMU_NRST], GPIO_OUT);
        gpio_set_dir(HAL_TO_PICO_PIN_MAPPINGS[HAL_PIN_LED_DATA], GPIO_OUT);

        initialized = true;
    }
}

void HAL_GPIO_SetPin(unsigned int pin, bool state) {
    assert(pin < HAL_TO_PICO_PIN_MAPPINGS_SIZE);

    gpio_put(HAL_TO_PICO_PIN_MAPPINGS[pin], state);
}

bool HAL_GPIO_GetPin(unsigned int pin) {
    assert(pin < HAL_TO_PICO_PIN_MAPPINGS_SIZE);

    return gpio_get(HAL_TO_PICO_PIN_MAPPINGS[pin]);
}
