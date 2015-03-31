#include <string.h>
#include "ST7735.h"
#include "interpreter.h"
#include "tm4c123gh6pm.h"
#include "UART.h"
#include "ADC.h"
#include "pins.h"
#include "OS.h"

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
				UART_OutString("1. Voltage v. time\n");
				UART_OutString("2. Voltage v. frequency\n");		
			}
			if(strcmp(command,"3") == 0){
				commandNum = 3;
				printADC_FFT();
			}
	}
	else if(processCommand == YES){
		if(commandNum == 1){
			if(strcmp(command,"1") == 0){
				Output_Clear();
				ST7735_DrawString(0,0,"Amazing Scope",ST7735_RED);
				ST7735_DrawString(0,1,"Hardware Trigger",ST7735_RED);
				ST7735_DrawString(0,2,"Sample Rate: 100 Hz",ST7735_RED);
				ADC_HardwareTrigger_T0A(TIME_2MS*10);
			}
			else{
				Output_Clear();
				ST7735_DrawString(0,0,"Amazing Scope",ST7735_RED);
				ST7735_DrawString(0,1,"Software Trigger",ST7735_RED);
				ST7735_DrawString(0,2,"Sample Rate: 100 Hz",ST7735_RED);
				ADC_SoftwareTrigger();
				UART_OutString("\nSoftware Sample Active\n");
			}
		  processCommand = NO;
			printCommands();
		}
	}
}