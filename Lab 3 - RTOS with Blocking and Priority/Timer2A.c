// Timer0A.c
// Runs on LM4F120/TM4C123
// Use Timer0A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
   Volume 1, Program 9.8

  "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Volume 2, Program 7.5, example 7.6

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <stdint.h>
#include "tm4c123gh6pm.h"

void WaitForInterrupt(void);  // low power mode
void (*PeriodicTaskA)(void);   // user function
void (*PeriodicTaskB)(void);   // user function


// ***************** Timer2A_Init ****************
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer2_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
	delay = SYSCTL_RCGC2_R;						// settle
  TIMER2_CTL_R &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);    // 1) disable TIMER2A during setup
  TIMER2_CFG_R = TIMER_CFG_16_BIT;    // 2) configure for 32-bit mode
// Initialize Timer2A 
	TIMER2_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
	TIMER2_IMR_R |= TIMER_IMR_TATOIM;			// arm timeout interrupt
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;    // 6) clear TIMER2A timeout flag
// Initialize Timer2B 
	TIMER2_TBMR_R = TIMER_TBMR_TBMR_PERIOD;
	TIMER2_IMR_R |= TIMER_IMR_TBTOIM;			// arm timeout interrupt
  TIMER2_ICR_R = TIMER_ICR_TBTOCINT;
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
	NVIC_EN0_R = (0x01 << 23) + (0x01 << 24);
}

void Timer2A_Launch(void(*task)(void), uint32_t period, unsigned long priority) {
	PeriodicTaskA = task;          // user function
	TIMER2_TAILR_R = period-1;    // 4) reload value
	priority = priority << 29;
	NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)| priority; // 8) Timer priority is based off of priority of thread
	TIMER2_CTL_R |= TIMER_CTL_TAEN;    // 10) enable TIMER2A  
}

void Timer2A_Handler(void){
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer2A timeout
	(*PeriodicTaskA)();
}

void Timer2B_Launch(void(*task)(void), uint32_t period, unsigned long priority) {
	PeriodicTaskB = task;          // user function
	TIMER2_TBILR_R = period-1;    // 4) reload value
	priority = priority << 5;
	NVIC_PRI6_R = (NVIC_PRI6_R&0xFFFFFF00)| priority; // 8) Timer priority is based off of priority of thread
	TIMER2_CTL_R |= TIMER_CTL_TBEN;    // 10) enable TIMER2A  
}

void Timer2B_Handler(void){
  TIMER2_ICR_R = TIMER_ICR_TBTOCINT;// acknowledge timer2A timeout
	(*PeriodicTaskB)();
}