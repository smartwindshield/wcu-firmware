#ifndef COMMS_BLUETOOTH_H
#define COMMS_BLUETOOTH_H

#include "util.h"

void BluetoothComms_Init(void);

uint32_t BluetoothComms_GetWindshieldLength(void);
uint32_t BluetoothComms_GetWindshieldHeight(void);
Vector3 BluetoothComms_GetUserWindshieldDistance(void);

#endif // COMMS_BLUETOOTH_H
