
#include "3140_concur.h"
#include "utils.h"

void p1 (void)
{
	int i;
	for (i=0; i < 10; i++) {
		delay ();
    LEDRed_Toggle();
	}
}

void p2 (void)
{
	int i;
	for (i=0; i <8; i++) {
		delay ();
    LEDBlue_Toggle();
	}
}

void p3 (void)
{
	int i;
	for (i=0; i <6; i++) {
		delay ();
    LEDGreen_Toggle();
	}
}

int main (void)
{
 LED_Initialize();
	
	// tests the case: 
	// 				where processes terminate one by one and leaves others to continue
	// expected output:
	// 				(r-g-b) white led toggles 3 times
	//				(r-b) purple led toggles 1 time
	//				(r) red led toggles 1 time
	// 				
	//				green led turns on and stays on forever
 if (process_create (p1,20) < 0) {
 	return -1;
 }
  if (process_create (p3,20) < 0) {
 	return -1;
 }
if (process_create (p2,20) < 0) {
		return -1;
}

 process_start ();
 
 LEDGreen_On();
 
 while (1) ;
 

 return 0;
}

