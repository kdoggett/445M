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
#include "sqrt.h"

#define PERIOD_12kHZ		78*80

#define TIME_1MS    80000
#define TIME_10S		800000000
#define TIME_1S			80000000
#define TIME_2MS    (2*TIME_1MS) 
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/5)

void cr4_fft_1024_stm32(void *pssOUT, void *pssIN, unsigned short Nbin);

//******* Switch Press *******
//spawns new foreground thread

int buttonPress = 0;
int32_t ADCtest;
unsigned int *data;

int switchADC[64] = {0};

void SW1_Work(void){ 
	Output_Clear();
	int i = 0;
	for (i = 0;i < 64;i++){
		switchADC[i] = MACQ_Get();
	}
	DisableInterrupts();
	ST7735_DrawString(0,0,"Amazing Scope",ST7735_RED);
	ST7735_DrawString(0,1,"Button Sample",ST7735_RED);
	ST7735_DrawString(0,2,"Sample Count: 64",ST7735_RED);
	for(i = 0;i < 64;i++){
		ST7735_PlotPoint(switchADC[i]);
		ST7735_PlotNext();
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
				ADCtest = MACQ_Get();
				ST7735_PlotPoint(ADCtest);
			//}
			//if(Graph_Type == VvF){
				int32_t x[1024],y[1024];      // input and output arrays for FFT
				for(int t = 0; t < 1024; t++){
					int32_t data = MACQ_Get();
					x[t] = data;
				}
				cr4_fft_1024_stm32(y,x,1024);
				int32_t real,imag,data;
				for(int t = 0; t < 512; t++){
					real = y[t] & 0xFFFF;
					imag = y[t] >> 16;
					data = sqrt(real*real + imag*imag);
					ST7735_PlotdBfs(data);
					if((t%4) == 3){
						ST7735_PlotNext();
					}
				}
			//}
			ST7735_PlotNext();
			OS_Sleep(10);
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