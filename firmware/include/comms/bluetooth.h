#ifndef COMMS_BLUETOOTH_H
#define COMMS_BLUETOOTH_H

#include "util.h"

void BluetoothComms_Init(void);
void BluetoothComms_Update(void);

float BluetoothComms_GetWindshieldLength(void);
float BluetoothComms_GetWindshieldHeight(void);
Vector3 BluetoothComms_GetUserWindshieldDistance(void);

#endif // COMMS_BLUETOOTH_H
