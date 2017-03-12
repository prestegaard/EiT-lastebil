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
#define NRF_LOG_MODULE_NAME "CAR"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

// From car
#include "rc_motor.h"
#include "rc_ultrasound.h"
#include "rc_steering.h"
// From framework
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
            SEGGER_RTT_WriteString(0, "CAR::::: TX SUCCESS EVENT\r\n");
            break;
        case NRF_ESB_EVENT_TX_FAILED:
            SEGGER_RTT_WriteString(0, "CAR::::: TX FAILED EVENT\r\n");
            (void) nrf_esb_flush_tx();
            (void) nrf_esb_start_tx();
            break;
        case NRF_ESB_EVENT_RX_RECEIVED:
            nrf_delay_ms(2);
            printf("CAR::::: RX RECEIVED EVENT\n\n");
            if (nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS)
            {
                if (rx_payload.length > 0)
                {
                    nrf_delay_ms(2);
                    printf("RX - deserialized message into struct\n");                                      
                    convert_payload_to_remote_message(&remote_msg, &rx_payload);

                    nrf_delay_ms(2);
                    printf("RX - senderID = %d\n", remote_msg.senderID);
                    nrf_delay_ms(2);
                    printf("RX - type     = %d\r\n", remote_msg.type);
                    nrf_delay_ms(2);
                    printf("RX - joy X    = %d\r\n", remote_msg.x);
                    nrf_delay_ms(2);
                    printf("RX - joy Y    = %d\r\n", remote_msg.y);
                    nrf_delay_ms(2);
                    printf("RX - button   = %d\r\n", remote_msg.button);
                    
                    switch(STATE) {
                        case STATE_CAR_ADVERTISE_AVAILABLE :     
                            if(remote_msg.type == MSG_REMOTE_TYPE_ADVERTISE_AVAILABLE){
                                SEGGER_RTT_WriteString(0, "Entered advertise mode\n");
                                car_msg.senderID = remote_msg.senderID;
                                car_msg.type = MSG_CAR_TYPE_ADVERTISE_AVAILABLE;
                                convert_car_message_to_payload(&car_msg, &tx_payload);
                                if(nrf_esb_write_payload(&tx_payload)){
                                    set_led(car_msg.senderID);
                                    NEXT_STATE = STATE_CAR_SINGLE_MODE;
                                }
                            }
                            break;
                        case STATE_CAR_SINGLE_MODE : 
                            if(remote_msg.senderID == car_msg.senderID && remote_msg.type == MSG_REMOTE_TYPE_JOYSTICK){
                                car_msg.type       = MSG_CAR_TYPE_ACKNOWLEDGE;
                                car_msg.speed_info = -1;
                                convert_car_message_to_payload(&car_msg, &tx_payload);
                                if(nrf_esb_write_payload(&tx_payload) != NRF_SUCCESS){
                                    NRF_LOG_WARNING("Sending packet failed\r\n");
                                }
                                static uint32_t start_motor = 1;
                                uint32_t left_speed = 0;
                                uint32_t right_speed = 0;
                                motorSpeeds(remote_msg.y, remote_msg.x, &left_speed, &right_speed);

                                uint32_t left_dir =  0;
                                uint32_t right_dir = 0;

                                if(left_speed > 512){
                                    left_dir = 1;
                                    left_speed -= 512;
                                }else {
                                    left_speed = 512 - left_speed;
                                }
                                if(right_speed > 512){
                                    right_dir = 1;
                                    right_speed -=512;
                                }else{
                                    right_speed = 512 - right_speed;
                                }

                                if(start_motor){
                                    motor_start();
                                    start_motor = 0;
                                }

                                if(left_dir == right_dir){
                                    motor_set_dir(BOTH, left_dir);
                                }else{
                                    motor_set_dir(LEFT, left_dir);
                                    motor_set_dir(RIGHT, right_dir);
                                }

                                if(left_speed == right_speed){
                                    motor_set_speed(BOTH, left_speed);
                                }else{
                                    motor_set_speed(LEFT, left_speed);
                                    motor_set_speed(RIGHT, right_speed);
                                }
                            }
                            break;
                    } // Switch statemachine
                    STATE = NEXT_STATE;
                } // payload >0
            } // read(&rx_payload) == sucsess
            break; // break case RX_EVENT
    } // Switch
    NRF_GPIO->OUTCLR = 0xFUL << 12;
    NRF_GPIO->OUTSET = (p_event->tx_attempts & 0x0F) << 12;
} // Function


void clocks_start( void )
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}



// Changes made:
/*
* added payload.length = 3 instead of default config 32
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
    nrf_esb_config.payload_length           = 11; // Only three byte payload for car
    nrf_esb_config.protocol                 = NRF_ESB_PROTOCOL_ESB_DPL;
    nrf_esb_config.bitrate                  = NRF_ESB_BITRATE_2MBPS;
    nrf_esb_config.event_handler            = nrf_esb_event_handler;
    nrf_esb_config.mode                     = NRF_ESB_MODE_PTX;
    nrf_esb_config.selective_auto_ack       = false;



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
    motor_init();
    ultrasound_init();

    SEGGER_RTT_WriteString(0, "CAR::::: EiT Car\r\n");
    printf("Halla fra printf\r\n");
    STATE = STATE_CAR_ADVERTISE_AVAILABLE;
    nrf_esb_start_rx();
    while (true)
    {
        /*
        switch (STATE){
            case STATE_CAR_ADVERTISE_AVAILABLE :
                car_msg.senderID = 0;
                //SEGGER_RTT_WriteString(0, "CAR::::: Advertising with SenderID: %d\r\n", car_msg.senderID);
                car_msg.type = MSG_CAR_TYPE_ADVERTISE_AVAILABLE;
                convert_car_message_to_payload(&car_msg, &tx_payload);
                // Wait until remote receives car available
                while(NEXT_STATE == STATE_CAR_ADVERTISE_AVAILABLE){
                    // NEXT_STATE gets updated in rx handler if correct message is received
                    
                    SEGGER_RTT_WriteString(0, "CAR::::: ADVERTISING\r\n");

                    if(nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS)
                    {
                        if (rx_payload.length > 0){
                            SEGGER_RTT_WriteString(0,"Received something\r\n");       
                        }
                        if(nrf_esb_write_payload(&tx_payload) != NRF_SUCCESS){
                            SEGGER_RTT_WriteString(0,"Sending packet failed\r\n");
                        }
                    }
                    nrf_delay_ms(100);
                }
                // Exit procedure after connection is made; single mode
                SEGGER_RTT_WriteString(0, "CAR::::: Connected to remote: \r\n");
                break;
            case STATE_CAR_SINGLE_MODE :
     

            case STATE_CAR_TRUCK_POOLING_PENDING :
                SEGGER_RTT_WriteString(0, "CAR::::: State: TRUCK POOLING PENDING, switches back to single mode\r\n");
                NEXT_STATE = STATE_CAR_SINGLE_MODE;
                break;

        }
        STATE = NEXT_STATE;
        */
        if (NRF_LOG_PROCESS() == false)
        {
         __WFE();
        }
    }
}


/*lint -restore */
/** @} */
