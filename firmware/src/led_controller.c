#include "led_controller.h"
#include "sensors/barometer.h"
#include "solar.h"
#include "util.h"

#include "HAL_GPIO.h"
#include "HAL_Neopixels.h"
#include "HAL_Time.h"

// Two strips in series of 167 each
static const size_t NUM_PIXELS = 160;

// Update which LEDs are on/off at this interval
static const uint32_t LED_UPDATE_INTERVAL_MS = 1000;

/**
 * Set which LEDs are on/off in accordance to the data obtained
 * from the Solar module.
 */
static void SetLEDs(void);

void LEDController_Init(void) {    
    HAL_Neopixels_RGB color = {0, 0, 0};

    HAL_GPIO_Init();
    HAL_Neopixels_Init(NUM_PIXELS, HAL_PIN_LED_DATA);

    HAL_Neopixels_Reset();
    for (int i = 0; i < NUM_PIXELS; i++) {
        HAL_Neopixels_SetPixel(i, color);
    }
    HAL_Neopixels_Update();
}

void LEDController_Update(void) {
     // Persistant variable to keep track of the last time LEDs were updated
    static uint32_t lastUpdateCall = 0;

    uint32_t currentTime = HAL_Time_GetTimeNow();

    if (lastUpdateCall == 0 || currentTime - lastUpdateCall >= LED_UPDATE_INTERVAL_MS) {
        SetLEDs();
        lastUpdateCall = currentTime;
    }
}

static void SetLEDs(void) {
    static int i = 0;
    HAL_Neopixels_RGB color = {0, 0, 255};
    HAL_Neopixels_RGB color0 = {255, 0, 0};
    // TODO: Set which LEDs are on and off here

    Vector2D solarWindshieldPos = Solar_GetWindshieldRelativeIntersectionPoint();

    HAL_Neopixels_Reset();
    HAL_Neopixels_SetPixel(i, color0);
    HAL_Neopixels_SetPixel(++i, color);
    HAL_Neopixels_Update();

    if (i >= NUM_PIXELS) i = 0;
}
