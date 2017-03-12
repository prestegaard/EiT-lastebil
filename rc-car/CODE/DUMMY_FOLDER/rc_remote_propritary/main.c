/* Copyright (c) 2014 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "sdk_common.h"
#include "nrf.h"
#include "nrf_esb.h"
#include "nrf_error.h"
#include "nrf_esb_error_codes.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_delay.h"
#include "app_util.h"
#define NRF_LOG_MODULE_NAME "REMOTE"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "rc_saadc.h"
#include "rc_utilities.h"
#include "rc_message_and_defines.h"

static nrf_esb_payload_t        tx_payload = NRF_ESB_CREATE_PAYLOAD(0, 0x01, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00);

static nrf_esb_payload_t        rx_payload;

/*lint -save -esym(40, BUTTON_1) -esym(40, BUTTON_2) -esym(40, BUTTON_3) -esym(40, BUTTON_4) -esym(40, LED_1) -esym(40, LED_2) -esym(40, LED_3) -esym(40, LED_4) */


static remote_packet_t msg;
static uint8_t STATE;

void nrf_esb_event_handler(nrf_esb_evt_t const * p_event)
{
    switch (p_event->evt_id)
    {
        case NRF_ESB_EVENT_TX_SUCCESS:
            NRF_LOG_DEBUG("TX SUCCESS EVENT\r\n");
            break;
        case NRF_ESB_EVENT_TX_FAILED:
            NRF_LOG_DEBUG("TX FAILED EVENT\r\n");
            (void) nrf_esb_flush_tx();
            (void) nrf_esb_start_tx();
            break;
        case NRF_ESB_EVENT_RX_RECEIVED:
            NRF_LOG_DEBUG("RX RECEIVED EVENT\r\n");
            while (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS)
            {
                if (rx_payload.length > 0)
                {
                    NRF_LOG_DEBUG("RX RECEIVED PAYLOAD\r\n");
                }
            }
            break;
    }
    NRF_GPIO->OUTCLR = 0xFUL << 12;
    NRF_GPIO->OUTSET = (p_event->tx_attempts & 0x0F) << 12;
}


void clocks_start( void )
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}



// Changes made:
/*
* added payload.length = 11 instead of default config 32
* changed to PROTOCOL_ESB, not PROTOCOL_ESB_DPL (dynamig payload length)
* added typedef remote_packet struct msg

*/
uint32_t esb_init( void )
{
    uint32_t err_code;
    uint8_t base_addr_0[4] = {0xE7, 0xE7, 0xE7, 0xE7};
    uint8_t base_addr_1[4] = {0xC2, 0xC2, 0xC2, 0xC2};
    uint8_t addr_prefix[8] = {0xE7, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8 };

    nrf_esb_config_t nrf_esb_config         = NRF_ESB_DEFAULT_CONFIG;
    nrf_esb_config.protocol                 = NRF_ESB_PROTOCOL_ESB;
    nrf_esb_config.retransmit_delay         = 600;
    nrf_esb_config.bitrate                  = NRF_ESB_BITRATE_2MBPS;
    nrf_esb_config.event_handler            = nrf_esb_event_handler;
    nrf_esb_config.mode                     = NRF_ESB_MODE_PTX;
    nrf_esb_config.selective_auto_ack       = false;
    nrf_esb_config.payload_length           = 11;

    err_code = nrf_esb_init(&nrf_esb_config);

    VERIFY_SUCCESS(err_code);

    err_code = nrf_esb_set_base_address_0(base_addr_0);
    VERIFY_SUCCESS(err_code);

    err_code = nrf_esb_set_base_address_1(base_addr_1);
    VERIFY_SUCCESS(err_code);

    err_code = nrf_esb_set_prefixes(addr_prefix, 8);
    VERIFY_SUCCESS(err_code);

    return err_code;
}





int main(void)
{
    ret_code_t err_code;

    gpio_init();

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    clocks_start();

    err_code = esb_init();
    APP_ERROR_CHECK(err_code);

    leds_init();
    buttons_init();
    joystick_init();

    NRF_LOG_DEBUG("EiT Remote\r\n");

    // Wait until user presses button and selects sender ID
    STATE = STATE_WAIT_FOR_CHANNEL_SELECT;
        while(!get_pressed_button()){
            // NOP
        }
        msg.senderID = get_pressed_button();
        set_led(msg.senderID);
        NRF_LOG_DEBUG("SenderID is selected: %d\r\n", msg.senderID);
    // Wait until remote reveives car available
    STATE = STATE_REMOTE_ADVERTISE_AVAILABLE;
        NRF_LOG_DEBUG("Advertising with SenderID: %d\r\n", msg.senderID);
        msg.type = MSG__REMOTE_TYPE_ADVERTISE_AVAILABLE;
        convert_msg_packet_to_array();
        while(STATE == STATE_REMOTE_ADVERTISE_AVAILABLE){
            nrf_esb_write_payload(tx_payload)
            nrf_delay_ms(100);
        }
        // Exit procedure after connection is made; single mode
        STATE = STATE_REMOTE_SINGLE_MODE;
        NRF_LOG_DEBUG("Connected to car\r\n");
    while (true)
       {
        switch (STATE){
            case STATE_REMOTE_SINGLE_MODE :
                tx_payload.noack = false;
                msg.type    = MSG_REMOYE_TYPE_JOYSTICK;
                msg.x       = joystick_read(x_dir);
                msg.y       = joystick_read(y_dir);
                msg.button  = joystick_button_read();
                convert_msg_packet_to_array();

        }

            if (nrf_esb_write_payload(&tx_payload) == NRF_SUCCESS)
            {
                // Toggle one of the LEDs.
                nrf_gpio_pin_write(LED_1, !(tx_payload.data[1]%8>0 && tx_payload.data[1]%8<=4));
                nrf_gpio_pin_write(LED_2, !(tx_payload.data[1]%8>1 && tx_payload.data[1]%8<=5));
                nrf_gpio_pin_write(LED_3, !(tx_payload.data[1]%8>2 && tx_payload.data[1]%8<=6));
                nrf_gpio_pin_write(LED_4, !(tx_payload.data[1]%8>3));
                tx_payload.data[1]++;
            }
            else
            {
                NRF_LOG_WARNING("Sending packet failed\r\n");
            }

            nrf_delay_us(50000);
        }
}
/*lint -restore */