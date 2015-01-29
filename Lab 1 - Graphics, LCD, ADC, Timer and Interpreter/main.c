#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"
#include "PLL.h"
#include "UART.h"
#include "tm4c123gh6pm.h"
#include "ADC.h"
#include "OS.h"

#define COMMAND_MAX	10


char command[COMMAND_MAX];

void dummy(void){
		unsigned long time = OS_ReadPeriodicTimer();
		//UART_OutUDec(time);
}

void ProcessCommand(char *command){ int commandNum;
	if (strcmp(command,"ADC") == 0){
		commandNum = 1;
	}
	if (strcmp(command,"Timer") == 0){
		commandNum = 2;
	}
	if (strcmp(command,"LCD") == 0){
		commandNum = 3;
	}
		switch(commandNum){
			case 1:
				ST7735_Message(1,1,"ADC Debug:",1);
				UART_OutString(command);
				break;
			case 2:
				ST7735_Message(1,2,"Timer Debug:",2);
				UART_OutString(command);
				break;
			case 3:
				ST7735_Message(1,3,"LCD Debug:",3);
				UART_OutString(command);
				break;
			default:
				ST7735_Message(2,1,"Default",1);
				UART_OutString(command);
				break;
		}
	}

int main(void){
	char i;
	PLL_Init();
	UART_Init();
	ST7735_InitR(INITR_REDTAB);
	
	/*PROCEDURE ONE 
	I/O Redirect & Intepreter 
	*/

	UART_OutString("working\n\n");		//LCD is working
	
	/*PROCEDURE TWO
	LCD Driver
	Below functions test writing to each line of the seperate screens (2 screens, 3 lines each)
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
	
  unsigned short DataBuffer[] = {0};
	
	ADC_Collect(1,1000,DataBuffer,64);  //channel 1, 10 kHz sample rate, store in DataBuffer, 64 samples
	
	/*PROCEDURE FOUR
	Periodic Timer
	*/

	OS_AddPeriodicThread(*dummy,10000000,0);	//test for periodic timer implementation
	
	/*loop forever*/
	
	while(1){
		
		while(RxFifo_Size() == 0){};					//temp way to see if we have a command (will get much more sophisticated in lab 2)
		UART_InString(command,COMMAND_MAX);		//get the command from the UART console
		ProcessCommand(command);							//submit the command for parsing and interpretation
	}
	
}