#include <stdint.h>

<<<<<<< HEAD
#define	PERIOD	80000

void ADC_HardwareTrigger_T0A(int period);
void ADC_SoftwareTrigger(void);
=======
void ADC_Collect(uint32_t channelNum, uint32_t fs, void(*task)(unsigned long data));

// This initialization function sets up the ADC according to the
// following parameters.  Any parameters not explicitly listed
// below are not modified:
// Max sample rate: <=125,000 samples/second
// Sequencer 0 priority: 1st (highest)
// Sequencer 1 priority: 2nd
// Sequencer 2 priority: 3rd
// Sequencer 3 priority: 4th (lowest)
// SS3 triggering event: software trigger
// SS3 1st sample source: programmable using variable 'channelNum' [0:7]
// SS3 interrupts: enabled but not promoted to controller
void ADC_Open(uint32_t channelNum);
>>>>>>> aa8c9ef2009c12bb4266218f976647f167100e91

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
<<<<<<< HEAD
void ADC_In(void);
=======
uint32_t ADC_In(void);

extern volatile uint32_t ADCvalue;
>>>>>>> aa8c9ef2009c12bb4266218f976647f167100e91
