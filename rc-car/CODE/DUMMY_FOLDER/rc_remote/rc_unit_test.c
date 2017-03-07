#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nrf_delay.h"
#include "nrf_drv_saadc.h"

#include "rc_motor.h"
#include "rc_uart.h"
#include "rc_ultrasound.h"
#include "rc_saadc.h"


void unit_test_motor(){
    printf("############### UNIT MOTOR \r\n");
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

void unit_test_ultrasound(){
    printf("############### UNIT TEST ULTRASOUND \r\n");
    for(uint8_t i=0; i<10; i++){
        printf("Distance: %d mm\t \r\n", ultrasound_get_distance());
        nrf_delay_ms(1000);
    }
}

void unit_test_saadc(){
    //static nrf_saadc_value_t     adc_val[2][1];
    saadc_enable(); 
 
    for(uint32_t i=0; i<100; i++){
//        nrf_drv_saadc_sample();
//        nrf_drv_saadc_sample_convert(0, adc_val[0]);
//        nrf_drv_saadc_sample_convert(1, adc_val[1]);
//        int test = (int)adc_val[0][0];
        int x_val = saadc_read_joystick(x_dir);
        int y_val = saadc_read_joystick(y_dir);
        printf("X: %d\t\t Y: %d\t\t \r\n", x_val, y_val);
        nrf_delay_ms(100);
    }
}



/** @} */
