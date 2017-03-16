#ifndef RC_CONTROLLER_
#define RC_CONTROLLER_

#include <stdint.h>

uint32_t get_speed(float iteration_time, uint32_t measured, uint32_t feed, float *last_error, float *integral);

#endif

/** @} */