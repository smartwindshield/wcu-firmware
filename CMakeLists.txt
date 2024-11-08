cmake_minimum_required(VERSION 3.24)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w)

# Initialize Pico SDK
include(pico_sdk_import.cmake)

project(wcu_firmware)

pico_sdk_init()

add_subdirectory(pi-pico-adafruit-neopixels/pico_neopixels)
add_subdirectory(SparkFun_u-blox_GNSS_Pico_library)

# Create a library containing the WCU firmware
# This will link to the Pico main.c implementation OR unit tests
add_library(wcu_firmware
    firmware/src/led_controller.c
    firmware/src/comms/bluetooth.c
    firmware/src/sensors/sensors_controller.c
    firmware/src/sensors/barometer.c
    firmware/src/sensors/gps.cpp
    firmware/src/sensors/power_delivery.c
)
target_include_directories(wcu_firmware PRIVATE 
    firmware/include
    HAL/include
    SparkFun_u-blox_GNSS_Pico_library/include
)

add_executable(wcu_firmware_pico
    core-pico/src/main.c
    core-pico/src/HAL_GPIO_pico.c
    core-pico/src/HAL_I2C_pico.c
    core-pico/src/HAL_CPP_I2C_pico.cpp
    core-pico/src/HAL_BT_pico.c
    core-pico/src/HAL_Neopixels_pico.cpp
    core-pico/src/HAL_Debug_pico.c
)
target_include_directories(wcu_firmware_pico PRIVATE 
    firmware/include
    HAL/include
    core-pico/include
    pi-pico-adafruit-neopixels/pico_neopixels/include
    SparkFun_u-blox_GNSS_Pico_library/include
)

# Link Pico executable with firmware library and Pico stdlib
target_link_libraries(wcu_firmware_pico 
    wcu_firmware
    pico_neopixel
    sparkfun_ublox_gnss_pico 
    # Pico Standard Library stuff:
    pico_stdlib
    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_none
    hardware_i2c
)

# Generate our GATT header (for Bluetooth LE)
pico_btstack_make_gatt_header(wcu_firmware PRIVATE "${CMAKE_CURRENT_LIST_DIR}/firmware/smartwindshield-bt.gatt")

pico_enable_stdio_usb(wcu_firmware_pico 1)
pico_enable_stdio_uart(wcu_firmware_pico 0)

# Generate hex file
pico_add_extra_outputs(wcu_firmware_pico)
