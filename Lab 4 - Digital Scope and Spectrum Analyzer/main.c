//Lab 4

#include "OS.h"
#include "pins.h"
#include "tm4c123gh6pm.h"
#include "FIFO.h"
#include "stdint.h"
#include "ADC.h"

#define TIME_1MS    80000          
#define TIME_2MS    (2*TIME_1MS)  
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/5)
#define Timer2 			2
#define Timer3			3

AddIndexFifo(ADCBuffer, 1000, uint32_t, FIFOSUCCESS, FIFOFAIL)

void SampleADC(void){
	DIO1 ^= BIT1;
	ADCBufferFifo_Put(ADCvalue);
}

void T_1(void){
	for(;;){
		DIO1 ^= BIT1;
		OS_Sleep(10);
	}
}

void T_2(void){
	for(;;){
		DIO2 ^= BIT2;
	}
}

void T_3(void){
	for(;;){
		DIO3 ^= BIT3;
	}
}

int main(void){
	OS_Init();
	OS_AddPeriodicThread(&SampleADC,TIME_1MS,3,Timer2);
	OS_AddThread(&T_1,128,2);
	OS_AddThread(&T_2,128,2);
	OS_AddThread(&T_3,128,2);
	OS_Launch(TIME_2MS);
	return 0;
}