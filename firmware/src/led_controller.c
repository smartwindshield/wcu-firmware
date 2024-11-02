#include "led_controller.h"
#include "HAL_GPIO.h"
#include "HAL_Neopixels.h"

// Two strips in series of 167 each
static const size_t NUM_PIXELS = 167 * 2;

void LEDController_Init(void) {    
    HAL_Neopixels_RGB color = {255, 255, 255};

    HAL_GPIO_Init();
    HAL_Neopixels_Init(NUM_PIXELS, HAL_PIN_LED_DATA);
    
    HAL_Neopixels_SetPixel(1, color);
}

void LEDController_Update(void) {
    
}
