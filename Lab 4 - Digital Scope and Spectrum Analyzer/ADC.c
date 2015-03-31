#include "tm4c123gh6pm.h"
#include "OS.h"
#include "pins.h"
#include "ST7735.h"
#include "FIFO.h"
#include "stdint.h"
#include "ADC.h"
#include "UART.h"
#include "Filter.h"
#include "MACQ.h"

volatile unsigned long ADC_Value;
//******* Periodic Thread ****
void ADC_Sample_Software(void){
		ADC_Value = ADC_In();	// Puts ADC sample value in FIFO
		ST7735_Message(0,2,"Software Trigger: ",ADC_Value);
}

unsigned long ADC_In(void){ 
	unsigned long result;
  ADC0_PSSI_R = 0x0004;            // 1) initiate SS2
  while((ADC0_RIS_R&0x04)==0){};   // 2) wait for conversion done
  result = ADC0_SSFIFO2_R&0xFFF;   // 3) read result
  ADC0_ISC_R = 0x0004;             // 4) acknowledge completion
	return result;
}

void ADC_SoftwareTrigger(void){ volatile int delay;
	long status;
	status = StartCritical();
  SYSCTL_RCGCGPIO_R |= 0x08;  
  delay = SYSCTL_RCGCGPIO_R;      // extra time for clock to stabilize
  delay = SYSCTL_RCGCGPIO_R;                      //      Ain5 is on PD2
	GPIO_PORTD_DIR_R &= ~0x04;  // 3.5) make PD2 input
	GPIO_PORTD_AFSEL_R |= 0x04; // 4.5) enable alternate function on PD2
	GPIO_PORTD_DEN_R &= ~0x04;  // 5.5) disable digital I/O on PD2
	GPIO_PORTD_AMSEL_R |= 0x04; // 6.5) enable analog functionality on PD2
  SYSCTL_RCGCADC_R |= 0x0001;   // 7) activate ADC0 
//  while((SYSCTL_PRADC_R&0x0001) != 0x0001){};   // good code, but not yet implemented in simulator
  delay = SYSCTL_RCGCADC_R;         // extra time for clock to stabilize
  delay = SYSCTL_RCGCADC_R;         // extra time for clock to stabilize
  delay = SYSCTL_RCGCADC_R;         // extra time for clock to stabilize
  delay = SYSCTL_RCGCADC_R;
  ADC0_PC_R &= ~0xF;              // 8) clear max sample rate field
  ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
  ADC0_SSPRI_R = 0x3210;          // 9) Sequencer 3 is lowest priority
  ADC0_ACTSS_R &= ~0x0004;        // 10) disable sample sequencer 2
  ADC0_EMUX_R &= ~0x0F00;         // 11) seq2 is software trigger
  ADC0_SSMUX2_R = 0x0089;         // 12) set channels for SS2
  ADC0_SSCTL2_R = 0x0060;         // 13) no TS0 D0 IE0 END0 TS1 D1, yes IE1 END1
  ADC0_IM_R &= ~0x0004;           // 14) disable SS2 interrupts
  ADC0_ACTSS_R |= 0x0004;         // 15) enable sample sequencer 2
	OS_AddPeriodicThread(&ADC_Sample_Software,TIME_2MS*5,5,1);
	EndCritical(status);
}

//------------ADC0_InSeq3------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion

void ADC_HardwareTrigger_T0A(int period){
  volatile int delay;
  SYSCTL_RCGCADC_R |= 0x01;     // 1) activate ADC0 
  SYSCTL_RCGCGPIO_R |= 0x08;    // Port D clock
  delay = SYSCTL_RCGCGPIO_R;    // allow time for clock to stabilize
  GPIO_PORTD_DIR_R &= ~0x08;    // make PD3 input
  GPIO_PORTD_AFSEL_R |= 0x08;   // enable alternate function on PD3
  GPIO_PORTD_DEN_R &= ~0x08;    // disable digital I/O on PD3
  GPIO_PORTD_AMSEL_R |= 0x08;   // enable analog functionality on PD3
  ADC0_PC_R = 0x01;             // 2) configure for 125K samples/sec
  ADC0_SSPRI_R = 0x3210;        // 3) sequencer 0 is highest, sequencer 3 is lowest
  SYSCTL_RCGCTIMER_R |= 0x01;   // 4) activate timer0 
  delay = SYSCTL_RCGCGPIO_R;
  TIMER0_CTL_R = 0x00000000;    // disable timer0A during setup
  TIMER0_CTL_R |= 0x00000020;   // enable timer0A trigger to ADC
  TIMER0_CFG_R = 0;             // configure for 32-bit timer mode
  TIMER0_TAMR_R = 0x00000002;   // configure for periodic mode, default down-count settings
  TIMER0_TAPR_R = 0;            // prescale value for trigger
  TIMER0_TAILR_R = period-1;    // start value for trigger
  TIMER0_IMR_R = 0x00000000;    // disable all interrupts
  TIMER0_CTL_R |= 0x00000001;   // enable timer0A 32-b, periodic, no interrupts
  ADC0_ACTSS_R &= ~0x08;        // 5) disable sample sequencer 3
  ADC0_EMUX_R = (ADC0_EMUX_R&0xFFFF0FFF)+0x5000; // 6) timer trigger event
  ADC0_SSMUX3_R = 4;            // 7) PD4 is channel 4
  ADC0_SSCTL3_R = 0x06;         // 8) set flag and end                       
  ADC0_IM_R |= 0x08;            // 9) enable SS3 interrupts
  ADC0_ACTSS_R |= 0x08;         // 10) enable sample sequencer 3
  NVIC_PRI4_R = (NVIC_PRI4_R&0xFFFF00FF)|0x00004000; // 11)priority 2
  NVIC_EN0_R = 1<<17;           // 12) enable interrupt 17 in NVIC
}

void ADC0Seq3_Handler(void){
	int16_t filterOutput;
	DIO2 ^= BIT2;
  ADC0_ISC_R = 0x08;          // acknowledge ADC sequence 3 completion
	filterOutput = ADC0_SSFIFO3_R;
	filterOutput = Filter_Calc(filterOutput);
	MACQ_Put(filterOutput);
}