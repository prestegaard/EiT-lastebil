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

#define NRF_LOG_MODULE_NAME "TRUCK 1"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"



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
    uint32_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("Start, reload: \r\n");
    NRF_LOG_INFO("EiT Lastebil, truck 1: \r\n");
    NRF_LOG_FLUSH();
    
    leds_init();
    NRF_LOG_INFO("leds_init(): \r\n");
    NRF_LOG_FLUSH();
    uart_init();   
    NRF_LOG_INFO("uart_init(): \r\n");
    NRF_LOG_FLUSH();
    motor_init();
    NRF_LOG_INFO("motor_init(): \r\n");
    NRF_LOG_FLUSH();
    ultrasound_init();
    NRF_LOG_INFO("ultrasound_init(): \r\n");
    NRF_LOG_FLUSH();
    //printf("\r\nEiT Lastebil: \r\n");

	NRF_LOG_INFO("unit test motor(): \r\n");
    NRF_LOG_FLUSH();
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
