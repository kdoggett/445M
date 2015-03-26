//Lab 4

#include "OS.h"
#include "pins.h"
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "UART.h"
#include <string.h> 
#include "interpreter.h"
#include "ADC.h"
#define PERIOD_12kHZ		78*80

#define TIME_1MS    80000          
#define TIME_2MS    (2*TIME_1MS) 
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/5) 

//******* Switch Press *******
//spawns new foreground thread

int buttonPress = 0;
uint32_t ADCtest;
unsigned int *data;
void SW1_Work(void){ 
	ADCtest = OS_Fifo_Get();
	buttonPress++;
	ST7735_Message(1,1,"Button Presses: ",buttonPress);
	ST7735_Message(1,2,"ADCtest: ",ADCtest);
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

void Graph(void){
<<<<<<< HEAD
		for(;;){
			DIO4 ^= BIT4;
			ADCtest = OS_Fifo_Get();
			ST7735_PlotPoint(ADCtest);
			ST7735_PlotNext();
		}
=======
		DIO4 ^= BIT4;
		ST7735_PlotClear(0,4095);  // range from 0 to 4095
		ST7735_PlotPoint(OS_Fifo_Get()); 
		ST7735_PlotNext();
>>>>>>> 142823331d0342f6890f8430cb779b4751525bb8
}


int main(void){
	OS_Init();
<<<<<<< HEAD
	//OS_AddThread(&Interpreter,128,3);		// runs continously
	ADC_HardwareTrigger_T0A(PERIOD_12kHZ);
=======
	OS_AddThread(&Interpreter,128,3);		// runs continously
>>>>>>> 142823331d0342f6890f8430cb779b4751525bb8
	OS_AddThread(&Graph,128,3);
	OS_AddSW1Task(&SW1Push,3);					// print one frame to the LCD
	OS_Launch(TIME_2MS);
	return 0;
}