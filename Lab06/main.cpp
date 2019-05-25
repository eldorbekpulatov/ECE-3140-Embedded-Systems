#include "mbed.h"
#include "Servo.h"
#include "Stepper.h"
#include "MK64F12.h"
#include <fsl_device_registers.h>


#define ONE_MS 0.001

struct current_status{
	float *current_x; // points to step_p->curr_pos
	float *current_y; // points to step_p->curr_pos
	int current_pen;
}typedef current_t;

void initializeStepper(Stepper *stepper) {
	if ( stepper->mode == 1.0 ) { 
					*(stepper->pinM0) = 0; 
					*(stepper->pinM1) = 0;
					*(stepper->pinM2) = 0;
				}
				else if ( stepper->mode == 0.5 ) {
					*(stepper->pinM0) = 0; 
					*(stepper->pinM1) = 0;
					*(stepper->pinM2) = 1;
				}
				else if ( stepper->mode == 0.25 ) {
					*(stepper->pinM0) = 0; 
					*(stepper->pinM1) = 1;
					*(stepper->pinM2) = 0;
				}
				else if ( stepper->mode == 1/8 ) {
					*(stepper->pinM0) = 0; 
					*(stepper->pinM1) = 1;
					*(stepper->pinM2) = 1;
				}
				else if ( stepper->mode == 1/16 ) {
					*(stepper->pinM0) = 1; 
					*(stepper->pinM1) = 0;
					*(stepper->pinM2) = 0;
				}
				else {
					*(stepper->pinM0) = 1; 
					*(stepper->pinM1) = 0;
					*(stepper->pinM2) = 1;
				}
				*(stepper->pinDir) = 1;
}


// GLOBALS
Stepper *y = new Stepper(PTB23, PTA1, PTB9, PTC16, PTC17, 0.03125, ONE_MS, 0.0, 1);
Stepper *x = new Stepper(PTD3, PTD2, PTD0, PTC12, PTC4, 0.03125, ONE_MS, 0.0, 1);
current_t current_position = {&(x->curr_pos), &(y->curr_pos), 0};
Servo myservo(PTD1);


// lowers pen
void penDown() {
	myservo = 0.25;
	current_position.current_pen = 1;
}
// raise pen
void penUp() {
	myservo = 0.6;
	current_position.current_pen = 0;
}
// calculate slope
float getSlope(current_t current, float x_cord, float y_cord) {
	if ( x_cord == *(current.current_x)) return INT_MAX;
	return (y_cord - *(current.current_y))/(x_cord - *(current.current_x)); 
}
// calculate gcd
int gcd( int a , int b) {
	if ( b == 0 ) return a;
	return gcd(b, a % b);
}
// impulse
void impulse(Stepper *stepper) {
	*(stepper->pinStep) = 1;
	wait(stepper->minImpulseDelay);
	*(stepper->pinStep) = 0;
	wait(stepper->minImpulseDelay);
}
// moves the given steeper
void move(Stepper *stepper, float goal, float time){
	int num_steps = (int) ( (goal - stepper->curr_pos)/stepper->mode );				
	*(stepper->pinDir) = 1;
  if(num_steps < 0) {
		*(stepper->pinDir) = 0;
    num_steps = -1 * num_steps;
	}			
	float delay = abs(time/(goal - stepper->curr_pos))*stepper->mode;
	for ( int i = 0; i < num_steps; i++ ) {
		impulse(stepper);
		if (*(stepper->pinDir)) stepper->curr_pos += stepper->mode;
		else stepper->curr_pos -= stepper->mode;
	}
}

void moveXY(float x_cord, float y_cord, float s) {
	int ximpulse = abs( (x_cord - x->curr_pos)/x->mode );
	int yimpulse = abs( (y_cord - y->curr_pos)/y->mode );
	if ( (x_cord - x->curr_pos) > 0 ) *(x->pinDir) = 1;
	else *(x->pinDir) = 0;
	if ( (y_cord - y->curr_pos) > 0 ) *(y->pinDir) = 1;
	else *(y->pinDir) = 0;
	
	int div = gcd(ximpulse, yimpulse);
	int xmult = ximpulse / div;
	int ymult = yimpulse / div;
	
	int shorter;
	if (ximpulse > yimpulse) shorter = yimpulse;
	else shorter = ximpulse;
	
	int i = 0;
	while ( i < shorter ) {
		for (int j = 0; j < ymult; j++ ) {
			impulse(y);
		}
		if (*(y->pinDir)) y->curr_pos += ymult*(y->mode);
		else y->curr_pos -= ymult*(y->mode);
		
		for (int n = 0; n < xmult; n++ ) {
			impulse(x);
		}
		if (*(x->pinDir)) x->curr_pos += xmult*(x->mode);
		else x->curr_pos -= xmult*(x->mode);
		i++;
	}
}
 
void moveTo(float x_cord, float y_cord){
	float s = getSlope(current_position, x_cord, y_cord); 
	float t;
	if (s == INT_MAX || abs(s) > 1) {
		// todo y fast
		t = abs((y_cord - *(current_position.current_y))*y->minStepDelay);
	}
	else if (abs(s) <= 1 || s == 0) {
		// todo: x fast
		t = abs((x_cord - *(current_position.current_x))*x->minStepDelay);
	}
	
	if (s == INT_MAX) {
		move(y,y_cord,t);
	}
	else if (s == 0) {
		move(x,x_cord,t);
	}
	else {
		moveXY(x_cord, y_cord, s);
	}
}

int main() {
  // init motors
	initializeStepper(x);
	initializeStepper(y);
	
	penUp();
	wait(0.5);
	
	moveTo(20,225);
	moveTo(20,215);
	moveTo(30,225);
	moveTo(30,170);

	
	moveTo(0,0);
	
}

