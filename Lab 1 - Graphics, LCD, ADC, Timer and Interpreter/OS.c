#include "OS.h"
#include "tm4c123gh6pm.h"

void (*PeriodicTask)(void);

int OS_AddPeriodicThread(void(*task)(void),unsigned long period,unsigned long priority){
	SYSCTL_RCGCTIMER_R |= 0x20;
	PeriodicTask = task;
	TIMER5_CTL_R = 0x00;					//disable during setup
	TIMER5_CFG_R = 0x00;					//32 bit mode
	TIMER5_TAMR_R = 0x02;					//periodic mode
	TIMER5_TAILR_R = period - 1;	//requested reload value
	TIMER5_TAPR_R = 0x00;					//bus clock resolution, no prescale
	TIMER5_ICR_R = 0x01;					//clear timeout flag
	TIMER5_IMR_R = 0x01;					//arm timeout interrupt
	NVIC_PRI23_R = (NVIC_PRI23_R&0xFFFFFF00) | 0x80; //priority 4	(need to change priority, maybe left shift 5 times?)
	NVIC_EN2_R = 0x10000000;			//enable IRQ 92
	TIMER5_CTL_R = 0x01;					//enable timer 5A
	EnableInterrupts();
	return 0;
}

void Timer5A_Handler(void){
	TIMER5_ICR_R = 0x01;					//acknowledge timeout
	(*PeriodicTask)();	
}

void OS_ClearPeriodicTime(void){
	TIMER5_TAILR_R = 0;						//resets counter to 0, TAILR register
}

unsigned long OS_ReadPeriodicTimer(void){
	return TIMER5_TAILR_R;
}