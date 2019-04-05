#include <fsl_device_registers.h> 

void red_led_init(){
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; //Enable the clock to port B
	PORTB->PCR[22] = PORT_PCR_MUX(001); //Set up PTB22 as GPIO 
	PTB->PDDR = 1 << 22; // enable Pin PTB22 as Output
	PTB->PSOR = 1 << 22; 
}
void red_led_on(){
	PTB->PCOR |= 1 << 22;
}
void red_led_toggle(){
	PTB->PTOR = 1 << 22;
}
void red_led_off(){
	PTB->PSOR = 1 << 22; 
}

void green_led_init(){
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; //Enable the clock to port E
	PORTE->PCR[26] = PORT_PCR_MUX(001); //Set up PTE26 as GPIO 
	PTE->PDDR = 1 << 26; // enable Pin PTE26 as Output
	PTE->PSOR = 1 << 26;
}
void green_led_on(){
	PTE->PCOR |= 1 << 26;
}
void green_led_off(){
	PTE->PSOR = 1 << 26;
}
void green_led_toggle(){
	PTE->PTOR = 1 << 26;
}

void blue_led_init(){
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; //Enable the clock to port B
	PORTB->PCR[21] = PORT_PCR_MUX(001); //Set up PTB21 as GPIO 
	PTB->PDDR = 1 << 21; // enable Pin PTB21 as Output
	PTB->PSOR = 1 << 21; 
}
void blue_led_on(){
	PTB->PCOR |= 1 << 21; 
}
void blue_led_off(){
	PTB->PSOR = 1 << 21; 
}
void blue_led_toggle(){
	PTB->PTOR = 1 << 21;
}



int main (void) {
	red_led_init(); // initialize the red led
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; // Enable clock to PIT module
	PIT->CHANNEL[0].LDVAL = 0x16e3600; // Set load value of zeroth PIT 
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;  // Set MCR
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;	/* Start timer*/
	
	while(1){
		if (PIT->CHANNEL[0].TFLG) { 
			red_led_toggle();
			PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK; 
		}
	}
}
