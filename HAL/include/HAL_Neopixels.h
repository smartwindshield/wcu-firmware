#ifndef HAL_NEOPIXELS_H
#define HAL_NEOPIXELS_H

#include <stddef.h>
#include <stdint.h>

typedef struct HAL_Neopixels_RGB_T {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} HAL_Neopixels_RGB;


// Declare these functions with C linkage if a C++ compiler is reading this file
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
    void HAL_Neopixels_Init(size_t num_pixels, unsigned int pin);

    void HAL_Neopixels_SetPixel(unsigned int pixel, HAL_Neopixels_RGB color);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HAL_NEOPIXELS_H
