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

#include <stdio.h>
#include <string.h>
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"
#include "bsp.h"
#include "app_timer.h"
#include "nrf_drv_clock.h"
#define NRF_LOG_MODULE_NAME "APP"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "rc_motor.h"
#include "nrf_delay.h"







void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)
{
	#ifdef DEBUG
	app_error_print(id, pc, info);
	#endif

	bsp_board_leds_on();
	app_error_save_and_stop(id, pc, info);
}


int main(void)
{
	nrf_gpio_pin_dir_set(17, NRF_GPIO_PIN_DIR_OUTPUT); 
	nrf_gpio_pin_set(17);


	NRF_LOG_INFO("PWM example\r\n");
	motor_init();
	motor_set_dir(LEFT, FORWARD);
	motor_set_dir(RIGHT, BACKWARD);

//	nrf_gpio_pin_dir_set(11, NRF_GPIO_PIN_DIR_OUTPUT); 
//	nrf_gpio_pin_dir_set(12, NRF_GPIO_PIN_DIR_OUTPUT); 
//	nrf_gpio_pin_dir_set(13, NRF_GPIO_PIN_DIR_OUTPUT); 
//	nrf_gpio_pin_set(11);
//	nrf_gpio_pin_clear(12);
//	nrf_gpio_pin_set(13);
	motor_set_speed(RIGHT, 511);
	motor_set_speed(LEFT, 511);
	motor_start();
	nrf_gpio_pin_clear(17);
	nrf_delay_ms(200);
	//nrf_gpio_pin_set(motor_pwm_b);
	for (;;){
		/*
		nrf_gpio_pin_set(17);
		nrf_delay_ms(1500);
		//nrf_gpio_pin_set(17);
		motor_set_speed(RIGHT, 7000);
		nrf_delay_ms(1500);
		*/
		}
}


/** @} */
