#include <stdint.h>
#include "tm4c123gh6pm.h"

void (*PeriodicTaskB)(void);   // user function
// ***************** Timer3A_Init ****************
void Timer3_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGCTIMER_R |= 0x08;			// activate
	delay = SYSCTL_RCGC2_R;					// settle
  TIMER3_CTL_R = 0x00;    				// disable during setup
  TIMER3_CFG_R = TIMER_CFG_16_BIT;    				// 32-bit mode
  TIMER3_TAMR_R = 0x02;   				// periodic mode, default down-count settings
	TIMER3_ICR_R |= 0x01;
	TIMER3_IMR_R |= 0x01;
	NVIC_EN1_R = 8;
}

void Timer3A_Launch(void(*task)(void), uint32_t period, unsigned long priority) {
	PeriodicTaskB = task;          // user function
	TIMER3_TAILR_R = period-1;    // 4) reload value
	priority = priority << 29;
	NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)| priority; // 8) Timer priority is based off of priority of thread
	TIMER3_CTL_R |= TIMER_CTL_TAEN;    // 10) enable TIMER3A 
}

void Timer3A_Handler(void){
  TIMER3_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer3A timeout
	(*PeriodicTaskB)();
}