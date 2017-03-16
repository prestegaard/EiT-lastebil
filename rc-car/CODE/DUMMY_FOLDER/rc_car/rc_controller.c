#include "rc_controller.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

static uint32_t DESIRED_DISTANCE = 100; //mm
static float Kp = 0.01;
static float Kd = 0.000001;
static float Ki = 0.001;
static float Kf = 1;

#define max(X, Y) (((X) > (Y)) ? (X) : (Y))
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define sign(x) ((x > 0) ? 1 : ((x < 0) ? -1 : 1))

uint32_t get_speed(float iteration_time, uint32_t measured, uint32_t feed, float *last_error, float *integral){
	float error = (float) (DESIRED_DISTANCE - measured)/1000;
	*integral += error*iteration_time;
	float derivative = (error - *last_error)/iteration_time;
	*last_error = error;
	uint32_t output = (uint32_t) Kp*error + Kd*derivative + Ki*(*integral) + Kf*feed;
	return max(0, min(output,511));
}