#include "rc_steering.h"

#define maxVal 511
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))
#define sign(x) ((x > 0) ? 1 : ((x < 0) ? -1 : 1))


// Scale inputs to -512..512 before calling this
// Scale output to 9 bit and set direction.
void motorSpeeds(uint32_t inturn, uint32_t inthrottle, uint32_t *left_speed, uint32_t *right_speed){
	int turn = inturn - 512;
	int throttle = inthrottle - 512;
	if(abs(throttle) < 10){
		throttle = 0;
	}

	if(abs(turn) < 10){
		turn = 0;
	}

	int leftSide = throttle;
	int rightSide = throttle;
	char *dir = malloc(256);
		if(throttle < 0){
			strcpy(dir, "Reverse");
		}else{
			strcpy(dir, "Forward");
		}
	if(turn < 0){
		printf("%s %s, %s:%i, %s:%i\n", dir,"LeftTurn", "Throttle", throttle, "Turn", turn);
		leftSide = throttle + sign(throttle)*turn;
	}else if(turn > 0){
		printf("%s %s, %s:%i, %s:%i\n", dir,"RighTurn", "Throttle", throttle, "Turn", turn);
		rightSide = throttle - sign(throttle)*turn;
	}
	else{
		printf("%s , %s:%i, %s:%i\n", dir,"Throttle", throttle, "Turn", turn);
	}

	free(dir);
	int norm = 1;
	if(max(abs(leftSide), abs(rightSide)) > maxVal){
		norm = max(abs(leftSide), abs(rightSide))/maxVal;
	}

	*left_speed = leftSide/norm;
	*right_speed = rightSide/norm;
}

// In main loop, after motor speeds are desided

void foo(){
	int left = 0;
	int right = 0;

	motorSpeeds(0,0, &left, &right);

	uint32_t leftDir = 1;
	uint32_t rightDir = 1;
	if(left < 0){
		leftDir = 0;
		left = abs(left);
		leftChange = true;
	}
	if(right < 0){
		rightDir = 0;
		right = abs(right);
		rightChange = true;
	}

	if(leftDir == rightDir){
		motor_set_dir(2, leftDir);
	}else{
		motor_set_dir(0, leftDir);
		motor_set_dir(1, rightDir);
	}

	if(left == right){
		motor_set_speed(2, left);
	}else{
		motor_set_speed(0, left);
		motor_set_speed(1, right);
	}
}


int main(){
	int left_speed = 0;
	int right_speed = 0;

	motorSpeeds(-100, 0 , &left_speed, &right_speed);
	printf("%s%i, %s%i\n", "Left:",left_speed, "Right:",right_speed);

	return 0;
}