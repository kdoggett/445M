// Lab2.c
// Runs on LM4F120/TM4C123
// Real Time Operating System for Labs 2 and 3
// Lab2 Part 1: Testmain1 and Testmain2
// Lab2 Part 2: Testmain3 Testmain4  and main
// Lab3: Testmain5 Testmain6, Testmain7, and main (with SW2)

// Jonathan W. Valvano 1/31/14, valvano@mail.utexas.edu
// EE445M/EE380L.6 
// You may use, edit, run or distribute this file 
// You are free to change the syntax/organization of this file

// LED outputs to logic analyzer for OS profile 
// PF1 is preemptive thread switch
// PF2 is periodic task, samples PD3
// PF3 is SW1 task (touch PF4 button)

// Button inputs
// PF0 is SW2 task (Lab3)
// PF4 is SW1 button input

// Analog inputs
// PD3 Ain3 sampled at 2k, sequencer 3, by DAS software start in ISR
// PD2 Ain5 sampled at 250Hz, sequencer 0, by Producer, timer tigger

#include "OS.h"
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "ADC.h"
#include "UART.h"
#include "PLL.h"
#include <string.h> 
#include "pins.h"

#define MS_5_DELAY		40000
#define	MS_10_DELAY		80000
#define	MS_100_DELAY	800000

int main(void){
	mainMain();
	//Testmain5();
	//Testmain6();
	//Testmain7();
	return 0;
}


//*********Prototype for FFT in cr4_fft_64_stm32.s, STMicroelectronics
void cr4_fft_64_stm32(void *pssOUT, void *pssIN, unsigned short Nbin);
//*********Prototype for PID in PID_stm32.s, STMicroelectronics
short PID_stm32(short Error, short *Coeff);

unsigned long NumCreated;   // number of foreground threads created
unsigned long PIDWork;      // current number of PID calculations finished
unsigned long FilterWork;   // number of digital filter calculations finished
unsigned long NumSamples;   // incremented every ADC sample, in Producer
#define FS 400            // producer/consumer sampling
#define RUNLENGTH (20*FS) // display results and quit when NumSamples==RUNLENGTH
// 20-sec finite time experiment duration 

#define PERIOD TIME_500US // DAS 2kHz sampling period in system time units
long x[64],y[64];         // input and output arrays for FFT

//---------------------User debugging-----------------------
unsigned long DataLost;     // data sent by Producer, but not received by Consumer
long MaxJitter;             // largest time jitter between interrupts in usec
#define JITTERSIZE 64
unsigned long const JitterSize=JITTERSIZE;
unsigned long JitterHistogram[JITTERSIZE]={0,};

//------------------Task 1--------------------------------
// 2 kHz sampling ADC channel 1, using software start trigger
// background thread executed at 2 kHz
// 60-Hz notch high-Q, IIR filter, assuming fs=2000 Hz
// y(n) = (256x(n) -503x(n-1) + 256x(n-2) + 498y(n-1)-251y(n-2))/256 (2k sampling)
// y(n) = (256x(n) -476x(n-1) + 256x(n-2) + 471y(n-1)-251y(n-2))/256 (1k sampling)
long Filter(long data){
static long x[6]; // this MACQ needs twice
static long y[6];
static unsigned long n=3;   // 3, 4, or 5
  n++;
  if(n==6) n=3;     
  x[n] = x[n-3] = data;  // two copies of new data
  y[n] = (256*(x[n]+x[n-2])-503*x[n-1]+498*y[n-1]-251*y[n-2]+128)/256;
  y[n-3] = y[n];         // two copies of filter outputs too
  return y[n];
} 
//******** DAS *************** 
// background thread, calculates 60Hz notch filter
// runs 2000 times/sec
// samples channel 4, PD3,
// inputs:  none
// outputs: none
unsigned long DASoutput;
void DAS(void){ 
unsigned long input;  
unsigned static long lastTime;  // time at previous ADC sample
unsigned long thisTime;         // time at current ADC sample
long jitter;                    // time between measured and expected, in us
  if(NumSamples < RUNLENGTH){   // finite time run
    input = ADC_In();           // channel set when calling ADC_Init
    thisTime = OS_Time();       // current time, 12.5 ns
		DIO1 ^= BIT1;
    DASoutput = Filter(input);
    FilterWork++;        // calculation finished
    if(FilterWork>1){    // ignore timing of first interrupt
      unsigned long diff = OS_TimeDifference(lastTime,thisTime);
      if(diff>PERIOD){
        jitter = (diff-PERIOD+4)/8;  // in 0.1 usec
      }else{
        jitter = (PERIOD-diff+4)/8;  // in 0.1 usec
      }
      if(jitter > MaxJitter){
        MaxJitter = jitter; // in usec
      }       // jitter should be 0
      if(jitter >= JitterSize){
        jitter = JITTERSIZE-1;
      }
      JitterHistogram[jitter]++; 
    }
    lastTime = thisTime;
  }
}
//--------------end of Task 1-----------------------------

//------------------Task 2--------------------------------
// background thread executes with SW1 button
// one foreground task created with button push
// foreground treads run for 2 sec and die
// ***********ButtonWork*************
void ButtonWork(void){
//unsigned long myId = OS_Id(); 
  ST7735_Message(1,0,"NumCreated =",NumCreated); 
  OS_Sleep(50);     // set this to sleep for 50msec
  ST7735_Message(1,1,"PIDWork     =",PIDWork);
  ST7735_Message(1,2,"DataLost    =",DataLost);
  ST7735_Message(1,3,"Jitter 0.1us=",MaxJitter);
  OS_Kill();  // done, OS does not return from a Kill
} 

//************SW1Push*************
// Called when SW1 Button pushed
// Adds another foreground task
// background threads execute once and return
void SW1Push(void){
	//if(OS_MsTime() > 20){ // debounce
		OS_AddThread(&ButtonWork,128,0);
		NumCreated++; 
	//}
	//OS_ClearMsTime();  // at least 20ms between touches
}

//************SW2Push*************
// Called when SW2 Button pushed, Lab 3 only
// Adds another foreground task
// background threads execute once and return
void SW2Push(void){
  //if(OS_MsTime() > 20){ // debounce
  //  if(OS_AddThread(&ButtonWork,100,4)){
		OS_AddThread(&ButtonWork,128,4);
		NumCreated++;  
  //  }
  //  OS_ClearMsTime();  // at least 20ms between touches
 // }
}
//--------------end of Task 2-----------------------------

//------------------Task 3--------------------------------
// hardware timer-triggered ADC sampling at 400Hz
// Producer runs as part of ADC ISR
// Producer uses fifo to transmit 400 samples/sec to Consumer
// every 64 samples, Consumer calculates FFT
// every 2.5ms*64 = 160 ms (6.25 Hz), consumer sends data to Display via mailbox
// Display thread updates LCD with measurement

//******** Producer *************** 
// The Producer in this lab will be called from your ADC ISR
// A timer runs at 400Hz, started by your ADC_Collect
// The timer triggers the ADC, creating the 400Hz sampling
// Your ADC ISR runs when ADC data is ready
// Your ADC ISR calls this function with a 12-bit sample 
// sends data to the consumer, runs periodically at 400Hz
// inputs:  none
// outputs: none
void Producer(unsigned long data){  
	DIO3 ^= BIT3;
  if(NumSamples < RUNLENGTH){   // finite time run
    NumSamples++;               // number of samples
    if(OS_Fifo_Put(data) == 0){ // send to consumer
      DataLost++;
    } 
  } 
}
void Display(void); 

//******** Consumer *************** 
// foreground thread, accepts data from producer
// calculates FFT, sends DC component to Display
// inputs:  none
// outputs: none
void Consumer(void){ 
unsigned long data,DCcomponent;   // 12-bit raw ADC sample, 0 to 4095
unsigned long t;                  // time in 2.5 ms
//unsigned long myId = OS_Id(); 
  ADC_Collect(5, FS, &Producer); // start ADC sampling, channel 5, PD2, 400 Hz
  NumCreated += OS_AddThread(&Display,128,0);
	DIO2 ^= BIT2;
  while(NumSamples < RUNLENGTH) {
    for(t = 0; t < 64; t++){   // collect 64 ADC samples
      data = OS_Fifo_Get();    // get from producer
      x[t] = data;             // real part is 0 to 4095, imaginary part is 0
    }
    cr4_fft_64_stm32(y,x,64);  // complex FFT of last 64 ADC values
    DCcomponent = y[0]&0xFFFF; // Real part at frequency 0, imaginary part should be zero
    OS_MailBox_Send(DCcomponent); // called every 2.5ms*64 = 160ms
  }
  OS_Kill();  // done
}
//******** Display *************** 
// foreground thread, accepts data from consumer
// displays calculated results on the LCD
// inputs:  none                            
// outputs: none
void Display(void){ 
unsigned long data,voltage;
  ST7735_Message(0,1,"Run length = ",(RUNLENGTH)/FS);   // top half used for Display
  while(NumSamples < RUNLENGTH) { 
    data = OS_MailBox_Recv();
    voltage = 3000*data/4095;               // calibrate your device so voltage is in mV
    ST7735_Message(0,2,"v(mV) =",voltage);
		OS_Sleep(50);
  }
  OS_Kill();  // done
} 

//--------------end of Task 3-----------------------------

//------------------Task 4--------------------------------
// foreground thread that runs without waiting or sleeping
// it executes a digital controller 
//******** PID *************** 
// foreground thread, runs a PID controller
// never blocks, never sleeps, never dies
// inputs:  none
// outputs: none
short IntTerm;     // accumulated error, RPM-sec
short PrevError;   // previous error, RPM
short Coeff[3];    // PID coefficients
short Actuator;
void PID(void){ 
	DIO4 ^= BIT4;
short err;  // speed error, range -100 to 100 RPM
//unsigned long myId = OS_Id(); 
  PIDWork = 0;
  IntTerm = 0;
  PrevError = 0;
  Coeff[0] = 384;   // 1.5 = 384/256 proportional coefficient
  Coeff[1] = 128;   // 0.5 = 128/256 integral coefficient
  Coeff[2] = 64;    // 0.25 = 64/256 derivative coefficient*
  while(NumSamples < RUNLENGTH) { 
    for(err = -1000; err <= 1000; err++){    // made-up data
      Actuator = PID_stm32(err,Coeff)/256;
    }
    PIDWork++;        // calculation finished
  }
  for(;;){ }          // done
}
//--------------end of Task 4-----------------------------

//------------------Task 5--------------------------------
// UART background ISR performs serial input/output
// Two software fifos are used to pass I/O data to foreground
// The interpreter runs as a foreground thread
// The UART driver should call OS_Wait(&RxDataAvailable) when foreground tries to receive
// The UART ISR should call OS_Signal(&RxDataAvailable) when it receives data from Rx
// Similarly, the transmit channel waits on a semaphore in the foreground
// and the UART ISR signals this semaphore (TxRoomLeft) when getting data from fifo
// Modify your intepreter from Lab 1, adding commands to help debug 
// Interpreter is a foreground thread, accepts input from serial port, outputs to serial port
// inputs:  none
// outputs: none
//void Interpreter(void);    // just a prototype, link to your interpreter ------------
// add the following commands, leave other commands, if they make sense
// 1) print performance measures 
//    time-jitter, number of data points lost, number of calculations performed
//    i.e., NumSamples, NumCreated, MaxJitter, DataLost, FilterWork, PIDwork
      
// 2) print debugging parameters 
//    i.e., x[], y[] 
//--------------end of Task 5-----------------------------

void UserWork(void){
	UART_OutString("User Work\n");	
}

#define COMMAND_MAX	100

char command[COMMAND_MAX];

void ProcessCommand(char *command){
	char commandType[COMMAND_MAX];
	unsigned long ADCValue;
// Initialize commandType buffer
	for(int j = 0; j < COMMAND_MAX; j++) {
		commandType[j] = 0;
	}
	uint32_t i = 0;
	char commandNum;
	while(1) {
		if(command[i] == ' ')
		{
			break;
		}
		else if(command[i] == NULL) {
			break;
		}
		else {
			commandType[i] = command[i];
		}
		i++;
	}
	if (strcmp(commandType,"ADC") == 0){
		commandType[i] = ' ';
		i++;
		commandNum = 1;
	}
	if (strcmp(commandType,"Timer") == 0){
		commandType[i] = ' ';
		i++;
		commandNum = 2;
	}
	if (strcmp(commandType,"LCD") == 0){
		commandType[i] = ' ';
		i++;
		commandNum = 3;
	}
	if (strcmp(commandType,"UserTask") == 0){
		commandType[i] = ' ';
		i++;
		commandNum = 4;		
	}
		switch(commandNum){
			case 1:
				ADCValue = ADC_In();
				UART_OutUDec(ADCValue);
				break;
			case 2:
				while(command[i] != 0){
					commandType[i] = command[i];
					i++;
				}
				UART_OutString(commandType);
				break;
			case 3:
				while(command[i] != 0){
					commandType[i] = command[i];
					i++;
				}
				UART_OutString(commandType);
				break;
			case 4:
				while(command[i] != NULL){
					commandType[i] = command[i];
					i++;
				}
				OS_AddSW1Task(&UserWork,0);
				UART_OutString("\nTask Created\n");
				break;				
			default:
				UART_OutString(commandType);
				break;
		}
		i = 0;
}

void Interpreter(void){        
  for(;;){
		DIO5 ^= BIT5;       // heartbeat
		while(RxFifo_Size() == 0 && UART_InChar() != 0x0D){};
			UART_InString(command,COMMAND_MAX);
			ProcessCommand(command);
		}
 }

//*******************final user main DEMONTRATE THIS TO TA**********
int mainMain(void){ 
  OS_Init();           // initialize, disable interrupts
  DataLost = 0;        // lost data between producer and consumer
  NumSamples = 0;
  MaxJitter = 0;       // in 1us units
	ST7735_DrawFastHLine(0, 80, 128, ST7735_YELLOW);

//********initialize communication channels
  OS_MailBox_Init();
  OS_Fifo_Init(32);    // ***note*** 4 is not big enough*****

//*******attach background tasks***********
  OS_AddSW1Task(&SW1Push,3);
  OS_AddSW2Task(&SW2Push,3);
  ADC_Open_SoftwareTrigger(4);  // sequencer 3, channel 4, PD3, sampling in DAS()
  OS_AddPeriodicThread(&DAS,PERIOD,3,1); // 2 kHz real time sampling of PD3		Timer2A

  NumCreated = 0 ;
// create initial foreground threads
	NumCreated += OS_AddThread(&Interpreter,128,2);
  NumCreated += OS_AddThread(&Consumer,128,2); 
	NumCreated += OS_AddThread(&PID,128,3);  // Lab 3, make this lowest priority
  OS_Launch(TIME_2MS); // doesn't return, interrupts enabled in here
  return 0;            // this never executes
}

/* JITTER */
unsigned long periodicTaskA;
unsigned long periodicTaskB;

int i = 0;
int j = 0;
unsigned long JitterRecordA[100];
unsigned long JitterRecordB[100];
int jitterSumA;
int jitterSumB;
int jitterAverageA;
int jitterAverageB;
void jitter(void){
	i = 0;
	j = 0;
	UART_OutString("Jitter: ");
	for (i = 0; i < 100; i++){
		jitterSumA += JitterRecordA[i]; 
	}
	jitterAverageA = jitterSumA/100;
	for (j = 0; j < 100; j++){
		jitterSumB += JitterRecordB[j];
	}
	jitterAverageB = jitterSumB/100;
	UART_OutString("Jitter TaskA Total: ");
	UART_OutUDec(jitterSumA);
	UART_OutString("\nJitter TaskB Total: ");
	UART_OutUDec(jitterSumB);
	UART_OutString("\nJitter TaskA Average: ");
	UART_OutUDec(jitterAverageA);
	UART_OutString("\nJitter TaskB Average: ");
	UART_OutUDec(jitterAverageB);
	UART_OutString("\nMax Jitter:");
	UART_OutUDec(MaxJitter);
	UART_OutChar('\n');
}

//******************* Lab 3 Preparation 2**********
// Modify this so it runs with your RTOS (i.e., fix the time units to match your OS)
// run this with 
// UART0, 115200 baud rate, used to output results 
// SYSTICK interrupts, period established by OS_Launch
// first timer interrupts, period established by first call to OS_AddPeriodicThread
// second timer interrupts, period established by second call to OS_AddPeriodicThread
// SW1 no interrupts
// SW2 no interrupts
unsigned long CountA;   // number of times Task A called
unsigned long CountB;   // number of times Task B called
unsigned long Count1;   // number of times thread1 loops

//*******PseudoWork*************
// simple time delay, simulates user program doing real work
// Input: amount of work in 100ns units (free free to change units
// Output: none
void PseudoWork(unsigned short work){
	DIO1 = 0;
	DIO2 = 0;
	unsigned long startTime;
  startTime = OS_Time();    // time in 100ns units;
	JitterRecordA[i] = OS_TimeDifference(periodicTaskA,startTime)/80000;
	JitterRecordB[i] = OS_TimeDifference(periodicTaskB,startTime)/80000;
	i++;
	j++;
	if(i == 100){
		i = 0;
	}
	if(j == 100){
		j = 0;
	}
	//while(OS_TimeDifference(startTime,OS_Time()) <= work){} 
}
void Thread6(void){  // foreground thread
  Count1 = 0;          
  for(;;){
		DIO3 = BIT3;
    Count1++; 
		DIO3 = 0;
  }
}

void Thread7(void){  // foreground thread
  UART_OutString("\n\rEE345M/EE380L, Lab 3 Preparation 2\n\r");
  OS_Sleep(2500);   // 10 seconds  
  jitter();         // print jitter information
  UART_OutString("\n\r\n\r");
  OS_Kill();
}
#define workA 500       // {5,50,500 us} work in Task A
#define counts1us 10    // number of OS_Time counts per 1us
void TaskA(void){       // called every {1000, 2990us} in background
  DIO1 = BIT1;      // debugging profile  
	periodicTaskA = OS_Time();
  CountA++;
  PseudoWork(workA*counts1us); //  do work (100ns time resolution)
}
#define workB 250       // 250 us work in Task B
void TaskB(void){       // called every pB in background
  DIO2 = BIT2;      // debugging profile  
	periodicTaskB = OS_Time();
  CountB++;
  PseudoWork(workB*counts1us); //  do work (100ns time resolution) 
}

int Testmain5(void){       // Testmain5 Lab 3
  OS_Init();           // initialize, disable interrupts
  NumCreated = 0 ;
  NumCreated += OS_AddThread(&Thread6,128,2); 
  NumCreated += OS_AddThread(&Thread7,128,1); 
  OS_AddPeriodicThread(&TaskA,TIME_1MS,0,1);           // 1 ms, higher priority
  OS_AddPeriodicThread(&TaskB,2*TIME_1MS,1,2);         // 2 ms, lower priority
 
  OS_Launch(TIME_2MS); // 2ms, doesn't return, interrupts enabled in here
  return 0;             // this never executes
}


//******************* Lab 3 Preparation 4**********
// Modify this so it runs with your RTOS used to test blocking semaphores
// run this with 
// UART0, 115200 baud rate,  used to output results 
// SYSTICK interrupts, period established by OS_Launch
// first timer interrupts, period established by first call to OS_AddPeriodicThread
// second timer interrupts, period established by second call to OS_AddPeriodicThread
// SW1 no interrupts, 
// SW2 no interrupts
Sema4Type s;            // test of this counting semaphore
unsigned long SignalCount1;   // number of times s is signaled
unsigned long SignalCount2;   // number of times s is signaled
unsigned long SignalCount3;   // number of times s is signaled
unsigned long WaitCount1;     // number of times s is successfully waited on
unsigned long WaitCount2;     // number of times s is successfully waited on
unsigned long WaitCount3;     // number of times s is successfully waited on
#define MAXCOUNT 20000
void OutputThread(void){  // foreground thread
  UART_OutString("\n\rEE345M/EE380L, Lab 3 Preparation 4\n\r");
  while(SignalCount1+SignalCount2+SignalCount3<100*MAXCOUNT){
    OS_Sleep(1000);   // 1 second
    UART_OutString(".");
  }       
  UART_OutString(" done\n\r");
  UART_OutString("Signalled="); UART_OutUDec(SignalCount1+SignalCount2+SignalCount3);
  UART_OutString(", Waited="); UART_OutUDec(WaitCount1+WaitCount2+WaitCount3);
  UART_OutString("\n\r");
  OS_Kill();
}
void Wait1(void){  // foreground thread
  for(;;){
		DIO1 ^= BIT1;
    OS_Wait(&s);    // three threads waiting
    WaitCount1++; 
  }
}
void Wait2(void){  // foreground thread
  for(;;){
		DIO2 ^= BIT2;
    OS_Wait(&s);    // three threads waiting
    WaitCount2++; 
  }
}
void Wait3(void){   // foreground thread
  for(;;){
		DIO4 ^= BIT4;
    OS_Wait(&s);    // three threads waiting
    WaitCount3++; 
  }
}
void Signal1(void){      // called every 799us in background
  if(SignalCount1<MAXCOUNT){
		DIO5 ^= BIT5;
    OS_Signal(&s);
    SignalCount1++;
  }
}
// edit this so it changes the periodic rate
void Signal2(void){       // called every 1111us in background
  if(SignalCount2<MAXCOUNT){
		DIO6 ^= BIT6;
    OS_Signal(&s);
    SignalCount2++;
  }
}
void Signal3(void){       // foreground
  while(SignalCount3<98*MAXCOUNT){
		DIO7 ^= BIT7;
    OS_Signal(&s);
    SignalCount3++;
  }
  OS_Kill();
}

long add(const long n, const long m){
static long result;
  result = m+n;
  return result;
}
int Testmain6(void){      // Testmain6  Lab 3
  volatile unsigned long delay;
  OS_Init();           // initialize, disable interrupts
  delay = add(3,4);
  SignalCount1 = 0;   // number of times s is signaled
  SignalCount2 = 0;   // number of times s is signaled
  SignalCount3 = 0;   // number of times s is signaled
  WaitCount1 = 0;     // number of times s is successfully waited on
  WaitCount2 = 0;     // number of times s is successfully waited on
  WaitCount3 = 0;	  // number of times s is successfully waited on
  OS_InitSemaphore(&s,0);	 // this is the test semaphore
  OS_AddPeriodicThread(&Signal1,(799*TIME_1MS)/1000,0,1);   // 0.799 ms, higher priority
  OS_AddPeriodicThread(&Signal2,(1111*TIME_1MS)/1000,1,2);  // 1.111 ms, lower priority
  NumCreated = 0 ;
  NumCreated += OS_AddThread(&Thread6,128,6);    	// idle thread to keep from crashing
  NumCreated += OS_AddThread(&OutputThread,128,2); 	// results output thread
  NumCreated += OS_AddThread(&Signal3,128,2); 	// signalling thread
  NumCreated += OS_AddThread(&Wait1,128,2); 	// waiting thread
  NumCreated += OS_AddThread(&Wait2,128,2); 	// waiting thread
  NumCreated += OS_AddThread(&Wait3,128,2); 	// waiting thread
 
  OS_Launch(TIME_1MS);  // 1ms, doesn't return, interrupts enabled in here
  return 0;             // this never executes
}


//******************* Lab 3 Measurement of context switch time**********
// Run this to measure the time it takes to perform a task switch
// UART0 not needed 
// SYSTICK interrupts, period established by OS_Launch
// first timer not needed
// second timer not needed
// SW1 not needed, 
// SW2 not needed
// logic analyzer on PF1 for systick interrupt (in your OS)
//                on PE0_DIO0 to measure context switch time
void Thread8(void){       // only thread running
  while(1){
    //PE0_DIO0 ^= 0x01;      // debugging profile  
  }
}
int Testmain7(void){       // Testmain7
  OS_Init();           // initialize, disable interrupts
  NumCreated = 0 ;
  NumCreated += OS_AddThread(&Thread8,128,2); 
  OS_Launch(TIME_1MS/10); // 100us, doesn't return, interrupts enabled in here
  return 0;             // this never executes
}
