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
void (*PeriodicTask)(void);   // user function


// ***************** Timer2A_Init ****************
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer2A_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
	delay = SYSCTL_RCGC2_R;						// settle
  TIMER2_CTL_R = 0x00000000;    // 1) disable TIMER2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear TIMER2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
}

void Timer2A_Launch(void(*task)(void), uint32_t period, unsigned long priority) {
	PeriodicTask = task;          // user function
	TIMER2_TAILR_R = period-1;    // 4) reload value
	priority = priority << 29;
	NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)| priority; // 8) Timer priority is based off of priority of thread
	TIMER2_CTL_R = 0x00000001;    // 10) enable TIMER2A  
}

void Timer2A_Handler(void){
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge timer2A timeout
	(*PeriodicTask)();
}


// ***************** Timer2B_Init ****************
// Activate TIMER0 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer2B_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
	delay = SYSCTL_RCGC2_R;						// settle
  TIMER2_CTL_R = 0x00000000;    // 1) disable TIMER2B during setup
  TIMER2_CFG_R = 0x00000004;    // 2) configure for 32-bit mode
  TIMER2_TBMR_R = 0x02;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TBPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear TIMER2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
}

void Timer2B_Launch(void(*task)(void), uint32_t period, unsigned long priority) {
	PeriodicTask = task;          // user function
	TIMER2_TBILR_R = period-1;    // 4) reload value
	priority = priority << 5;
	NVIC_PRI6_R = (NVIC_PRI6_R&0xFFFFFF00)| priority; // 8) Timer priority is based off of priority of thread
	TIMER2_CTL_R = 0x00000001;    // 10) enable TIMER2A  
}

void Timer2B_Handler(void){
  TIMER2_ICR_R = TIMER_ICR_TBTOCINT;// acknowledge timer2A timeout
	(*PeriodicTask)();
}