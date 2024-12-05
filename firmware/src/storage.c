#include "storage.h"

void Storage_Init(void) {
    // TODO: EEPROM
}

uint32_t Storage_Retrieve(uint32_t id) {
    uint32_t value;

    // TODO: Load data from EEPROM. For now, hardcoded values
    if (id == STORAGE_ID_WINDSHIELD_HEIGHT) {
        value = 457; // 457.2 mm
    } else if (id == STORAGE_ID_WINDSHIELD_WIDTH) {
        value = 610; // 609.6 mm
    } else {
        value = 0;
    }

    return value;
}

void Storage_Store(uint32_t id, uint32_t value) {
    // TODO: Store data in EEPROM. for now, do nothing
}
