#include "OS.h"
#include "PLL.h"
#include "tm4c123gh6pm.h"
#include "pins.h"
#include "timers.h"
#include <stdint.h>
#include "UART.h"

/*--------- TCB Stucture ---------*/

#define NUMTHREADS  10        // maximum number of threads
#define STACKSIZE   128      // number of 32-bit words in stack
#define SUCCESS			1
#define FAIL				0
#define	FIFO_SIZE		1000

struct tcb{
  int			 			*sp;      			// pointer to stack (valid for threads not running
  struct tcb		*next;  				// linked-list pointer to next
	struct tcb		*prev;					// linked-list pointer to previous
	int 					ID;							// identifies thread
	int						sleep;					// sleep status
	int						priority;				// priority of thread
	int						blockedState;		// blocked status
	int						empty;					// Tell whether or not a thread is empty.
};
typedef struct tcb tcb;			// typedef tcb as tcbType
tcb tcbs[NUMTHREADS];				// allocate memory for NUMTHREADS threads
tcb *RunPt;									// pointer to running thread
tcb *NextThread;						// pointer to next thread
int32_t Stacks[NUMTHREADS][STACKSIZE];		// allocate memory on stack outside TCB

void DisableInterrupts(void);
void EnableInterrupts(void);
int32_t StartCritical(void);
void EndCritical(int32_t primask);
void StartOS(void);

/*********** OS ADD TCB/STACK ***********/

int threadNum = 0;
int threadMaxed = 0;
int threadsCreated = 0;
tcb *firstThread = &tcbs[0];
int OS_AddThread(void(*task)(void), unsigned long stackSize, unsigned long priority){ 
	int32_t status; 
	status = StartCritical();
	threadsCreated++;
	int threadCount = 0;
	int threadNum = 0;
	for(int i = 0; i < NUMTHREADS; i++){
		if(tcbs[i].empty == 1){
			threadCount++;
		}
	}
	while(tcbs[threadNum].empty == 1){
		threadNum++;
		if(threadNum > NUMTHREADS) {break;}
	}
	
	/* Fill TCB */
	tcbs[threadNum].sp = &Stacks[threadNum][stackSize-16];	
	tcbs[threadNum].sleep = 0;
	tcbs[threadNum].priority = priority;
	if(threadCount == 0){					// only one thread in TCB, point to itself
		tcbs[0].next = &tcbs[0];
	}						
	else{ 												// second to last thread points to last and last points to first
		tcbs[threadNum].next = firstThread;					
		firstThread->prev->next = &tcbs[threadNum];
	}
	if(threadCount == 0) { 
		tcbs[0].prev = &tcbs[0];		// only one thread in TCB, point to itself
	}
	else { 												// first thread points to last thread and last thread points to second to last
		tcbs[threadNum].prev = firstThread->prev;					
		firstThread->prev = &tcbs[threadNum];
	}
	
	/* Fill Stack */
	Stacks[threadNum][stackSize-1] = 0x01000000;   // thumb bit
	Stacks[threadNum][stackSize-2] = (int32_t)(task); // PC
  Stacks[threadNum][stackSize-3] = 0x14141414;   // R14
  Stacks[threadNum][stackSize-4] = 0x12121212;   // R12
  Stacks[threadNum][stackSize-5] = 0x03030303;   // R3
  Stacks[threadNum][stackSize-6] = 0x02020202;   // R2
  Stacks[threadNum][stackSize-7] = 0x01010101;   // R1
  Stacks[threadNum][stackSize-8] = 0x00000000;   // R0
  Stacks[threadNum][stackSize-9] = 0x11111111;   // R11
  Stacks[threadNum][stackSize-10] = 0x10101010;  // R10
  Stacks[threadNum][stackSize-11] = 0x09090909;  // R9
  Stacks[threadNum][stackSize-12] = 0x08080808;  // R8
  Stacks[threadNum][stackSize-13] = 0x07070707;  // R7
  Stacks[threadNum][stackSize-14] = 0x06060606;  // R6
  Stacks[threadNum][stackSize-15] = 0x05050505;  // R5
  Stacks[threadNum][stackSize-16] = 0x04040404;  // R4	
// Sort Linked List based off of priorites
	if(threadCount > 2) {
		tcb *lastThread = &tcbs[threadNum];
		tcb *nextThread = firstThread->next;
		while(nextThread != firstThread) {													// Iterate through all threads to apply sorting algorithm
			if(lastThread->priority <= firstThread->priority) {				// If last thread is higher priority, then break while loop, technically the threads are in order
				break;
			}
			if(lastThread->priority < nextThread->priority){
				lastThread->prev->next = firstThread;
				firstThread->prev = lastThread->prev;
				lastThread->next = nextThread;
				nextThread->prev->next = lastThread;
				lastThread->prev = nextThread->prev;		
				nextThread->prev = lastThread;
			}
			nextThread = nextThread->next;
		}
		firstThread = &tcbs[0];
		for(int threadIndex = 0; threadIndex <= threadNum; threadIndex++) {
			if(firstThread->priority > tcbs[threadIndex].priority){
				firstThread = &tcbs[threadIndex];
			}
		}
	}
	tcbs[threadNum].empty = 1;
	EndCritical(status);
	return threadMaxed;
}

/*********** OS INIT/LAUNCH ***********/

void ConsoleInit(void){
	UART_OutString("File System\n\n");
}

void OS_Init(void){
  DisableInterrupts();
  PLL_Init();                 // set processor clock to 80 MHz
	Debug_Port_Init();					// intialize PE0-3 and PC4-7 for logic analyzer debugging
	tcbs_Init();								// set all threads 'empty' field to zero
	Timer2_Init();							// interrupt triggered, user thread
	Timer3_Init();							// interrupt triggered, user thread
	UART_Init();								// interrupt driven UART from Valvano
	ConsoleInit();							// Console information message
	OS_Fifo_Init(FIFO_SIZE); 		// Used for passing data between ADC and display
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R =(NVIC_SYS_PRI3_R&0x00FFFFFF)|0x60000000; // priority 6 - SysTick
	NVIC_SYS_PRI3_R =(NVIC_SYS_PRI3_R&0xFF00FFFF)|0x00E00000; // priority 7 - PendSV
}
	
void OS_Launch(unsigned long theTimeSlice){
	NVIC_ST_RELOAD_R = theTimeSlice - 1;	// set reload value for systick, user defined
	NVIC_ST_CTRL_R = 0x07; 			// enable, core clock and interrupt arm
	RunPt = firstThread;        // thread 0 will run first
	StartOS();	
}

/*********** Initialize TCBs *********/
void tcbs_Init(void){
	int threadIndex;
	for(threadIndex = 0; threadIndex < NUMTHREADS; threadIndex++) {
		tcbs[threadIndex].empty = 0;
	}
}

/*********** SYSTICK ***********/

void SysTick_Handler(){ tcb* LastThread;
	DisableInterrupts();
	DIO0 ^= BIT0;
	NextThread = firstThread->next;
	LastThread = firstThread;
	while(NextThread != LastThread) {
		if(NextThread->sleep > 0){
			NextThread->sleep--;
		}
		NextThread = NextThread->next;
	}
	NextThread = firstThread->next;
	while(NextThread != LastThread){
		if(NextThread->sleep == 0 && NextThread->priority <= firstThread->priority){
			firstThread = NextThread;
			break;
		}
		NextThread = NextThread->next;
	}
	NextThread = firstThread;
	NVIC_INT_CTRL_R = NVIC_INT_CTRL_PEND_SV;
	EnableInterrupts();

}

/*********** PERIODIC TASKS ***********/
	
int OS_AddPeriodicThread(void(*task)(void), unsigned long period, unsigned long priority, int timerNumber){
	if(timerNumber == 1){
		Timer2A_Launch(task, period, priority);
	}
	else if(timerNumber == 2){
		Timer3A_Launch(task, period, priority);	
	}
	return SUCCESS;
}

/*********** SLEEP/KILL ***********/

void OS_Sleep(unsigned long sleepTime){
	DisableInterrupts();
	RunPt->sleep = sleepTime;
	EnableInterrupts();
	NVIC_INT_CTRL_R = NVIC_INT_CTRL_PENDSTSET;
}
void OS_Kill(void){
	DisableInterrupts();
	RunPt->prev->next = RunPt->next;
	RunPt->next->prev = RunPt->prev;
	RunPt->empty = 0;
	if(firstThread == RunPt){
		firstThread = &tcbs[0];
		for(int threadIndex = 0; threadIndex <= threadNum; threadIndex++) {
			if(firstThread->priority > tcbs[threadIndex].priority){
				firstThread = &tcbs[threadIndex];
			}
		}
	}
	EnableInterrupts();
	NVIC_INT_CTRL_R = NVIC_INT_CTRL_PENDSTSET;
}	

/*********** TIME ***********/
	
int OS_Time(void){ 
	unsigned long time = NVIC_ST_CURRENT_R;
	return time;
}

int OS_TimeDifference(unsigned long start, unsigned long stop){ int diff;
	if (start > stop){
		diff = start - stop;
	}
	else {diff = (TIME_2MS - stop) + start;}	
	return diff;
}
	
void OS_ClearMsTime(void){
	TIMER3_TAR_R = 0;
}

int OS_MsTime(void){
	DisableInterrupts();
	int elapsedTime;
	EnableInterrupts();
	return 21;
}

/*********** FIFO ***********/

unsigned long volatile *PutPt;
unsigned long volatile *GetPt;
unsigned long Fifo[FIFO_SIZE];

void OS_Fifo_Init(unsigned long size){
	PutPt = GetPt = &Fifo[0];
}

unsigned long OS_Fifo_Get(void){
	unsigned long volatile *nextGetPt;
	unsigned long volatile *storeGetPt;
	storeGetPt = GetPt;
	nextGetPt = GetPt + 1;
	if(PutPt == GetPt){
		return FAIL;
	}
	if(nextGetPt == &Fifo[FIFO_SIZE]){
		GetPt = &Fifo[0];		
	}
	GetPt = nextGetPt;
	return *storeGetPt;
}

int OS_Fifo_Put(unsigned long data){
	unsigned long volatile *nextPutPt;
	nextPutPt = PutPt + 1;
	if(nextPutPt == GetPt){
		return FAIL;
	}
	if(nextPutPt == &Fifo[FIFO_SIZE]){
		nextPutPt = &Fifo[0];
  }
	else{
		*(PutPt) = data;
		PutPt = nextPutPt;
	}
	return SUCCESS;
}

/*********** SEMAPHORES ***********/

void OS_InitSemaphore(Sema4Type *semaPt, long value){
	semaPt->Value = value;
}
void OS_Wait(Sema4Type *semaPt){
	DisableInterrupts();
	while(semaPt->Value <= 0){
		EnableInterrupts();
		DisableInterrupts();		
	}
	semaPt->Value = semaPt->Value - 1;
	EnableInterrupts();
}

void OS_Signal(Sema4Type *semaPt){
	long status;
	status = StartCritical();
	semaPt->Value = semaPt->Value + 1;
	EndCritical(status);
}

void OS_bWait(Sema4Type *semaPt){
	DisableInterrupts();
	while(semaPt->Value == 0){
		EnableInterrupts();
		DisableInterrupts();
	}
	semaPt->Value = semaPt->Value - 1;
	EnableInterrupts();
}

void OS_bSignal(Sema4Type *semaPt){
	long status;
	status = StartCritical();
	semaPt->Value = 1;
	EndCritical(status);
}

/*********** MAILBOX ***********/
unsigned long mail;
Sema4Type MailboxFree;
Sema4Type MailboxFull;

void OS_MailBox_Init(void){
	//OS_InitSemaphore(&MailboxFree,1);
	//OS_InitSemaphore(&MailboxFull,0);
	mail = 0;
}

void OS_MailBox_Send(unsigned long data){
	//OS_bWait(&MailboxFree);
	mail = data;
	//OS_bSignal(&MailboxFull);
}
unsigned long OS_MailBox_Recv(void){
	//OS_bWait(&MailboxFull);
	unsigned long data = mail;
	//OS_bWait(&MailboxFree);
}
	
	
/*---------- Future OS Functions -----------*/

unsigned long OS_Id(void){return 0;}
long OS_Fifo_Size(void){return 0;}
