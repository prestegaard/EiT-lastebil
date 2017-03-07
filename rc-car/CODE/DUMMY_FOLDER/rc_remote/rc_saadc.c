#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nrf.h"
#include "nrf_drv_saadc.h"

#include "app_util_platform.h"
#include "boards.h"

#include "rc_saadc.h"

void saadc_callback(nrf_drv_saadc_evt_t const * p_event);

void saadc_init(){
    ret_code_t err_code;
    nrf_drv_saadc_config_t saadc_config = NRF_DRV_SAADC_DEFAULT_CONFIG;
    saadc_config.resolution = NRF_SAADC_RESOLUTION_10BIT;
/*
    if(x_pin == 3){
        x_pin = NRF_SAADC_INPUT_AIN1;
        y_pin = NRF_SAADC_INPUT_AIN2;
    }
    else if(x_pin == 4){
        x_pin = NRF_SAADC_INPUT_AIN2;
        y_pin = NRF_SAADC_INPUT_AIN1;
    }

*/    
    // X direction
    nrf_saadc_channel_config_t channel_0_config =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(x_pin);
    channel_0_config.gain = NRF_SAADC_GAIN1_4;
    channel_0_config.reference = NRF_SAADC_REFERENCE_VDD4;

    // Y direction
    nrf_saadc_channel_config_t channel_1_config =
        NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(y_pin);
    channel_1_config.gain = NRF_SAADC_GAIN1_4;
    channel_1_config.reference = NRF_SAADC_REFERENCE_VDD4;

    err_code = nrf_drv_saadc_init(&saadc_config, saadc_callback);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(0, &channel_0_config);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_saadc_channel_init(1, &channel_1_config);
    APP_ERROR_CHECK(err_code);
}


void saadc_callback(nrf_drv_saadc_evt_t const * p_event){
    //NOP
}

void saadc_enable(){
       nrf_drv_saadc_sample();
}
int32_t saadc_read_joystick(uint32_t direction){
    static nrf_saadc_value_t     adc_val[2][1];
    if (direction == x_dir){
        nrf_drv_saadc_sample_convert(0, adc_val[0]);
        return (int32_t)adc_val[0][0];
    }
    else if (direction == y_dir){
        nrf_drv_saadc_sample_convert(1, adc_val[1]);
        return (int32_t)adc_val[1][0];
    }
    else 
        return -1;
}

/** @} */
