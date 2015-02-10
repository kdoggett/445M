#include "OS.h"
#include "tm4c123gh6pm.h"

#define PF1                     (*((volatile unsigned long *)0x40025008))


void (*PeriodicTask)(void);

volatile unsigned char command_in[8];
void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R = 0x11;          // enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0
}

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
	PF1 = PF1^0x02;            // toggle red LED, PF1
	(*PeriodicTask)();
	PF1 = PF1^0x02;            // toggle red LED, PF1
}

void OS_ClearPeriodicTime(void){
	TIMER5_TAILR_R = 0;						//resets counter to 0, TAILR register
}

unsigned long OS_ReadPeriodicTimer(void){
	return TIMER5_TAILR_R;
}