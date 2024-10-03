# wcu-firmware
Windshield Control Unit (WCU) firmware

## Structure
There are three parts to the firmware:

- HAL Headers (Hardware Abstraction Layer)
- Firmware (interacts with HW using HAL-defined functions)
- pico-core (Contains main.c, calls firmware code, implements HAL-defined functions)

The HAL-defined functions will allow interacting with GPIO, I2C, etc.
In order for the firmware to compile for desktop, and allow testing with dummy code, the HAL functions let the code build without the pico-sdk.
Thus, a dummy implementation of HAL functions can be linked to the firmware, allowing it to be tested on PC.

When compiling for the Pico, the pico-core directory contains implementations of the HAL functions that will call the corresponding pico-sdk functiosn for GPIO, I2C, etc. These, and ```main.c``` will link to the firmware library.

