#include "tm4c123gh6pm.h"
#include "edisk.h"

// Use PD7 for chip select bit for read/write to LCD versus SDC
#define SDC_CS   (*((volatile unsigned long *)0x40007200))
void CS_Init(void){ unsigned long volatile delay;
  SYSCTL_RCGCGPIO_R |= 0x08; // activate port D
  delay = SYSCTL_RCGCGPIO_R;
  delay = SYSCTL_RCGCGPIO_R;
  GPIO_PORTD_LOCK_R = 0x4C4F434B;   // 2) unlock PortD PD7  
  GPIO_PORTD_CR_R |= 0xFF;          // allow changes to PD7-0       
  GPIO_PORTD_PUR_R |= 0x80;         // enable weak pullup on PD7
  GPIO_PORTD_DIR_R |= 0x80;         // make PD7 output 
  GPIO_PORTD_DR4R_R |= 0x80;        // 4mA output on outputs
  SDC_CS = 0xFF;
  GPIO_PORTD_PCTL_R &= ~0xF0000000;
  GPIO_PORTD_AMSEL_R &= ~0x80; // disable analog functionality on PD7
  GPIO_PORTD_DEN_R |= 0x80;    // enable digital I/O on PD7
}

//Used for SSI
void Timer5_Init(void){volatile unsigned short delay;
  SYSCTL_RCGCTIMER_R |= 0x20;
  delay = SYSCTL_SCGCTIMER_R;
  delay = SYSCTL_SCGCTIMER_R;
  TIMER5_CTL_R = 0x00000000;       // 1) disable timer5A during setup
  TIMER5_CFG_R = 0x00000000;       // 2) configure for 32-bit mode
  TIMER5_TAMR_R = 0x00000002;      // 3) configure for periodic mode, default down-count settings
  TIMER5_TAILR_R = 799999;         // 4) reload value, 10 ms, 80 MHz clock
  TIMER5_TAPR_R = 0;               // 5) bus clock resolution
  TIMER5_ICR_R = 0x00000001;       // 6) clear timer5A timeout flag
  TIMER5_IMR_R = 0x00000001;       // 7) arm timeout interrupt
  NVIC_PRI23_R = (NVIC_PRI23_R&0xFFFFFF00)|0x00000040; // 8) priority 2
// interrupts enabled in the main program after all devices initialized
// vector number 108, interrupt number 92
  NVIC_EN2_R = 0x10000000;         // 9) enable interrupt 92 in NVIC
  TIMER5_CTL_R = 0x00000001;       // 10) enable timer5A
}
// Executed every 10 ms
void Timer5A_Handler(void){ 
  TIMER5_ICR_R = 0x00000001;       // acknowledge timer5A timeout
  //disk_timerproc();
}


//********SSI0_Init*****************
// Initialize SSI0 interface to SDC
// inputs: clock divider to set clock frequency

void SSI0_Init(unsigned long CPSDVSR){
  SYSCTL_RCGCGPIO_R |= 0x01;   // activate port A
  SYSCTL_RCGCSSI_R |= 0x01;    // activate SSI0
  CS_Init();                            // activate CS, make it high (deselect)
  Timer5_Init();
  GPIO_PORTA_AFSEL_R |= 0x3C;           // enable alt funct on PA2,4,5
  GPIO_PORTA_PUR_R |= 0x3C;             // enable weak pullup on PA2,3,4,5
  GPIO_PORTA_DEN_R |= 0x3C;             // enable digital I/O on PA2,3,4,5
                                        // configure PA2,3,4, 5 as SSI
  GPIO_PORTA_DIR_R |= 0xC8;             // PA7,PA6,PA3 output (CS to LCD)
  GPIO_PORTA_DATA_R |= 0x88;            // PA7,PA3 high (disable LCD)
  
  GPIO_PORTA_DR4R_R |= 0xEC;            // 4mA output on outputs
  GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0xFF0000FF)+0x00222200;
  GPIO_PORTA_AMSEL_R = 0;               // disable analog functionality on PA
  SSI0_CR1_R &= ~SSI_CR1_SSE;           // disable SSI
  SSI0_CR1_R &= ~SSI_CR1_MS;            // master mode
                                        // clock divider for 8 MHz SSIClk (assumes 16 MHz PIOSC)
  SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+CPSDVSR; 
  // CPSDVSR must be even from 2 to 254
  
  SSI0_CR0_R &= ~(SSI_CR0_SCR_M |       // SCR = 0 (80 Mbps base clock)
                  SSI_CR0_SPH |         // SPH = 0
                  SSI_CR0_SPO);         // SPO = 0
                                        // FRF = Freescale format
  SSI0_CR0_R = (SSI0_CR0_R&~SSI_CR0_FRF_M)+SSI_CR0_FRF_MOTO;
                                        // DSS = 8-bit data
  SSI0_CR0_R = (SSI0_CR0_R&~SSI_CR0_DSS_M)+SSI_CR0_DSS_8;
  SSI0_CR1_R |= SSI_CR1_SSE;            // enable SSI
}

/**********************************
********  DISK FUNCTIONS **********
***** Init, Status, Read, Write ***
***********************************/

//*************** eDisk_Init ***********
// power on, initialize SSI, send clock train (enables SPI mode)
int eDisk_Init(void){
	CS_Init();
	SSI0_Init(200); // 400,000 bps, slow mode
	return SUCCESS;
}

//*************** eDisk_Status ***********
// check status

int eDisk_Status(void){
	
	return SUCCESS;
}

//*************** eDisk_Write ***********
// (read from RAM) Write data to the SD card

int eDisk_Write(char driverNumber, const char* data, int sector, char count){
	send_cmd(0,0);
	
	return SUCCESS;
}

//************* Send Command ************

char send_cmd(char cmd, int arg){
	char response;
	send_data(0);
	
	return response;
	
}

//********* Transmit over SSI ***********

char send_data(char data){
	char response;
	//busy wait SSI
	
	return response;
	
}

//*************** eDisk_Read ***********
// Read data from the SD card  (write to RAM)

int eDisk_Read(void){
	
	
	return SUCCESS;
}




