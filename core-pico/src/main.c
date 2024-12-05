#include "HAL_Time.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/time.h"

#include "storage.h"
#include "led_controller.h"
#include "comms/bluetooth.h"
#include "sensors/gps.h"
#include "sensors/sensors_controller.h"
#include "wcu_firmware_config.h"

#include <hardware/timer.h>
#include <pico/stdio.h>
#include <pico/types.h>
#include <stdnoreturn.h>

#define STARTUP_DELAY 1000

// Make this a power of 2 for quick division
#define NUM_TIMING_SAMPLES 8

static const int ERROR_NO_GPS_LED_PERIOD = 100;

// Interval to print out performance timing information, in us
static const uint64_t TIMING_OUTPUT_INTERVAL = 5000 * 1000;

static noreturn void errorHandler(int period) {
    printf("[errorHandler]: Error Handler invoked\n");

    for (;;) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(period);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(period);
    }
}

/**
 * Calculates the average time the main loop takes to run.
 *
 * Stores a certain amount of samples, and then averages them.
 */
static void measureTiming(uint64_t startTime) {
    static uint32_t lastTimingOutput = 0;

    static uint64_t measurements[NUM_TIMING_SAMPLES] = {0, };
    static int currentIndex = 0;
    static uint64_t sumTime = 0;
    uint32_t currentTiming = 0;
    uint32_t averageTime = 0;

    uint64_t currentTime = time_us_64();

    currentTiming = currentTime - startTime;

    measurements[currentIndex] = currentTiming;
    sumTime += measurements[currentIndex];

    if (++currentIndex >= NUM_TIMING_SAMPLES) {
        averageTime = sumTime / NUM_TIMING_SAMPLES;
        
        // Only print out the timing data every x useconds
        if (currentTime - lastTimingOutput >= TIMING_OUTPUT_INTERVAL) {
            printf("[Performance]: Latest timing: %d us, average: %d us.\n", 
                    currentTiming,
                    averageTime);

            lastTimingOutput = currentTime;
        }

        currentIndex = 0;
        sumTime = 0;
    }
}

int main() {
    bool statusLEDState = true;
    uint64_t startTime;

    stdio_init_all();

#if STARTUP_DELAY
    sleep_ms(STARTUP_DELAY); // wait for usb serial, sensors to initialize
#endif

    // Display cool startup messages to UART
    printf("----------------------------------------------------------------------------\n");
    stdio_flush();
    printf("%s\n", WCU_FIRMWARE_FANCY_NAME);
    stdio_flush();
    printf("%s Revision (Major.Minor): %i.%i\n",
            WCU_FIRMWARE_NAME, WCU_FIRMWARE_REV_MAJOR, WCU_FIRMWARE_REV_MINOR);
    printf("%s\n", WCU_FIRMWARE_AUTHORS);
    printf("----------------------------------------------------------------------------\n");
    
    printf("[main]: Initializing modules...\n");

    Storage_Init();
    LEDController_Init();
    BluetoothComms_Init();
    SensorsController_Init();

    printf("[main]: Modules initialized...\n");

    // Turn on LED after all modules initialized and has GPS connectivity
#ifdef LOCATION_PROVIDER_ONBOARD_GPS
    if (GPS_HasChipConnectivity()) {
        printf("[main]: GPS Module reports connectivity.\n");
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    } else {
        // Blink LED to show we have no GPS connectivity
        printf("[main]: GPS Module reports no connectivity.\n");
        errorHandler(ERROR_NO_GPS_LED_PERIOD);
    }
#endif // LOCATION_PROVIDER_ONBOARD_GPS
    
    for (;;) {
        startTime = time_us_64();

        BluetoothComms_Update();
        SensorsController_Update();
        LEDController_Update();

        measureTiming(startTime);

        // Blinking the LED to show activity of the main loop
        statusLEDState = !statusLEDState;
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, statusLEDState);
    }

    return 0;
}

