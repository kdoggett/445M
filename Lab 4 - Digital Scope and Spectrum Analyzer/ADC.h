#include "FIFO.h"
#include <stdint.h>

#define	PERIOD	80000

void ADC_HardwareTrigger_T0A(int period);
void ADC_SoftwareTrigger(void);

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC_In(void);