#ifndef RC_STEERING_
#define RC_STEERING_

#include <stdint.h>


void motorSpeeds(uint32_t inturn, uint32_t inthrottle, uint32_t *left_speed, uint32_t *right_speed, uint32_t id);

void motorDirections(uint32_t *left_speed, uint32_t *right_speed, uint32_t *left_dir, uint32_t *right_dir);

#endif

/** @} */
