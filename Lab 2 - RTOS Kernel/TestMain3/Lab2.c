// Lab2.c
// testmain3

#include "OS.h"
#include "tm4c123gh6pm.h"
#include "pins.h"

unsigned long NumCreated;   // number of foreground threads created

unsigned long Count1;   // number of times thread1 loops
unsigned long Count2;   // number of times thread2 loops
unsigned long Count3;   // number of times thread3 loops
unsigned long Count4;   // number of times thread4 loops
unsigned long Count5;   // number of times thread5 loops

//*******************Third TEST**********
// Once the second test runs, test this (Lab 1 part 2)
// no UART1 interrupts
// SYSTICK interrupts, with or without period established by OS_Launch
// Timer interrupts, with or without period established by OS_AddPeriodicThread
// PortF GPIO interrupts, active low
// no ADC serial port or LCD output
// tests the spinlock semaphores, tests Sleep and Kill
Sema4Type Readyc;        // set in background
int Lost;
void BackgroundThread1c(void){   // called at 1000 Hz
	PC4_DIO4 ^= 0x10;
  Count1++;
  OS_Signal(&Readyc);
}
void Thread5c(void){
  for(;;){
		PC5_DIO5 ^= 0x20;
    OS_Wait(&Readyc);
    Count5++;   // Count2 + Count5 should equal Count1 
    Lost = Count1-Count5-Count2;
  }
}
void Thread2c(void){

  OS_InitSemaphore(&Readyc,0);
  Count1 = 0;    // number of times signal is called      
  Count2 = 0;    
  Count5 = 0;    // Count2 + Count5 should equal Count1  
  NumCreated += OS_AddThread(&Thread5c,128,5); 
  OS_AddPeriodicThread(&BackgroundThread1c,TIME_1MS,0); 
  for(;;){
		PE0_DIO0 ^= 0x01;       // heartbeat
    OS_Wait(&Readyc);
    Count2++;   // Count2 + Count5 should equal Count1
  }
}

void Thread3c(void){
	
  Count3 = 0;          
  for(;;){
		PE1_DIO1 ^= 0x02;       // heartbeat
    Count3++;
  }
}
void Thread4c(void){
	
  Count3 = 0;          
  for(;;){
		PE2_DIO2 ^= 0x04;       // heartbeat
    Count3++;
		//OS_Sleep(10);
  }
}

void BounceWait(void){ int i;
	PC7_DIO7 ^= 0x80;
  for(i=0;i<64;i++){
    Count4++;
    OS_Sleep(10);
  }
  OS_Kill();
  Count4 = 0;
}
void BackgroundThread5c(void){   // called when Select button pushed
	PC6_DIO6 ^= 0x40;
  //NumCreated += OS_AddThread(&BounceWait,128,6); 
}
      
int main(void){   // Testmain3
	Debug_Port_Init();
  Count4 = 0;          
  OS_Init();           // initialize, disable interrupts
// Count2 + Count5 should equal Count1
  NumCreated = 0 ;
  OS_AddSW1Task(&BackgroundThread5c,2);
  NumCreated += OS_AddThread(&Thread2c,128,2); 
  NumCreated += OS_AddThread(&Thread3c,128,3); 
	NumCreated += OS_AddThread(&Thread4c,128,4); 
  //NumCreated += OS_AddThread(&BounceWait,128,6); 
  OS_Launch(TIME_2MS); // doesn't return, interrupts enabled in here
  return 0;            // this never executes
}