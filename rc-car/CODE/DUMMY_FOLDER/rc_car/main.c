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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "boards.h"

#include "app_uart.h"
#include "app_error.h"

#include "nrf_delay.h"
#include "nrf.h"

#include "bsp.h"

#define NRF_LOG_MODULE_NAME "TRUCK 1"

#include "rc_motor.h"

#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */


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

void unit_test_motor(){
    // DISABLE LEFT MOTOR
    motor_set_speed(LEFT, 0);

    // RIGHT MOTOR FORWARD
    motor_set_dir(RIGHT, FORWARD);
    motor_set_speed(RIGHT, 200);
    motor_start();
    nrf_delay_ms(1000);
    motor_set_speed(RIGHT, 500);
    nrf_delay_ms(1000);

    // RIGHT MOTOR BACKWARD
    motor_set_dir(RIGHT, BACKWARD);
    nrf_delay_ms(1000);
    motor_set_speed(RIGHT, 200);
    nrf_delay_ms(1000);

    // PAUSE
    motor_stop();
    nrf_delay_ms(1000);

    // DISABLE RIGHT MOTOR
    motor_set_speed(RIGHT, 0);

    // LEFT MOTOR FORWARD 
    motor_set_dir(LEFT, FORWARD);
    motor_set_speed(LEFT, 200);
    motor_start();
    nrf_delay_ms(1000);
    motor_set_speed(LEFT, 500);
    nrf_delay_ms(1000);

    // LEFT MOTOR BACKWARD
    motor_set_dir(LEFT, BACKWARD);
    nrf_delay_ms(1000);     
    motor_set_speed(LEFT, 200);
    nrf_delay_ms(1000);
    motor_stop();
}



void uart_error_handle(app_uart_evt_t * p_event){
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}


int main(void)
{
    leds_init();
    uint32_t err_code;
    const app_uart_comm_params_t comm_params = {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_ENABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200
    };

    APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_error_handle,
                         APP_IRQ_PRIORITY_LOW,
                         err_code);

    APP_ERROR_CHECK(err_code);
    
    printf("\r\nStart: \r\n");

    motor_init();
    motor_set_speed(RIGHT, 500);    
    motor_start();
   // unit_test_motor();
    uint32_t i = 0;
    for (;;){
    //    uint8_t cr;
    //    while (app_uart_get(&cr) != NRF_SUCCESS);
    //    while (app_uart_put(cr) != NRF_SUCCESS);
        nrf_delay_ms(1000);
        printf("i = %d \r\n", i);
        i++;
      }
}


/** @} */
