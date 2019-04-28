///*************************************************************************
// *  This test will test the case in which there are unready realtime
// *  processes, but no realtime or non-realtime processes to run in the
// *  meantime. This test should wait until the unready processes are
// *  ready, run them, and then finish. 
// *
// * This test will pause for 5 seconds, then flash the blue LED then red
// * LED then turn on the green LED indefinitely.
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
//#define NRT_STACK 80
//#define RT_STACK  80
// 


///*--------------------------------------*/
///* Time structs for real-time processes */
///*--------------------------------------*/

///* Constants used for 'work' and 'deadline's */
//realtime_t t_1msec = {0, 1};
//realtime_t t_10sec = {10, 0};

///* Process start time */
//realtime_t t_pRT1 = {0,100};
//realtime_t t_pRT2 = {0,100};

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
//	LEDBlue_On();
//	mediumDelay();
//	LEDBlue_Toggle();
//	mediumDelay();
//}

//void pRT2(void) {
//	LEDRed_On();
//	mediumDelay();
//	LEDRed_Toggle();
//	mediumDelay();
//}

///*--------------------------------------------*/
///* Main function - start concurrent execution */
///*--------------------------------------------*/
//int main(void) {	
//	 
//	LED_Initialize();

//	/* Create processes */ 
//	if (process_rt_create(pRT1, RT_STACK, &t_pRT1, &t_10sec) < 0){ return -1; }
//	if (process_rt_create(pRT2, RT_STACK, &t_pRT2, &t_10sec) < 0) { return -1; } 
// 
//  /* Launch concurrent execution */
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
