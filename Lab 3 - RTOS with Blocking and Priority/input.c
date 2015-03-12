#include "tm4c123gh6pm.h"
#include "pins.h"

/*********** SWITCH TASKS ***********/

void (*SW1Task)(void);
void (*SW2Task)(void);
unsigned long previousTime = 0;

//SW1 Task on PF4 (rising edge interrrupt)
int OS_AddSW1Task(void(*task)(void), unsigned long priority){ 
	volatile unsigned long delay;
	SW1Task = task;
  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
	delay = SYSCTL_RCGC2_R;						// settle
  GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x10;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x10;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
	priority = priority << 21;
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|priority; // (g) priority user defined
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
	return 1;
}

//SW2 Task on PF0 (rising edge interrupt)
int OS_AddSW2Task(void(*task)(void), unsigned long priority){
	volatile unsigned long delay;
	SW2Task = task;
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
	GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  GPIO_PORTF_DIR_R &= ~0x01;    // (c) make PF0 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x01;  //     disable alt funct on PF0
  GPIO_PORTF_DEN_R |= 0x01;     //     enable digital I/O on PF0   
  GPIO_PORTF_PCTL_R = 0x00000000; // configure PF0 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x01;     //     enable weak pull-up on PF0
  GPIO_PORTF_IS_R &= ~0x01;     // (d) PF0 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x01;    //     PF0 is not both edges
  GPIO_PORTF_IEV_R &= ~0x01;    //     PF0 falling edge event
  GPIO_PORTF_ICR_R = 0x01;      // (e) clear flag0
  GPIO_PORTF_IM_R |= 0x01;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
	priority = priority << 21;
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|priority; // (g) priority user defined
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
	return 1;
}

void GPIOPortF_Handler(void){
	DIO3 ^= BIT3;
	if(GPIO_PORTF_MIS_R == 0x10){
		GPIO_PORTF_ICR_R = 0x10;
		(*SW1Task)();
	}
	else if(GPIO_PORTF_MIS_R == 0x01){
		GPIO_PORTF_ICR_R = 0x01;
		(*SW2Task)();
	}
}