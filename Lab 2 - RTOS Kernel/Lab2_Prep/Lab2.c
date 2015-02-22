// Lab2.c
// lab 2.2 prep
//	Demonstrates a semaphore implentation of the LCD and UART
//	OS_InitSemaphore
//	OS_Signal
//	OS_Wait
//	OS_bSignal
//	OS_bWait

#include "OS.h"
#include "PLL.h"
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "UART.h"


Sema4Type	UARTReady;
Sema4Type	displayReady;


unsigned long NumCreated;   // number of foreground threads created


	
unsigned long Count1;   // number of times thread1 loops
unsigned long Count2;   // number of times thread2 loops
unsigned long Count3;   // number of times thread3 loops

void PortE_Init(void){ unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x10;       // activate port E
  delay = SYSCTL_RCGC2_R;        
  delay = SYSCTL_RCGC2_R;         
  GPIO_PORTE_DIR_R |= 0x0F;    // make PE3-0 output heartbeats
  GPIO_PORTE_AFSEL_R &= ~0x0F;   // disable alt funct on PE3-0
  GPIO_PORTE_DEN_R |= 0x0F;     // enable digital I/O on PE3-0
  GPIO_PORTE_PCTL_R = ~0x0000FFFF;
  GPIO_PORTE_AMSEL_R &= ~0x0F;;      // disable analog functionality on PF
}

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


void Thread1b(void){       
  int i;	
  for(;;){
    PE2 ^= 0x04;
    ST7735_DrawString(0,0,"I am thread 1",ST7735_RED);
		for (i=0;i<10000;i++){}
  }
}
void Thread2b(void){       
  int i;
	for(;;){
    //PE3 ^= 0x08;
    ST7735_DrawString(0,2,"I am thread 2",ST7735_RED);		
		for (i=0;i<10000;i++){}
		
  }
}
void Thread3b(void){         
  int i;
	for(;;){
    PF1 ^= 0x02;
    ST7735_DrawString(0,3,"I am thread 3",ST7735_RED);		
		for (i=0;i<10000;i++){}
  }
}

int main(void){
	PLL_Init();
	UART_Init();
	PortE_Init();
	PortF_Init();
	ST7735_InitR(INITR_REDTAB);
	
	ST7735_DrawFastHLine(0, 80, 128, ST7735_YELLOW);
	UART_OutString("UART works\n\n");

	OS_InitSemaphore(&displayReady,1);
	OS_InitSemaphore(&UARTReady,1);
	
/* Questions 5 */
	OS_Init();
	NumCreated = 0;
	NumCreated += OS_AddThread(&Thread1b, 128, 1);
	NumCreated += OS_AddThread(&Thread2b, 128, 1);
	NumCreated += OS_AddThread(&Thread3b, 128, 1);
	OS_Launch(TIME_2MS);
	
/* Questios 6 */
	
	
	while(1){}
	
	return 0;
}