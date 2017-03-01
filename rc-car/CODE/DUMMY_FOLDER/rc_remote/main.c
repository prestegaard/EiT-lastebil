/* Copyright (c) 2015 Nordic Semiconductor. All Rights Reserved.
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

/** @file
 * @defgroup pwm_example_main main.c
 * @{
 * @ingroup pwm_example
 *
 * @brief PWM Example Application main file.
 *
 * This file contains the source code for a sample application using PWM.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nrf_delay.h"
#include "nrf.h"

#include "bsp.h"

#include "rc_motor.h"
#include "rc_uart.h"
#include "rc_ultrasound.h"
#include "rc_unit_test.h"



void leds_init(){
    nrf_gpio_pin_dir_set(17, NRF_GPIO_PIN_DIR_OUTPUT); 
    nrf_gpio_pin_dir_set(18, NRF_GPIO_PIN_DIR_OUTPUT); 
    nrf_gpio_pin_dir_set(19, NRF_GPIO_PIN_DIR_OUTPUT); 
    nrf_gpio_pin_dir_set(20, NRF_GPIO_PIN_DIR_OUTPUT); 
}

void set_led(uint32_t number){
    nrf_gpio_pin_clear(number);
}


void clear_led(uint32_t number){
    nrf_gpio_pin_set(number);
}


int main(void)
{
    leds_init();
    uart_init();   
    motor_init();
    ultrasound_init();
    printf("\r\nEiT Lastebil: \r\n");

    motor_set_speed(RIGHT, 500);    
    motor_start();


    unit_test_motor();
    unit_test_ultrasound();
    for (;;){
    //    uint8_t cr;
    //    while (app_uart_get(&cr) != NRF_SUCCESS);
    //    while (app_uart_put(cr) != NRF_SUCCESS);
        nrf_delay_ms(1000);
        //printf("Distance: %d mm\t \r\n", ultrasound_get_distance());
      }
}


/** @} */
