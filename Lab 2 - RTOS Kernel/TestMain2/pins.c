#include "tm4c123gh6pm.h"

// Inits PC 4/5/6/7 PE 0/1/2/3 for debugging

void Debug_Port_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x10;     			// activate Port E
	delay = SYSCTL_RCGC2_R;						// settle
	GPIO_PORTE_AFSEL_R &= ~0x0F;    	// Disable alt
	GPIO_PORTE_AMSEL_R &= ~0x0F;;   	// Disable analog
	GPIO_PORTE_DIR_R |= 0x0F;    			// Direction Out
  GPIO_PORTE_DEN_R |= 0x0F;     		// Enable digital
  GPIO_PORTE_PCTL_R &= ~0x0000FFFF;	// GPIO only
	
	SYSCTL_RCGC2_R |= 0x04;     			// activate Port C
	delay = SYSCTL_RCGC2_R;						// settle
	GPIO_PORTC_AFSEL_R &= ~0xF0;  		// Disable alt
	GPIO_PORTC_AMSEL_R &= ~0xF0;  		// Disable analog
	GPIO_PORTC_DIR_R |= 0xF0;  				// Direction Out
	GPIO_PORTC_DEN_R |= 0xF0; 				// Enable digital   
	GPIO_PORTC_PCTL_R &= ~0xFFFF0000;	// GPIO only	
}
