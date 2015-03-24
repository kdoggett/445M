#include <stdint.h>
#include "tm4c123gh6pm.h"

void (*PeriodicTaskA)(void);   // user function
void (*PeriodicTaskB)(void);   // user function

// *******************************
// ********** Timer3A ************
// *******************************
void Timer3_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGCTIMER_R |= 0x08;
	delay = SYSCTL_RCGC2_R;
  TIMER3_CTL_R = 0x00;
  TIMER3_CFG_R = 0;
  TIMER3_TAMR_R = 0x02;
	TIMER3_ICR_R |= 0x01;
	TIMER3_IMR_R |= 0x01;
	NVIC_EN1_R = 8;
}

void Timer3A_Launch(void(*task)(void), uint32_t period, unsigned long priority) {
	PeriodicTaskB = task;
	TIMER3_TAILR_R = period-1;
	priority = priority << 29;
	NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)| priority;
	TIMER3_CTL_R |= TIMER_CTL_TAEN;
}

void Timer3A_Handler(void){
  TIMER3_ICR_R = TIMER_ICR_TATOCINT;
	(*PeriodicTaskB)();
}

// *******************************
// ********** Timer2A ************
// *******************************
void Timer2_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGCTIMER_R |= 0x04;
	delay = SYSCTL_RCGC2_R;
	TIMER2_CTL_R &= ~TIMER_CTL_TAEN;
  TIMER2_CFG_R = 0;
	TIMER2_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
	TIMER2_IMR_R |= TIMER_IMR_TATOIM;
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;
	NVIC_EN0_R = 1 << 23;
}

void Timer2A_Launch(void(*task)(void), uint32_t period, unsigned long priority) {
	PeriodicTaskA = task;
	TIMER2_TAILR_R = period-1;
	priority = priority << 29;
	NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)| priority;
	TIMER2_CTL_R |= TIMER_CTL_TAEN;
}

void Timer2A_Handler(void){
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;
	(*PeriodicTaskA)();
}