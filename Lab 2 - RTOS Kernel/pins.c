#include "tm4c123gh6pm.h"

// Inits PC 4/5/6/7 PE 0/1/2/3 for debugging

void Debug_Port_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x14;     			// activate Port C/E
	delay = SYSCTL_RCGC2_R;						// settle
	GPIO_PORTC_AFSEL_R &= ~0xF0;  		// Disable alt
	GPIO_PORTE_AFSEL_R &= ~0x0F;    	// Disable alt
	GPIO_PORTC_AMSEL_R &= ~0xF0;  		// Disable analog
	GPIO_PORTE_AMSEL_R &= ~0x0F;;   	// Disable analog
	GPIO_PORTC_DIR_R |= 0xF0;  				// Direction Out
	GPIO_PORTE_DIR_R |= 0x0F;    			// Direction Out
	GPIO_PORTC_DEN_R |= 0xF0; 				// Enable digital        
  GPIO_PORTE_DEN_R |= 0x0F;     		// Enable digital
  GPIO_PORTE_PCTL_R = ~0x0000FFFF;	// GPIO only
}

// Inits PF4, onboard switch 1

void Switch_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x20;     			// activate Port F
	delay = SYSCTL_RCGC2_R;						// settle
	GPIO_PORTE_AFSEL_R &= ~0x10;    	// Disable alt
	GPIO_PORTF_AMSEL_R &= ~0x10;  		// Disable analog
	GPIO_PORTF_DIR_R &= ~0x10;  			// Direction In
	GPIO_PORTC_DEN_R |= 0x10; 				// Enable digital 
	GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFF0FFF0)+0x00000000;		//GPIO only
	GPIO_PORTF_PUR_R |= 0x10;					// Internal resistor	
}