#include <string.h>
#include "interpreter.h"
#include "UART.h"
#include "pins.h"
#include "efile.h"

#define TIME_1MS    80000          
#define TIME_2MS    (2*TIME_1MS)  
#define PERIOD_12kHZ		78*80
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/5) 

#define	NO	0
#define	YES	1

void printCommands(void){
	UART_OutString("\n\nCommands: \n");
	UART_OutString("1. Format\n");
	UART_OutString("2. Directory\n");
	UART_OutString("3. Print File\n");
	UART_OutString("4. Delete File\n");	
}

void printADC_FFT(void){
	
}

int commandNum = 0;		//(1) Hardware/software trigger, (2) Enable/Disable, (3) Print ADC Info
int processCommand = 0;

void ProcessCommand(char *command){
	if(processCommand == NO){
		processCommand = YES;
			if(strcmp(command,"1") == 0){
				commandNum = 1;
				eFile_Format();
			}
			if(strcmp(command,"2") == 0){
				commandNum = 2;
				//eFile_Directory(
			}
			if(strcmp(command,"3") == 0){
				commandNum = 3;
				//print file
			}
			if(strcmp(command,"4") == 0){
				commandNum = 4;
				//delete file
			}
	}
	else if(processCommand == YES){
		if(commandNum == 1){
			if(strcmp(command,"1") == 0){
				UART_OutString("\nHardware Sample Active\n");
			}
			else{
				UART_OutString("\nSoftware Sample Active\n");
			}
		  processCommand = NO;
			printCommands();
		}
	}
}