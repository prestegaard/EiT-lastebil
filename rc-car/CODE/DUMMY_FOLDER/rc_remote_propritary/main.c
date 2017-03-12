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
#include "SEGGER_RTT.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

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
#include "rc_messages_and_defines.h"
#include "rc_utilities.h"

#define RTT_PRINTF(...) \
do { \
     char str[64];\
     sprintf(str, __VA_ARGS__);\
     SEGGER_RTT_WriteString(0, str);\
 } while(0)

#define printf RTT_PRINTF
 
static nrf_esb_payload_t        tx_payload = NRF_ESB_CREATE_PAYLOAD(0, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00);

static nrf_esb_payload_t        rx_payload;

/*lint -save -esym(40, BUTTON_1) -esym(40, BUTTON_2) -esym(40, BUTTON_3) -esym(40, BUTTON_4) -esym(40, LED_1) -esym(40, LED_2) -esym(40, LED_3) -esym(40, LED_4) */


static remote_packet_t remote_msg;
static car_packet_t car_msg;
static uint8_t STATE;
static uint8_t NEXT_STATE;
static uint8_t receive_ack;



void nrf_esb_event_handler(nrf_esb_evt_t const * p_event)
{
    switch (p_event->evt_id)
    {
        case NRF_ESB_EVENT_TX_SUCCESS:
            SEGGER_RTT_WriteString(0, "TX SUCCESS EVENT\r\n");
            break;
        case NRF_ESB_EVENT_TX_FAILED:
            SEGGER_RTT_WriteString(0, "TX FAILED EVENT\r\n");
            (void) nrf_esb_flush_tx();
            (void) nrf_esb_start_tx();
            break;
        case NRF_ESB_EVENT_RX_RECEIVED:
            SEGGER_RTT_WriteString(0, "RX RECEIVED EVENT\r\n");
            if (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS)
            {
                if (rx_payload.length > 0)
                {
                    SEGGER_RTT_WriteString(0, "RX RECEIVED PAYLOAD\r\n");
                    convert_payload_to_car_message(&car_msg, &rx_payload);
                    switch(STATE) {
                        case STATE_REMOTE_ADVERTISE_AVAILABLE :     
                            if(car_msg.senderID == remote_msg.senderID && car_msg.type == STATE_CAR_ADVERTISE_AVAILABLE){
                                NEXT_STATE = STATE_REMOTE_SINGLE_MODE;
                            }
                            break;
                        case STATE_REMOTE_SINGLE_MODE : 
                            if(car_msg.senderID == remote_msg.senderID && car_msg.type == MSG_CAR_TYPE_ACKNOWLEDGE){
                                receive_ack = 1;
                            }
                            break;
                    }
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
* added typedef remote_packet struct remote_msg

*/
uint32_t esb_init( void )
{
    uint32_t err_code;
    uint8_t base_addr_0[4] = {0xE7, 0xE7, 0xE7, 0xE7};
    uint8_t base_addr_1[4] = {0xC2, 0xC2, 0xC2, 0xC2};
    uint8_t addr_prefix[8] = {0xE7, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8 };

    nrf_esb_config_t nrf_esb_config         = NRF_ESB_DEFAULT_CONFIG;
    nrf_esb_config.protocol                 = NRF_ESB_PROTOCOL_ESB_DPL;
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

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    clocks_start();

    err_code = esb_init();
    APP_ERROR_CHECK(err_code);

    leds_init();
    buttons_init();
    joystick_init();

    SEGGER_RTT_WriteString(0, "EiT Remote\r\n");
    STATE = STATE_REMOTE_WAIT_FOR_CHANNEL_SELECT;
    tx_payload.noack = false;
    while (true)
    {
        switch (STATE){
            case STATE_REMOTE_WAIT_FOR_CHANNEL_SELECT : 
                while(!get_pressed_button()){
                    // Wait until user presses button and selects sender ID
                }
                remote_msg.senderID = get_pressed_button();
								printf("SenderID: %d \r\n", remote_msg.senderID);
                //SEGGER_RTT_printf(0, "SenderID is selected: %d\r\n", remote_msg.senderID);
                SEGGER_RTT_WriteString(0, "SenderID is selected\r\n");
                                NEXT_STATE = STATE_REMOTE_ADVERTISE_AVAILABLE;
                break;
                
            case STATE_REMOTE_ADVERTISE_AVAILABLE :
                //SEGGER_RTT_printf(0, "Advertising with SenderID: %d\r\n", remote_msg.senderID);
                SEGGER_RTT_WriteString(0, "Advertising with SenderID: \r\n");
                remote_msg.type = MSG_REMOTE_TYPE_ADVERTISE_AVAILABLE;
                // Wait until remote receives car available
                remote_msg.x = 0;
                while(NEXT_STATE == STATE_REMOTE_ADVERTISE_AVAILABLE){
                    remote_msg.x ++;
                    //remote_msg.type ++;
                    convert_remote_message_to_payload(&remote_msg, &tx_payload);
                    // NEXT_STATE gets updated in rx handler if correct message is received
                    nrf_esb_write_payload(&tx_payload);
                    SEGGER_RTT_WriteString(0, "REMOTE::: Advertising \r\n");
                    if(nrf_esb_write_payload(&tx_payload) != NRF_SUCCESS){
                        SEGGER_RTT_WriteString(0,"Sending packet failed\r\n");
                    }
                    if(nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS)
                    {
                        if (rx_payload.length > 0){
                            SEGGER_RTT_WriteString(0,"Received something\r\n");
                            NEXT_STATE = STATE_REMOTE_SINGLE_MODE;
                            break;       
                        }
                    }                    
                    nrf_delay_ms(100);
                }
                // Exit procedure after connection is made; single mode
                set_led(remote_msg.senderID);
                SEGGER_RTT_WriteString(0, "Connected to car\r\n");
                break;
            case STATE_REMOTE_SINGLE_MODE :
                SEGGER_RTT_WriteString(0, "REMOTE::: STATE SINGLE MODE\r\n");
                remote_msg.type    = MSG_REMOTE_TYPE_JOYSTICK;
                remote_msg.x       = joystick_read(x_dir);
                remote_msg.y       = joystick_read(y_dir);
                remote_msg.button  = joystick_button_read();
                convert_remote_message_to_payload(&remote_msg, &tx_payload);
                receive_ack = 0;
                if(nrf_esb_write_payload(&tx_payload) == NRF_SUCCESS){
                    uint32_t timeout = 64000000; // 64 mill, aka 1 second timeout
                    while(!receive_ack && --timeout > 0){
                        // Wait for ack, gets updated in rx handler
                    }
                    if(receive_ack){
                        if(remote_msg.button == 1){
                            NEXT_STATE = STATE_REMOTE_TRUCK_POOLING_PENDING;
                        }
                        else{
                            NEXT_STATE = STATE;
                        }
                    }
                    else{
                        // No ack reveiced, aka connection lost.
                        NRF_LOG_WARNING("No ack received, return to advertise mode\r\n");
                        NEXT_STATE = STATE_REMOTE_ADVERTISE_AVAILABLE;
                    }    
                }
                else
                {
                    NRF_LOG_WARNING("Sending packet failed\r\n");
                }
                break;

            case STATE_REMOTE_TRUCK_POOLING_PENDING :
                SEGGER_RTT_WriteString(0, "State: TRUCK POOLING PENDING, switches back to single mode\r\n");
                NEXT_STATE = STATE_REMOTE_SINGLE_MODE;
                break;

        }
        STATE = NEXT_STATE;
        nrf_delay_ms(100);
    }

}
/*lint -restore */
/** @} */
