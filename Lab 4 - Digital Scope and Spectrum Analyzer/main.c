//Lab 4

#include "OS.h"
#include "pins.h"
#include "tm4c123gh6pm.h"

#define TIME_1MS    80000          
#define TIME_2MS    (2*TIME_1MS)  
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/5) 

void PT_1(void){
	DIO1 ^= BIT1;
}

void T_1(void){
	//for(;;){
		DIO1 ^= BIT1;
	//}
	OS_Sleep(100);
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
	//OS_AddPeriodicThread(&PT_1,TIME_500US,3,1);
	OS_AddThread(&T_1,128,2);
	OS_AddThread(&T_2,128,2);
	OS_AddThread(&T_3,128,2);
	OS_Launch(TIME_2MS);
	return 0;
}