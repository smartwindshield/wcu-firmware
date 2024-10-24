#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

void LEDController_Init(void);

/**
 * Called once per iteration of the main loop.
 * Should update which LEDs are on based on algorithm output.
 */
void LEDController_Update(void);

#endif // LED_CONTROLLER_H
