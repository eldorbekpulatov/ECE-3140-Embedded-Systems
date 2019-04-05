#include "3140_concur.h"
#include "utils.h"
#include "lock.h"

/**
TEST 2: 
	Includes three processes running concurrently with two locks.
	Process A and Process B each have different locks.
	Process C needs both locks to complete. 
	
	Expectation:
		We had red and blue LED toggles protected with different locks,
		and green LED toggles only when both red and blue leds aren't.
		RED and BLUE should always blink/collide togother since atomic 
		period is low. GREEN then should blink after both red and blue 
		are off. Each LED should toggle 5 times each.
		
	Result:
		Test Passed!
**/


lock_t l;
lock_t m;


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
		l_lock(&m);
		LEDRed_Toggle();
		delay();
		LEDRed_Toggle();
		l_unlock(&m);
	}
}


void p3(void){
	int i= 0;
	while(i < 5){
		/*NCS*/
		delay();
		i++;
		/*CS*/
		l_lock(&l);
		l_lock(&m);
		LEDGreen_Toggle();
		delay();
		LEDGreen_Toggle();
		l_unlock(&m);
		l_unlock(&l);
	}
}

int main(void){
	LED_Initialize();           /* Initialize the LEDs           */	

	l_init(&l);
	l_init(&m);
	
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
