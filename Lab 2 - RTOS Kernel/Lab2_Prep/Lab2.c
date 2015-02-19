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

/******************************************/
/* Global Semphores for Different Threads */
/******************************************/
			Sema4Type displayReady;
			Sema4Type	UARTReady;
/******************************************/
/******************************************/


unsigned long NumCreated;   // number of foreground threads created


void Thread1 (void) {/*Do Nothing*/}
void Thread2 (void) {/*Do Nothing*/}
void Thread3 (void) {/*Do Nothing*/}

int main(void){
	PLL_Init();
	UART_Init();
/* Lab2 Part 2 */

/* Questions 4 */
	OS_InitSemaphore(&displayReady,1);
	OS_InitSemaphore(&UARTReady,1);
	OS_Signal(&displayReady);
	OS_Signal(&UARTReady);
	OS_Wait(&displayReady);
	OS_Wait(&UARTReady);
	OS_bSignal(&displayReady);
	OS_bSignal(&UARTReady);
	
/* Questions 5 */
	OS_Init();
	NumCreated = 0;
	NumCreated += OS_AddThread(&Thread1, 128, 1);
	NumCreated += OS_AddThread(&Thread2, 128, 1);
	NumCreated += OS_AddThread(&Thread3, 128, 1);
	OS_Launch(TIME_2MS);
	
/* Questios 6 */

	
	UART_OutString("UART works\n\n");
	
	
	
	while(1){}
	
	return 0;
}