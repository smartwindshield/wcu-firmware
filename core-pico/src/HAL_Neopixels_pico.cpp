#include "HAL_Neopixels.h"
#include "HAL_GPIO_pico.h"

#include "Adafruit_NeoPixel.hpp"

static Adafruit_NeoPixel pixels;

// HAL functions are for C, so give them C linkage
extern "C" {
    void HAL_Neopixels_Init(size_t num_pixels, unsigned int pin) {
        assert(pin < HAL_TO_PICO_PIN_MAPPINGS[pin]);

        pixels = Adafruit_NeoPixel(num_pixels, HAL_TO_PICO_PIN_MAPPINGS[pin], NEO_GRB + NEO_KHZ800);
        pixels.begin();
        pixels.clear();
        pixels.show();
    }

    void HAL_Neopixels_SetPixel(unsigned int pixel, HAL_Neopixels_RGB color) {
        pixels.setPixelColor(pixel, pixels.Color(color.r, color.g, color.b));
        pixels.show();
    }
}
