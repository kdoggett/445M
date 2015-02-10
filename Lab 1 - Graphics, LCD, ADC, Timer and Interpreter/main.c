#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"
#include "PLL.h"
#include "UART.h"
#include "tm4c123gh6pm.h"
#include "ADC.h"
#include "OS.h"

#define COMMAND_MAX	100
#define NUM_SAMPLES 10

char command[COMMAND_MAX];

void dummy(void){
		unsigned long time = OS_ReadPeriodicTimer();
		UART_OutUDec(time);
}

void ProcessCommand(char *command){
	char commandType[COMMAND_MAX];
// Initialize commandType buffer
	for(int j = 0; j < COMMAND_MAX; j++) {
		commandType[j] = 0;
	}
	uint32_t i = 0;
	char commandNum;
	while(1) {
		if(command[i] == ' ')
		{
			break;
		}
		else if(command[i] == NULL) {
			break;
		}
		else {
			commandType[i] = command[i];
		}
		i++;
	}
	if (strcmp(commandType,"ADC") == 0){
		commandType[i] = ' ';
		i++;
		commandNum = 1;
	}
	if (strcmp(commandType,"Timer") == 0){
		commandType[i] = ' ';
		i++;
		commandNum = 2;
	}
	if (strcmp(commandType,"LCD") == 0){
		commandType[i] = ' ';
		i++;
		commandNum = 3;
	}
		switch(commandNum){
			case 1:
				uint32_t ADCValue = ADC_In();
				ST7735_Message(1,3,commandType, ADCValue);
				UART_OutUDec(ADCValue);
				break;
			case 2:
				while(command[i] != 0){
					commandType[i] = command[i];
					i++;
				}
				UART_OutString(commandType);
				break;
			case 3:
				while(command[i] != 0){
					commandType[i] = command[i];
					i++;
				}
				UART_OutString(commandType);
				ST7735_Message(0,3,commandType,0);
				break;
			default:
				ST7735_Message(2,1,"Default",1);
				UART_OutString(commandType);
				break;
		}
		i = 0;
}

int main(void){
	PLL_Init();
	UART_Init();
	PortF_Init();
	ADC_Open(4, 80000000);
	ST7735_InitR(INITR_REDTAB);
	
	UART_OutString("UART works\n\n");		//LCD is working
	ST7735_Message(0,0,"Screen works",1);
	
	/********* PROCEDURE ONE - I/O Redirect & Intepreter 
	Description: The I/O is redirect in UART.c when we copy data in the software FIFO to the hardware (UART0 register)
	*/


	
	/********* PROCEDURE TWO - LCD Driver
	Description: Below functions test writing to each line of the seperate screens (2 screens, 3 lines each)
	*/
	
	//	ST7735_Message(1,1,"Screen works",0);
	
		ST7735_DrawFastHLine(0, 80, 128, ST7735_YELLOW);
		ST7735_Message(0,0,"Line 1:",1);
		ST7735_Message(0,1,"Line 2:",2);
		ST7735_Message(0,2,"Line 3:",3);
		ST7735_Message(0,3,"Line 4:",4);
		ST7735_Message(0,3,"a:",12345);
		ST7735_Message(1,0,"Line 5:",5);
		ST7735_Message(1,1,"Line 6:",6);
		ST7735_Message(1,2,"Line 7:",7);
		ST7735_Message(1,3,"Line 8:",8);
	
	/********* PROCEDURE THREE - ADC Sampling
	Description: Collects NUM_SAMPLES on any of the 12 ADC0 channels using sequence 3 with Tomer0A interrupts
	*/
	
	/*
	int i = 0;
  unsigned short DataBuffer[NUM_SAMPLES] = {0};
	ADC_Collect(4,10000,DataBuffer,NUM_SAMPLES);
	for(i = 0;i < NUM_SAMPLES;i++){
		UART_OutUDec(DataBuffer[i]);
		UART_OutChar('\n');
	}
	*/
	
	/********* PROCEDURE FOUR - Periodic Timer User Function
	Description: Uses Timer5 to implement a simple interrupt for triggering any user task 
	*/

	
	//OS_AddPeriodicThread(*dummy,1000000,0);	//test for periodic timer implementation
	
	
	while(1){
		
		/******* INTERPRETER */
		while(RxFifo_Size() == 0){};					//temp way to see if we have a command (will get much more sophisticated in lab 2)
		UART_InString(command,COMMAND_MAX);		//get the command from the UART console
		ProcessCommand(command);							//submit the command for parsing and interpretation
		
	}
		
}
