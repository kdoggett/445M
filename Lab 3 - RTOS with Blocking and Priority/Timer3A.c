#include <stdint.h>
#include "tm4c123gh6pm.h"


// ***************** Timer3A_Init ****************
// Does not trigger interrupt, only used for general purpose timing
// Used by OS_ClearMsTime() and OS_MsTime()
void Timer3A_Init(unsigned long period){ volatile unsigned long delay;
  SYSCTL_RCGCTIMER_R |= 0x08;			// activate
	delay = SYSCTL_RCGC2_R;					// settle
  TIMER3_CTL_R = 0x00;    				// disable during setup
  TIMER3_CFG_R = 0x00;    				// 32-bit mode
	TIMER3_TAILR_R = period - 1;
  TIMER3_TAMR_R = 0x02;   				// periodic mode, default down-count settings
  TIMER3_TAPR_R = 0;            	// bus clock resolution
	TIMER3_CTL_R = 0x01;    				// enable TIMER3A
}