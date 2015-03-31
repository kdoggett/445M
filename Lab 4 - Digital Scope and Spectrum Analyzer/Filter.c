#include <stdint.h>

/* Implementation of the FIR Filter */
/* Implement the MACQ using two copies of the data */

int16_t Data[102]; 				// two copies of sample data
int16_t *currentValue;		// Pointer to the current value

void Filter_Init(void) {
	currentValue = &Data[0];
}

/* Calculate one filter output 
 * called at sampling rate
 * Input: new ADC data
 * Output: filter output, ADC data
*/

int16_t Filter_Calc (int16_t newdata) {
	int i;
	int32_t sum; 
	int16_t *copyCurrentValue,*coefficients;
	if(currentValue = &Data[0]) {
		currentValue = &Data[50]; //wrap
	}
	else {
		currentValue--;						// make room for new data
	}
	*currentValue = *(currentValue + 51) = newdata; // two copies of incoming data
	copyCurrentValue = currentValue;								// copy of data pointers
	coefficients = h;																// pointer to coefficients;
	sum = 0;
	for(i = 51; i; i--){
		sum += (*copyCurrentValue)*(*coefficients);
		copyCurrentValue++;
		coefficients++;
	}
	return sum/16384;
}