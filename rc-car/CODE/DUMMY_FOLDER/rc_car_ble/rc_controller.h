#ifndef RC_CONTROLLER_
#define RC_CONTROLLER_

#include <stdint.h>

uint32_t get_speed(uint32_t iteration_time, uint32_t measured, uint32_t feed, uint32_t *last_error, uint32_t *integral);

#endif

/** @} */