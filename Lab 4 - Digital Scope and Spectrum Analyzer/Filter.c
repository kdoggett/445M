#include <stdint.h>

/* Implementation of the FIR Filter */
/* Implement the MACQ using two copies of the data */

int32_t Data[102]; 				// two copies of sample data
int32_t *currentValue;		// Pointer to the current value
const int32_t h[51]={4,-1,-8,-14,-16,-10,-1,6,5,-3,-13,
     -15,-8,3,5,-5,-20,-25,-8,25,46,26,-49,-159,-257,
     984,-257,-159,-49,26,46,25,-8,-25,-20,-5,5,3,-8,
     -15,-13,-3,5,6,-1,-10,-16,-14,-8,-1,4};

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
	int32_t *copyCurrentValue;
	const int32_t *coefficients;
	if(currentValue == &Data[0]) {
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