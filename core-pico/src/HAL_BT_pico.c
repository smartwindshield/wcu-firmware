/*
 * This file is based on the Pico W SDK Bluetooth example code,
 * although heavily modified.
 *
 * @see https://github.com/raspberrypi/pico-examples/blob/master/pico_w/bt/standalone/server_common.c
 */
#include "HAL_BT.h"

#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "btstack.h"
#include "l2cap.h"
#include "pico/cyw43_arch.h"

// Bluetooth LE Limitation
#define MAX_ADVERTISEMENT_DATA_SIZE 31

static const uint8_t ADV_DATA_SUFFIX[] = {
    0x03, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x1a, 0x18,
};
static const size_t ADV_DATA_SUFFIX_SIZE = sizeof(ADV_DATA_SUFFIX);
static const size_t ADV_DATA_PREFIX_SIZE = 3;
static const size_t MAX_DEVICE_NAME_SIZE = MAX_ADVERTISEMENT_DATA_SIZE - 3 - ADV_DATA_SUFFIX_SIZE;

/**
 * Represents a characteristic stored in memory
 */
typedef struct Characteristic_T {
    bool registered;
    uint8_t id;
    uint8_t value[HAL_BT_MAX_CHARACTERISTIC_SIZE];
    size_t size;
} Characteristic;

static uint8_t advertisement_data[MAX_ADVERTISEMENT_DATA_SIZE] = {
    // Flags general discoverable
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, 0x06

    // The rest of this data will be filled in GenerateAdvertisementData()
};

// Set in GenerateAdvertisementData()
static size_t advertisement_data_size = 3;

static btstack_packet_callback_registration_t hci_event_callback_registration;
static hci_con_handle_t con_handle;

/**
 * Stores the values of all registered characteristics.
 * When a characteristic is registered 
 */
static Characteristic characteristics[HAL_BT_MAX_CHARACTERISTICS];

static void GenerateAdvertisementData(const char *device_name) {
    int i, j;
    int device_name_len = strlen(device_name);
    assert(device_name_len <= MAX_DEVICE_NAME_SIZE);

    // Prefix the device name string with it's length and type (required)
    // + 1 to the length for the data type byte
    advertisement_data[ADV_DATA_PREFIX_SIZE] = device_name_len + 1; 
    advertisement_data[ADV_DATA_PREFIX_SIZE + 1] = BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME;

    // Copy the device name into the advertisement data after the prefix
    for(i = 0, j = ADV_DATA_PREFIX_SIZE + 2; i < device_name_len; i++, j++) {
        advertisement_data[j] = device_name[i];
    }

    for(i = 0; i < ADV_DATA_SUFFIX_SIZE; i++, j++) {
        advertisement_data[j] = ADV_DATA_SUFFIX[i];
    }

    advertisement_data_size = j;
}

static uint16_t att_read_callback(hci_con_handle_t connection_handle, 
                                  uint16_t att_handle, uint16_t offset,
                                  uint8_t * buffer, uint16_t buffer_size) {
    UNUSED(connection_handle);
    Characteristic *ch;

    if (att_handle < HAL_BT_MAX_CHARACTERISTICS){
        ch = &characteristics[att_handle];

        // Send the value we have stored currently
        if (ch->registered) {
            return att_read_callback_handle_blob(ch->value, ch->size, offset, buffer, buffer_size);
        }
    }

    return 0;
}

static int att_write_callback(hci_con_handle_t connection_handle, 
                              uint16_t att_handle, uint16_t transaction_mode,
                              uint16_t offset, uint8_t *buffer, uint16_t buffer_size) {
    UNUSED(transaction_mode);
    UNUSED(offset);
    UNUSED(buffer_size);
    Characteristic *ch;

    if (att_handle < HAL_BT_MAX_CHARACTERISTICS) {
        ch = &characteristics[att_handle];

        // Save the new value into memory
        if (ch->registered && buffer_size <= ch->size) {
            memcpy(ch->value, buffer, buffer_size);

            att_server_request_can_send_now_event(connection_handle);
        }
    }
    
    return 0;
}

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(size);
    UNUSED(channel);
    bd_addr_t null_addr;
    uint8_t event_type;

    // setup advertisements
    uint16_t adv_int_min = 800;
    uint16_t adv_int_max = 800;
    uint8_t adv_type = 0;

    if (packet_type == HCI_EVENT_PACKET) {
        event_type = hci_event_packet_get_type(packet);
        switch(event_type){
            case BTSTACK_EVENT_STATE:
                if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
                    memset(null_addr, 0, 6);

                    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
                    gap_advertisements_set_data(advertisement_data_size, advertisement_data);
                    gap_advertisements_enable(1);
                }
                break;
            case HCI_EVENT_DISCONNECTION_COMPLETE:
                //le_notification_enabled = 0;
                break;
            case ATT_EVENT_CAN_SEND_NOW:
                //att_server_notify(con_handle, CHARACTERISTIC_DRIVER_HEIGHT, (uint8_t*)&value, sizeof(value));
                break;
            default:
                break;
        }
    }
}

void HAL_BT_Init(const char *device_name, const uint8_t *profile_data) {
    GenerateAdvertisementData(device_name);

    // Clear characteristic storage
    memset(characteristics, 0, HAL_BT_MAX_CHARACTERISTICS);

    // initialize CYW43 driver architecture (will enable BT if/because CYW43_ENABLE_BLUETOOTH == 1)
    if (cyw43_arch_init()) {
        printf("failed to initialise cyw43_arch\n");
        assert(0);
    }

    l2cap_init();
    sm_init();

    att_server_init(profile_data, att_read_callback, att_write_callback);    

    // inform about BTstack state
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    // register for ATT event
    att_server_register_packet_handler(packet_handler);

    // turn on bluetooth!
    hci_power_control(HCI_POWER_ON);    
}

int HAL_BT_RegisterCharacteristic(uint8_t id, size_t size) {
    int result = 0;

    if (id >= HAL_BT_MAX_CHARACTERISTICS) {
        result = -1;
    } else {
        characteristics[id].registered = true;
        // Clear the value in case something was there previously
        memset(characteristics[id].value, 0, HAL_BT_MAX_CHARACTERISTIC_SIZE);
        characteristics[id].size = size;
    }

    return result;
}

size_t HAL_BT_GetCharacteristic(uint8_t id, uint8_t *value) {
    int result_len;

    assert(value != NULL);

    if (id >= HAL_BT_MAX_CHARACTERISTICS || !characteristics[id].registered) {
        result_len = -1;
    } else {
        result_len = characteristics[id].size;
        memcpy(value, characteristics[id].value, result_len);
    }

    return result_len;
}

void HAL_BT_SetCharacteristic(uint8_t id, uint8_t *value, size_t size) {
   assert(value != NULL);

   if (id < HAL_BT_MAX_CHARACTERISTICS && characteristics[id].registered
        && size <= characteristics[id].size) {
       memcpy(characteristics[id].value, value, size);
   }
}
