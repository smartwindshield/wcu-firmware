cmake_minimum_required(VERSION 3.24)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialize Pico SDK
include(pico_sdk_import.cmake)

project(wcu_firmware)

pico_sdk_init()

# Create a library containing the WCU firmware
# This will link to the Pico main.c implementation OR unit tests
add_library(wcu_firmware
    firmware/src/led_controller.c
)
target_include_directories(wcu_firmware PRIVATE 
    firmware/include
    HAL/include
)

add_executable(wcu_firmware_pico
    core-pico/src/main.c
    core-pico/src/HAL_GPIO_pico.c
)
target_include_directories(wcu_firmware_pico PRIVATE 
    firmware/include
    HAL/include
)

# Link Pico executable with firmware library and Pico stdlib
target_link_libraries(wcu_firmware_pico wcu_firmware pico_stdlib)

# Generate hex file
pico_add_extra_outputs(wcu_firmware_pico)
