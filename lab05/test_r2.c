///*************************************************************************
// *  This test will deal with the case of context switching between
// *  realtime processes. A realtime process with an early start time
// *  but late deadline will stop. It should then be interrupted by
// *  other realtime processes with later start times, but earlier
// *  deadlines.
// *
// *  This test should turn the red LED on after 1 sec, then quickly flash the
// *  blue LED 3 times after 4 sec, then turn the red LED off. Then, the green LED
// *  will turn on indefinitely. 
// *  
// ************************************************************************/
// 
//#include "utils.h"
//#include "3140_concur.h"
//#include "realtime.h"

///*--------------------------*/
///* Parameters for test case */
///*--------------------------*/


// 
///* Stack space for processes */
//#define RT_STACK  80
// 


///*--------------------------------------*/
///* Time structs for real-time processes */
///*--------------------------------------*/

///* Constants used for deadline */
//realtime_t t_1msec = {0, 1};
//realtime_t t_10sec = {10, 0};
//realtime_t t_20sec = {20, 0};

///* Process start time */
//realtime_t t_pRT1 = {1, 0};
//realtime_t t_pRT2 = {4, 0};

// 
///*------------------*/
///* Helper functions */
///*------------------*/
//void shortDelay(){delay();}
//void mediumDelay() {delay(); delay();}

///*-------------------
// * Real-time process
// *-------------------*/

//void pRT1(void) {
//	LEDRed_On();
//	mediumDelay();
//	LEDRed_Toggle();
//	mediumDelay();
//}

//void pRT2(void) {
//	int i;
//	for (i=0; i<3;i++){
//	LEDBlue_On();
//	shortDelay();
//	LEDBlue_Toggle();
//	shortDelay();
//	}
//}

///*--------------------------------------------*/
///* Main function - start concurrent execution */
///*--------------------------------------------*/
//int main(void) {	
//	 
//	LED_Initialize();

//    /* Create processes */ 
//    if (process_rt_create(pRT1, RT_STACK, &t_pRT1, &t_20sec) < 0) { return -1; }
//    if (process_rt_create(pRT2, RT_STACK, &t_pRT2, &t_10sec) < 0) { return -1; }
//   
//   
//    /* Launch concurrent execution */
//	process_start();

//  LED_Off();
//  while(process_deadline_miss>0) {
//		LEDGreen_On();
//		shortDelay();
//		LED_Off();
//		shortDelay();
//		process_deadline_miss--;
//	}
//	
//	/* Hang out in infinite loop (so we can inspect variables if we want) */ 
//	LEDGreen_On();
//	while (1);
//	return 0;
//}
