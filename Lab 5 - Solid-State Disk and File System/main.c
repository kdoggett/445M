//Lab 5
//Solid-state disk and File System

#include "OS.h"
#include "pins.h"
#include "tm4c123gh6pm.h"
#include "UART.h"
#include <string.h> 
#include "interpreter.h"
#include "ST7735.h"
#include "edisk.h"

#define TIME_1MS    80000          
#define TIME_2MS    (2*TIME_1MS) 
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/5) 

//******* Switch Press *******
//spawns new foreground thread

void SW1_Work(void){ 
	OS_Kill();
}

void SW1Push(void){
	OS_AddThread(&SW1_Work,128,3);
}

//******* Robot *********

void robot(void){
		for(;;){
		}
}

//******* Interpreter *********

char command[COMMAND_MAX];

void interpreter(void){        
	for(;;){
		while(RxFifo_Size() == 0){};
			UART_InString(command,COMMAND_MAX);
			ProcessCommand(command);
	}
}

void PortF_Init(void){  unsigned long volatile delay;
  SYSCTL_RCGCGPIO_R |= 0x20; // activate port F
  delay = SYSCTL_RCGCGPIO_R;
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R |= 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_DIR_R = 0x0E;     // make PF3-1 output (PF3-1 built-in LEDs),PF4,0 input
  GPIO_PORTF_PUR_R = 0x11;     // PF4,0 have pullup
  GPIO_PORTF_AFSEL_R = 0x00;   // disable alt funct on PF4-0
  GPIO_PORTF_DEN_R = 0x1F;     // enable digital I/O on PF4-0
  GPIO_PORTF_PCTL_R = 0x00000000;
  GPIO_PORTF_AMSEL_R = 0;      // disable analog functionality on PF
}


// PF1 is profile for eDisk_WriteBlock
// PF2 on for running, low for error
// PF3 is profile for eDisk_ReadBlock
//unsigned char buffer[512];
//#define MAXBLOCKS 100
//#define PF1  (*((volatile unsigned long *)0x40025008))
//#define PF2  (*((volatile unsigned long *)0x40025010))
//#define PF3  (*((volatile unsigned long *)0x40025020))

//void diskError(char* errtype, unsigned long n){
//  PF2 = 0x00;      // turn LED off to indicate error
//	//ST7735_OutString(errtype);
//	if(n == 1){
//		ST7735_OutString("Read/Write Error");
//	}
//	else if(n == 2){
//		ST7735_OutString("Write Protected");
//	}
//	else if(n == 3){
//		ST7735_OutString("Not Ready");
//	}
//	else if(n == 4){
//		ST7735_OutString("Invalid Parameter");
//	}
//  while(1){};
//}
//void TestDisk(void){  DSTATUS result;  unsigned short block;  int i; unsigned long n;
//  // simple test of eDisk
//  result = eDisk_Init(0);  // initialize disk
//  if(result) diskError("eDisk_Init",result);
//  n = 1;    // seed
//  for(block = 0; block < MAXBLOCKS; block++){
//    for(i=0;i<512;i++){
//      n = (16807*n)%2147483647; // pseudo random sequence
//      buffer[i] = 0xFF&n;
//    }
//    PF1 = 0x02;
//    if(eDisk_WriteBlock(buffer,block))diskError("eDisk_WriteBlock",block); // save to disk
//    PF1 = 0;
//  }
//  n = 1;  // reseed, start over to get the same sequence
//  for(block = 0; block < MAXBLOCKS; block++){
//    PF3 = 0x08;
//    if(eDisk_ReadBlock(buffer,block))diskError("eDisk_ReadBlock",block); // read from disk
//    PF3 = 0;
//    for(i=0;i<512;i++){
//      n = (16807*n)%2147483647; // pseudo random sequence
//      if(buffer[i] != (0xFF&n)){
//          PF2 = 0x00;   // turn LED off to indicate error
//      }
//    }
//  }
//}

int main1(void){ int i = 0;
	OS_Init();
	eDisk_Init(0);
	PortF_Init();
	PF2 = 0x04;    // turn blue LED on
	while(1){
    TestDisk();
    i = i + 1;
  }
	OS_AddThread(&robot,128,3);
	OS_AddThread(&interpreter,128,3);
	OS_Launch(TIME_2MS);
	return 0;
}