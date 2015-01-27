#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"
#include "PLL.h"
#include "UART.h"
#include "tm4c123gh6pm.h"
#include "ADC.h"
#include "OS.h"

void dummy(void){
		unsigned long time = OS_ReadPeriodicTimer();
		UART_OutUDec(time);
}

int main(void){
	char i;
	PLL_Init();
	UART_Init();
	ST7735_InitR(INITR_REDTAB);
	
	/*PROCEDURE ONE 
	I/O Redirect & Intepreter 
	*/
  for(i='A'; i<='Z'; i=i+1){
    UART_OutChar(i);
  }
	
	/*PROCEDURE TWO
	LCD Driver  
	*/
//	ST7735_DrawFastHLine(0, 80, 128, ST7735_YELLOW);
//	ST7735_Message(1,1,"Line 1:",1);
//	ST7735_Message(1,2,"Line 2:",2);
//	ST7735_Message(1,3,"Line 3:",3);
//	ST7735_Message(2,1,"Line 4:",4);
//	ST7735_Message(2,2,"Line 5:",5);
//	ST7735_Message(2,3,"Line 6:",6);
	
	/*PROCEDURE THREE
	Testing ADC  	
	*/
	
//  unsigned short DataBuffer[] = {0};
	
//	ADC_Collect(1,1000,DataBuffer,64);  //channel 1, 10 kHz sample rate, store in DataBuffer, 64 samples
	
	/*PROCEDURE FOUR
	Periodic Timer
	*/

	OS_AddPeriodicThread(*dummy,10000000,0);
	
	while(1){
	}
	
	
}