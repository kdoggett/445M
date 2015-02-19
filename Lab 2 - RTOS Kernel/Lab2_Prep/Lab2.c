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

Sema4Type displayReady;
Sema4Type	UARTReady;

int main(void){
	PLL_Init();
	UART_Init();

	OS_InitSemaphore(&displayReady,1);
	OS_InitSemaphore(&UARTReady,1);
	
	OS_Wait(&displayReady);
	
	
	
	UART_OutString("UART works\n\n");
	
	
	
	while(1){}
	
	return 0;
}