//Lab 4

#include "OS.h"
#include "pins.h"
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "UART.h"
#include <string.h> 
#include "interpreter.h"
#include "ADC.h"
#include "MACQ.h"

#define PERIOD_12kHZ		78*80

#define TIME_1MS    80000
#define TIME_10S		800000000
#define TIME_1S			80000000
#define TIME_2MS    (2*TIME_1MS) 
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/5)

//******* Switch Press *******
//spawns new foreground thread

<<<<<<< HEAD
uint32_t ADCtest;
=======
int buttonPress = 0;
int32_t ADCtest;
>>>>>>> 1c1990f348784d2f710a9c553a3d1b67ac98aee5
unsigned int *data;

void SW1_Work(void){ 
	DisableInterrupts();
	ST7735_PlotClear(0,4095);
	int i = 0;
	for (i = 0;i < 64;i++){
		ADCtest = OS_Fifo_Get();
		ST7735_PlotPoint(ADCtest);
		ST7735_PlotNext();
		OS_Sleep(50);
	}
	EnableInterrupts();
	OS_Kill();
}

void SW1Push(void){
	OS_AddThread(&SW1_Work,128,3);
}

//******* Interpreter *********

char command[COMMAND_MAX];

void dummyThread(void){
	for(;;){
		DIO1 ^= BIT1;
		//OS_Sleep(2);
	}
	
}

void Interpreter(void){        
	for(;;){
		while(RxFifo_Size() == 0){DIO1 ^= BIT1;};
			UART_InString(command,COMMAND_MAX);
			ProcessCommand(command);
	}
}

void Graph(void){
		for(;;){
			DIO3 ^= BIT3;
			//if(Graph_Type == VvT){
<<<<<<< HEAD
			ADCtest = OS_Fifo_Get();
			ST7735_PlotPoint(ADCtest);
			ST7735_PlotNext();
=======
				ADCtest = MACQ_Get();
				ST7735_PlotPoint(ADCtest);
				ST7735_PlotNext();
>>>>>>> 1c1990f348784d2f710a9c553a3d1b67ac98aee5
			OS_Sleep(10);
			//}
			//if(Graph_Type == VvF){
			
			//}
		}
}

int main(void){
	OS_Init();
	ST7735_PlotClear(0,4095);
	OS_AddThread(&Interpreter,128,3);		// runs continously
	OS_AddThread(&Graph,128,3);
	OS_AddSW1Task(&SW1Push,3);					// print one frame to the LCD
	OS_Launch(TIME_2MS);
	return 0;
}