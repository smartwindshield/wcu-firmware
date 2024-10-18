#include "comms/bluetooth.h"

#include "HAL_BT.h"

void BluetoothComms_Init(void) {
    HAL_BT_Init(25);
}
