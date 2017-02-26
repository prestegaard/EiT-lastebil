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

#include "rc_uart.h"


const app_uart_comm_params_t comm_params = {
    RX_PIN_NUMBER,
    TX_PIN_NUMBER,
    RTS_PIN_NUMBER,
    CTS_PIN_NUMBER,
    APP_UART_FLOW_CONTROL_ENABLED,
    false,
    UART_BAUDRATE_BAUDRATE_Baud115200
};


// NEEDS TO BE DECLARED BEFORE INIT
void uart_error_handle(app_uart_evt_t * p_event);

void uart_init(){
    uint32_t err_code;
    APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_error_handle,
                         APP_IRQ_PRIORITY_LOW,
                         err_code
                        );
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

/** @} */
