//Lab 5
//Solid-state disk and File System

#include "OS.h"
#include "pins.h"
#include "tm4c123gh6pm.h"
#include "UART.h"
#include <string.h> 
#include "interpreter.h"
#include "ST7735.h"

#define TIME_1MS    80000          
#define TIME_2MS    (2*TIME_1MS) 
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/5) 

//******* Switch Press *******
//spawns new foreground thread

void SW1_Work(void){ 
	OS_Kill();
}

void SW1Push(void){
	OS_AddThread(&SW1_Work,128,3);
}

//******* Robot *********

void robot(void){
		for(;;){
			ST7735_OutUDec(1);
		}
}

//******* Interpreter *********

char command[COMMAND_MAX];

void interpreter(void){        
	for(;;){
		while(RxFifo_Size() == 0){};
			UART_InString(command,COMMAND_MAX);
			ProcessCommand(command);
	}
}

int main(void){
	OS_Init();
	OS_AddThread(&robot,128,3);
	OS_AddThread(&interpreter,128,3);
	OS_Launch(TIME_2MS);
	return 0;
}