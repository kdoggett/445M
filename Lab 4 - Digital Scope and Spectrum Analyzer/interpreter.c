#include <string.h>
#include "ST7735.h"
#include "interpreter.h"
#include "UART.h"
#include "ADC.h"
#include "pins.h"

#define TIME_1MS    80000        
#define PERIOD_100Hz	TIME_2MS*5
#define TIME_2MS    (2*TIME_1MS)  
#define PERIOD_12kHZ		78*80
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/5) 

#define	NO	0
#define	YES	1

void printCommands(void){
	UART_OutString("\n\nCommands: \n");
	UART_OutString("1. Trigger Type\n");
	UART_OutString("2. Enable/Disable ADC\n");
	UART_OutString("3. Print ADC input and FFT calculations\n");	
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
				UART_OutString("\n\nChoose trigger type: \n");
				UART_OutString("1. Hardware sample\n");
				UART_OutString("2. Software sample\n");
			}
			if(strcmp(command,"2") == 0){
				commandNum = 2;
				UART_OutString("1. Disable ADC\n");
				UART_OutString("2. Enable ADC\n");		
			}
			if(strcmp(command,"3") == 0){
				commandNum = 3;
				printADC_FFT();
			}
	}
	else if(processCommand == YES){
		if(commandNum == 1){
			if(strcmp(command,"1") == 0){
				ADC_HardwareTrigger_T0A(PERIOD_100Hz);
				UART_OutString("\nHardware Sample Active\n");
			}
			else{
				ADC_SoftwareTrigger();
				UART_OutString("\nSoftware Sample Active\n");
			}
		  processCommand = NO;
			printCommands();
		}
	}
}