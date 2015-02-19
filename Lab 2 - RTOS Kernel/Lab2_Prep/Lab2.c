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


unsigned long NumCreated;   // number of foreground threads created

#define PE0  (*((volatile unsigned long *)0x40024004))
#define PE1  (*((volatile unsigned long *)0x40024008))
#define PE2  (*((volatile unsigned long *)0x40024010))
	
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

void Thread1b(void){        
  for(;;){
    PE0 ^= 0x01;       // heartbeat
    ST7735_DrawString(0,0,"I am ",ST7735_RED);
  }
}
void Thread2b(void){       
  for(;;){
    PE1 ^= 0x02;       // heartbeat
    ST7735_DrawString(0,2,"I am thread 2",ST7735_RED);
  }
}
void Thread3b(void){         
  for(;;){
    PE2 ^= 0x04;       // heartbeat
    ST7735_DrawString(0,3,"I am thread 3",ST7735_RED);
  }
}

int main(void){
	PLL_Init();
	UART_Init();
	PortE_Init();
	ST7735_InitR(INITR_REDTAB);
	
	ST7735_DrawFastHLine(0, 80, 128, ST7735_YELLOW);
	UART_OutString("UART works\n\n");


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