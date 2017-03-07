#ifndef RC_SAADC__
#define RC_SAADC__


// NB X and Y pins must be either 3 or 4, else change the c file!!
#include "nrf_drv_saadc.h"

#define x_pin NRF_SAADC_INPUT_AIN1
#define y_pin NRF_SAADC_INPUT_AIN2
#define x_dir 0
#define y_dir 1

void saadc_init(void);
void saadc_enable(void);
int32_t saadc_read_joystick(uint32_t direction);


#endif // RC_SAADC__




/** @} */
