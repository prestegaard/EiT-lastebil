#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nrf_delay.h"
#include "nrf.h"

#include "bsp.h"

#include "rc_utilities.h"


void leds_init(){
    nrf_gpio_pin_dir_set(17, NRF_GPIO_PIN_DIR_OUTPUT); 
    nrf_gpio_pin_dir_set(18, NRF_GPIO_PIN_DIR_OUTPUT); 
    nrf_gpio_pin_dir_set(19, NRF_GPIO_PIN_DIR_OUTPUT); 
    nrf_gpio_pin_dir_set(20, NRF_GPIO_PIN_DIR_OUTPUT); 
    nrf_gpio_pin_set(17);
    nrf_gpio_pin_set(18);
    nrf_gpio_pin_set(19);
    nrf_gpio_pin_set(20);
}

void buttons_init(){
    nrf_gpio_cfg_input(13, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(14, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(15, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(16, NRF_GPIO_PIN_PULLUP);
}

void set_led(uint32_t number){
    if(number >=1 && number <= 4){
        number = 16 + number; // Convert from led number to pin    
        nrf_gpio_pin_clear(number);
    }
}

void clear_led(uint32_t number){
    if(number >=1 && number <= 4){
        number = 16 + number; // Convert from led number to pin    
        rf_gpio_pin_set(number);
    }
}

uint8_t get_pressed_button(){
    // Priority button 1 first, button 4 last
    if(!nrf_gpio_pin_read(13))
        return 1;
    if(!nrf_gpio_pin_read(14))
        return 2;
    if(!nrf_gpio_pin_read(15))
        return 3;
    if(!nrf_gpio_pin_read(16))
        return 4;
// Retrun 0 if no buttons are pressed
    return 0;
}

void convert_remote_msg_packet_to_array(remote_packet_t const * remote_msg, nrf_esb_payload_t * p_payload){
    p_payload.data[0]   = remote_msg.senderID;
    p_payload.data[1]   = remote_msg.type
    p_payload.data[2]   = remote_msg.x;       // LSB
    p_payload.data[3]   = remote_msg.x >> 8; 
    p_payload.data[4]   = remote_msg.x >> 16;
    p_payload.data[5]   = remote_msg.x >> 24; // MSB
    p_payload.data[6]   = remote_msg.y;       // LSB 
    p_payload.data[7]   = remote_msg.y >> 8;  
    p_payload.data[8]   = remote_msg.y >> 16; 
    p_payload.data[9]   = remote_msg.y >> 24; // MSB
    p_payload.data[10]  = remote_msg.button;    
}

void convert_car_msg_packet_to_array(car_packet_t const * car_msg, nrf_esb_payload_t * p_payload){
    p_payload.data[0]   = remote_msg.senderID;
    p_payload.data[1]   = remote_msg.type
    p_payload.data[2]   = remote_msg.speed_info;       // LSB
   
}
/** @} */
