#ifndef __UTILITIES_H
#define __UTILITIES_H

#include <stdbool.h>
#include <stdint.h>
#include "nrf_esb.h"
#include "rc_messages_and_defines.h"
void leds_init(void);
void set_led(uint32_t number);
void clear_led(uint32_t number);

void buttons_init(void);
uint8_t get_pressed_button(void);


void convert_remote_msg_packet_to_array(remote_packet_t const * remote_msg, nrf_esb_payload_t * p_payload);
void convert_car_msg_packet_to_array(car_packet_t const * car_msg, nrf_esb_payload_t * p_payload);
#endif // UTILITIES
