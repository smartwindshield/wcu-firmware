#include "HAL_BT.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "btstack.h"
#include "pico/cyw43_arch.h"

/* 
 * This include is autogenerated by BTStack, it contains macros for the custom
 * GATT service characteristics (by their UUIDs).
 */
#include "smartwindshield-bt.h"

#define TEST_CHARACTERISTIC ATT_CHARACTERISTIC_90E4BA9D_227F_41A6_8D10_6D8187F4A770_01_VALUE_HANDLE

static uint8_t ADV_DATA[] = {
    // Flags general discoverable
    0x02, BLUETOOTH_DATA_TYPE_FLAGS, 0x02,
    
    // Name
    0x10, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME,
    'S', 'm', 'a', 'r', 't', 'W', 'i', 'n', 'd', 's', 'h', 'i', 'e', 'l', 'd',

    0x03, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x1a, 0x18,
};
static const uint8_t ADV_DATA_LEN = sizeof(ADV_DATA);

const uint8_t value = 0xAE;

extern uint8_t const profile_data[];

static btstack_timer_source_t heartbeat;
static btstack_packet_callback_registration_t hci_event_callback_registration;
static hci_con_handle_t con_handle;

static uint16_t att_read_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size) {
    UNUSED(connection_handle);

    if (att_handle == TEST_CHARACTERISTIC){
        return att_read_callback_handle_blob((const uint8_t *)&value, sizeof(value), offset, buffer, buffer_size);
    }
    return 0;
}

static int att_write_callback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size) {
    UNUSED(transaction_mode);
    UNUSED(offset);
    UNUSED(buffer_size);
    
    if (att_handle != TEST_CHARACTERISTIC) return 0;
    att_server_request_can_send_now_event(connection_handle);
    return 0;
}

void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    UNUSED(size);
    UNUSED(channel);
    bd_addr_t local_addr;
    if (packet_type != HCI_EVENT_PACKET) return;

    uint8_t event_type = hci_event_packet_get_type(packet);
    switch(event_type){
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) return;
            gap_local_bd_addr(local_addr);
            printf("BTstack up and running on %s.\n", bd_addr_to_str(local_addr));

            // setup advertisements
            uint16_t adv_int_min = 800;
            uint16_t adv_int_max = 800;
            uint8_t adv_type = 0;
            bd_addr_t null_addr;
            memset(null_addr, 0, 6);
            gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
            assert(ADV_DATA_LEN <= 31); // ble limitation
            gap_advertisements_set_data(ADV_DATA_LEN, (uint8_t*) ADV_DATA);
            gap_advertisements_enable(1);
            break;
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            //le_notification_enabled = 0;
            break;
        case ATT_EVENT_CAN_SEND_NOW:
            att_server_notify(con_handle, TEST_CHARACTERISTIC, (uint8_t*)&value, sizeof(value));
            break;
        default:
            break;
    }
}

void HAL_BT_Init(uint32_t heartbeatPeriod) {
    // initialize CYW43 driver architecture (will enable BT if/because CYW43_ENABLE_BLUETOOTH == 1)
    if (cyw43_arch_init()) {
        printf("failed to initialise cyw43_arch\n");
        // TODO: Do something with this
    }

    att_server_init(profile_data, att_read_callback, att_write_callback);    

    // inform about BTstack state
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    // register for ATT event
    att_server_register_packet_handler(packet_handler);

    // turn on bluetooth!
    hci_power_control(HCI_POWER_ON);    
}