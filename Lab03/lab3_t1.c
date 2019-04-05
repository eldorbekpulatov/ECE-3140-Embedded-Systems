
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

int main (void)
{
 LED_Initialize();
	
	// tests the case when we just have one program running
	// expected output: 
	// 		red led toggles for 5 times and green led turns on and stays on
 
 if (process_create (p1,20) < 0) {
 	return -1;
 }

 process_start();
 
 LEDGreen_On();
 
 while (1) ;

 return 0;
}

