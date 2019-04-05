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

int main (void)
{		//LED initialization
		blue_led_init();
		green_led_init();
	
		//Timer initialization
	  	NVIC_EnableIRQ(PIT0_IRQn); /* enable PIT0 Interrupts */
		SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; // Enable clock to PIT module
		PIT->MCR &= 0x0;  // enable PIT MCR
	
		PIT->CHANNEL[0].LDVAL = 0x13e3600; // Set load value to PIT 
		PIT->CHANNEL[0].TCTRL = 0x3;	/* Start timer*/

		while(1)
		{
			for(int i=0x500000;i>0;i--){}; // approximately 1.0 second delay
			blue_led_toggle();	 
		}
}

/* PIT Interrupt Handler */
void PIT0_IRQHandler(void)
{	
		green_led_toggle();
		for(int i=0x90000;i>0;i--){}; // approximately 0.1 second delay
		green_led_toggle();
		PIT->CHANNEL[0].TFLG =0x1; 
}

