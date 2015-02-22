#include "OS.h"
#include "PLL.h"
#include "tm4c123gh6pm.h"
#include <stdint.h>

/*--------- TCB Stucture ---------*/

#define NUMTHREADS  3        // maximum number of threads
#define STACKSIZE   100      // number of 32-bit words in stack

struct tcb{
  int32_t 			*sp;      			// pointer to stack (valid for threads not running
  struct tcb		*next;  				// linked-list pointer to next
	struct tcb		*prev;					// linked-list pointer to previous
	char 					ID;							// identifies thread
	char					sleepState;			// sleep status
	char					priority;				// priority of thread
	char					blockedState;		// blocked status
};
typedef struct tcb tcbType;			// typedef tcb as tcbType
tcbType tcbs[NUMTHREADS];				// allocate memory for NUMTHREADS threads
tcbType *RunPt;									// pointer to running thread
int32_t Stacks[NUMTHREADS][STACKSIZE];		// allocate memory on stack outside TCB

/*--------- Set Up Initial Stack for Thread ---------*/

void SetInitialStack(int i, int stackSize){
  tcbs[i].sp = &Stacks[i][stackSize-16]; // thread stack pointer
  Stacks[i][stackSize-1] = 0x01000000;   // thumb bit
  Stacks[i][stackSize-3] = 0x14141414;   // R14
  Stacks[i][stackSize-4] = 0x12121212;   // R12
  Stacks[i][stackSize-5] = 0x03030303;   // R3
  Stacks[i][stackSize-6] = 0x02020202;   // R2
  Stacks[i][stackSize-7] = 0x01010101;   // R1
  Stacks[i][stackSize-8] = 0x00000000;   // R0
  Stacks[i][stackSize-9] = 0x11111111;   // R11
  Stacks[i][stackSize-10] = 0x10101010;  // R10
  Stacks[i][stackSize-11] = 0x09090909;  // R9
  Stacks[i][stackSize-12] = 0x08080808;  // R8
  Stacks[i][stackSize-13] = 0x07070707;  // R7
  Stacks[i][stackSize-14] = 0x06060606;  // R6
  Stacks[i][stackSize-15] = 0x05050505;  // R5
  Stacks[i][stackSize-16] = 0x04040404;  // R4
}

/*--------- OS Functions for Test Main 3 ----------*/

// function definitions in osasm.s
void OS_DisableInterrupts(void); // Disable interrupts
void OS_EnableInterrupts(void);  // Enable interrupts
int32_t StartCritical(void);
void EndCritical(int32_t primask);
void StartOS(void);

int stackNum = 0;
int threadMaxed = 0;

int OS_AddThread(void(*task)(void), unsigned long stackSize, unsigned long priority){ 
	int32_t status; 
	status = StartCritical();
	if(stackNum == 2){
		tcbs[stackNum].next = &tcbs[0];
	}
	else{
	tcbs[stackNum].next = &tcbs[stackNum + 1]; // first thread points to next thread
	}
	tcbs[stackNum].priority = priority;
	SetInitialStack(stackNum,stackSize); 
	Stacks[stackNum][stackSize-2] = (int32_t)(task); // PC
	stackNum++;
	if(stackNum > NUMTHREADS){
		threadMaxed = 1;
	}
	else {
		threadMaxed = 0;
	}
	EndCritical(status);
	return threadMaxed;
}

void OS_Init(void){
  DisableInterrupts();
  PLL_Init();                 // set processor clock to 80 MHz
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R =(NVIC_SYS_PRI3_R&0x00FFFFFF)|0xE0000000; // priority 7
}
	
void OS_Launch(unsigned long theTimeSlice){
	NVIC_ST_RELOAD_R = theTimeSlice - 1;
	NVIC_ST_CTRL_R = 0x07; // enable, core clock and interrupt arm
	RunPt = &tcbs[0];       // thread 0 will run first
	StartOS();	
}

void OS_Wait(Sema4Type *semaPt){}
void OS_Signal(Sema4Type *semaPt){}	
void OS_InitSemaphore(Sema4Type *semaPt, long value){}
int OS_AddPeriodicThread(void(*task)(void), 
   unsigned long period, unsigned long priority){}
int OS_AddSW1Task(void(*task)(void), unsigned long priority){}
void OS_Sleep(unsigned long sleepTime){}
void OS_Kill(void){}
	
	
/*---------- Future OS Functions -----------*/
	


void OS_bWait(Sema4Type *semaPt){}
void OS_bSignal(Sema4Type *semaPt){}
unsigned long OS_Id(void){}
int OS_AddSW2Task(void(*task)(void), unsigned long priority){}
void OS_Fifo_Init(unsigned long size){}
int OS_Fifo_Put(unsigned long data){}
unsigned long OS_Fifo_Get(void){}
long OS_Fifo_Size(void){}
void OS_MailBox_Init(void){}
void OS_MailBox_Send(unsigned long data){}
unsigned long OS_MailBox_Recv(void){}
unsigned long OS_Time(void){}
unsigned long OS_TimeDifference(unsigned long start, unsigned long stop){}
void OS_ClearMsTime(void){}
unsigned long OS_MsTime(void){}


