#include "3140_concur.h"
#include "utils.h"
#include "lock.h"

/**
TEST 1: 
	Have two lock protected processes and a single unprotected process. 
	Red and Blue light toggles will be locked, and green toggles are unrotected.
	
	Expectation:
		Have red and blue LED's toggle independently without ever interfering with each other,
		whereas green led can collide with red or blue. Each color should toggle on and off 
		five times each. 
	
	Outcome:
		Test Passed!
**/


lock_t l;


void p1(void){
	int i= 0;
	while(i < 5){
		/*NCS*/
		delay();
		i++;
		/*CS*/
		l_lock(&l);
		LEDBlue_Toggle();
		delay();
		LEDBlue_Toggle();
		l_unlock(&l);
	}
}


void p2(void){
	int i= 0;
	while(i < 5){
		/*NCS*/
		delay();
		i++;
		/*CS*/
		l_lock(&l);
		LEDRed_Toggle();
		delay();
		LEDRed_Toggle();
		l_unlock(&l);
	}
}


void p3(void){
	int i= 0;
	while(i < 5){
		/*NCS*/
		delay();
		i++;
		/*CS*/
		// l_lock(&l);
		LEDGreen_Toggle();
		delay();
		LEDGreen_Toggle();
		// l_unlock(&l);
	}
}

int main(void){
	LED_Initialize();           /* Initialize the LEDs           */	

	l_init (&l);
	
	if (process_create (p1,20) < 0) {
	 	return -1;
	}
	if (process_create (p2,20) < 0) {
	 	return -1;
	}
	if (process_create (p3,20) < 0) {
	 	return -1;
	}
	
	process_start();
	LEDGreen_On();

	while(1);
	return 0;	
}
