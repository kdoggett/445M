//Lab 4

#include "OS.h"
#include "pins.h"
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "UART.h"
#include <string.h> 
#include "interpreter.h"
#include "ADC.h"

#define TIME_1MS    80000          
#define TIME_2MS    (2*TIME_1MS)  
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/5) 

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
	OS_AddThread(&Interpreter,128,3);		// runs continously
	OS_AddThread(&DummyThread,128,3);
	OS_AddSW1Task(&SW1Push,3);					// print one frame to the LCD
	OS_Launch(TIME_2MS);
	return 0;
}