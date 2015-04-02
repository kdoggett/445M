#include <stdint.h>

/* Implementation of the FIR Filter */
/* Implement the MACQ using two copies of the data */

int32_t Data[102]; 				// two copies of sample data
int32_t *currentValue;		// Pointer to the current value
const long h[51]={2,-2,-1,1,3,-2,-1,0,4,-3,-1,
     -1,5,-3,-1,-2,8,-4,-2,-5,13,-4,-3,-35,108,
     365,108,-35,-3,-4,13,-5,-2,-4,8,-2,-1,-3,5,
     -1,-1,-3,4,0,-1,-2,3,1,-1,-2,2};

void Filter_Init(void) {
	currentValue = &Data[0];
}

/* Calculate one filter output 
 * called at sampling rate
 * Input: new ADC data
 * Output: filter output, ADC data
*/

int32_t Filter_Calc (int16_t newdata) {
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
	return sum/512;
}