//Lab 4

#include "OS.h"
#include "pins.h"
#include "tm4c123gh6pm.h"
<<<<<<< HEAD
#include "ST7735.h"
#include "UART.h"
#include <string.h> 
#include "interpreter.h"
#include "ADC.h"
=======
#include "FIFO.h"
#include "stdint.h"
#include "stdio.h"
#include "ADC.h"
#include "UART.h"
>>>>>>> aa8c9ef2009c12bb4266218f976647f167100e91

#define TIME_1MS    80000          
#define TIME_2MS    (2*TIME_1MS)  
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/5)
#define Timer2 			2
#define Timer3			3
#define BUFFER_MAX 100

<<<<<<< HEAD
//******* Switch Press *******
//spawns new foreground thread

int buttonPress = 0;

void SW1_Work(void){ unsigned long ADCtest;
	buttonPress++;
	ADCtest = OS_Fifo_Get();
	ST7735_Message(1,2,"ADC Value: ",ADCtest);
	ST7735_Message(1,1,"Button Presses: ",buttonPress);
	OS_Kill();
}

void SW1Push(void){
	OS_AddThread(&SW1_Work,128,3);
=======
AddIndexFifo(ADCBuffer, 1000, uint32_t, FIFOSUCCESS, FIFOFAIL)
void ProcessCommand(char*);
char inputBuffer[BUFFER_MAX];

void SampleADC(void){
	DIO1 ^= BIT1;
	ADCBufferFifo_Put(ADCvalue);
}

void Interpreter (void) {
	while (RxFifo_Size() == 0){};
	UART_InString(inputBuffer, BUFFER_MAX);
	ProcessCommand(inputBuffer);
}
void T_1(void){
	for(;;){
		DIO1 ^= BIT1;
		OS_Sleep(10);
	}
>>>>>>> aa8c9ef2009c12bb4266218f976647f167100e91
}

//******* Interpreter *********

char command[COMMAND_MAX];

void Interpreter(void){        
	for(;;){
		while(RxFifo_Size() == 0){DIO1 ^= BIT1;};
			UART_InString(command,COMMAND_MAX);
			ProcessCommand(command);
	}
}

void DummyThread(void){
	for(;;){
		DIO4 ^= BIT4;
		OS_Sleep(2);
	};
	
}


int main(void){
	OS_Init();
<<<<<<< HEAD
	OS_AddThread(&Interpreter,128,3);		// runs continously
	OS_AddThread(&DummyThread,128,3);
	OS_AddSW1Task(&SW1Push,3);					// print one frame to the LCD
=======
	OS_AddPeriodicThread(&SampleADC,TIME_1MS,3,Timer2);
	OS_AddThread(&T_1,128,2);
	OS_AddThread(&T_2,128,2);
	OS_AddThread(&T_3,128,2);
>>>>>>> aa8c9ef2009c12bb4266218f976647f167100e91
	OS_Launch(TIME_2MS);
	return 0;
}

void ProcessCommand(char *uartInput){
	char command[BUFFER_MAX];
// Initialize commandType buffer
	for(int j = 0; j < BUFFER_MAX; j++) {
		command[j] = 0;
	}
	uint32_t i = 0;
	char commandNum;
	while(1) {
		if(uartInput[i] == ' ')
		{
			break;
		}
		else if(uartInput[i] == NULL) {
			break;
		}
		else {
			command[i] = uartInput[i];
		}
		i++;
	}
	if (strcmp(command,"ADC") == 0){
		command[i] = ' ';
		i++;
		commandNum = 1;
	}
switch(commandNum){
			case 1:
				ST7735_Message(1,3,command, ADCvalue);
				UART_OutUDec(ADCvalue);
				break;
			default:
				ST7735_Message(2,1,"Invalid Command",1);
				UART_OutString("Invalid Command");
				break;
		}
		i = 0;
}