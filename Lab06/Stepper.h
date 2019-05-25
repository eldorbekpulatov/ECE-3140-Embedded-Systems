#ifndef __STEPPER_H__
#define __STEPPER_H__

#include "mbed.h"

class Stepper{
	public:
		DigitalOut *pinM0;
		DigitalOut *pinM1;
		DigitalOut *pinM2;
		DigitalOut *pinDir;
		DigitalOut *pinStep;
		PinName     M0;
		PinName     M1;
		PinName     M2;
		PinName     Dir;
		PinName     Step;
		float mode;	  // = 1/[1,2,4,8,16,32]
		float minStepDelay; 	  // = 5ms equivalent
		float minImpulseDelay; // = mode * stepDelay
		float curr_pos;   // = [0.0 - 250.0]
		int curr_direction;

		Stepper(PinName M0, PinName M1, PinName M2, PinName Dir, PinName Step,
			float mode, float stepDelay, float curr_pos, int curr_direction ){
			
				this->M0 = M0;	
				this->pinM0 = new DigitalOut(M0);
				this->M1 = M1;	
				this->pinM1 = new DigitalOut(M1);
				this->M2 = M2;	
				this->pinM2 = new DigitalOut(M2);
				this->Dir = Dir;	
				this->pinDir = new DigitalOut(Dir);
				this->Step = Step;	
				this->pinStep = new DigitalOut(Step);
				this->mode = mode;
				this->minStepDelay = stepDelay;
				this->curr_pos = curr_pos;
				this->curr_direction = curr_direction;
				this->minImpulseDelay = mode*stepDelay;
		}
		
};




#endif
