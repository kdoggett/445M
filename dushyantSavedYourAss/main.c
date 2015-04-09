//Lab 4
//#include <stdio.h>
#include <stdint.h> 
#include "OS.h"
#include "pins.h"
#include "tm4c123gh6pm.h"
#include "UART.h"
#include <string.h> 
#include "interpreter.h"
#include "ADC.h"

#define PERIOD_12kHZ		78*80

#define TIME_1MS    80000
#define TIME_10S		800000000
#define TIME_1S			80000000
#define TIME_2MS    (2*TIME_1MS) 
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/5)

//void cr4_fft_1024_stm32(void *pssOUT, void *pssIN, unsigned short Nbin);

//******* Switch Press *******
//spawns new foreground thread

int buttonPress = 0;
int32_t ADCtest;
unsigned int *data;

void dummy(void){ for(;;); }


int main(void){
	OS_Init();
	OS_AddSW1Task(&dummy,3);					// print one frame to the LCD
	OS_Launch(TIME_2MS);
	return 0;
}