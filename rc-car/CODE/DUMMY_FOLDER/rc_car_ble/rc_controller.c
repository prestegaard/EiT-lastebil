#include "rc_controller.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define DESIRED_DISTANCE = 100; //mm
#define Kp = 0.1;
#define Kd = 1;
#define Ki = 0.01;
#define Kf = 1;

#define max(X, Y) (((X) > (Y)) ? (X) : (Y))
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define sign(x) ((x > 0) ? 1 : ((x < 0) ? -1 : 1))

uint32_t get_speed(uint32_t iteration_time, uint32_t measured, uint32_t feed, uint32_t *last_error, uint32_t *integral){
	uint32_t error = DESIRED_DISTANCE - measured;
	*integral += error*iteration_time;
	uint32_t derivative = (error - *last_error)/iteration_time;
	*last_error = error;
	uint32_t output = Kp*error + Kd*derivative + Ki*(*integral) + Kf*feed;
	return max(0, min(output,511));
}