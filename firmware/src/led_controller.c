#include "led_controller.h"
#include "sensors/barometer.h"
#include "solar.h"
#include "util.h"

#include "HAL_GPIO.h"
#include "HAL_Neopixels.h"
#include "HAL_Time.h"
#include "HAL_Debug.h"

#include <math.h>

// 18 LEDs per row, 14 rows
static const size_t LEDS_PER_ROW = 18;
static const size_t NUM_ROWS = 13;

// Total number of LEDs
static const size_t NUM_PIXELS = LEDS_PER_ROW * NUM_ROWS;

static const HAL_Neopixels_RGB SUN_LED_COLOR = {255, 0, 0};
static const HAL_Neopixels_RGB NO_INTERSECT_COLOR = {0, 255, 0};

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

static int map_to_led_index(double x, double y, int rows, int cols) {
    // Ensure x and y are in the range [-1, 1]
    if (x < -1 || x > 1 || y < -1 || y > 1) {
        HAL_Debug_Printf("[LED]: Error: Coordinates out of range.\n");
        return -1;
    }
    // Convert relative coordinates to grid indices
    int col_index = (int)round((x + 1) / 2 * (cols - 1)); // Map x to column (0 to cols-1)
    int row_index = (int)round((y + 1) / 2 * (rows - 1)); // Map y to row (0 to rows-1)
    // Handle snaking pattern for LED mapping
    if (row_index % 2 == 1) {
        // Odd rows reverse the column order
        col_index = cols - 1 - col_index;
    }
    // Compute the LED index
    int led_index = row_index * cols + col_index;
    return led_index;
}

static void SetLEDs(void) {
    // Old LED snake color code (for LED strip testing)
#if 0
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
#else
    int ledIndex;
    Vector2D solarWindshieldPos;

    bool intersected = Solar_GetWindshieldRelativeIntersectionPoint(&solarWindshieldPos);

    HAL_Neopixels_Reset();
    if (intersected) {
        ledIndex = map_to_led_index(solarWindshieldPos.x, solarWindshieldPos.y, NUM_ROWS, LEDS_PER_ROW);

        HAL_Neopixels_SetPixel(ledIndex, SUN_LED_COLOR);
    } else {
        HAL_Neopixels_SetPixel(1, NO_INTERSECT_COLOR);
    }

    HAL_Neopixels_Update();
#endif
}
