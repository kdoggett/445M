#include <stdint.h>

/* Implements a MACQ for the Display
 * Input: 32-bit integer value
 * Output: 
*/

#define MACQSIZE 10


int32_t MACQBuffer[MACQSIZE];
int32_t* CurrentValue;
int32_t* OldestValue;

void MACQ_Init (void) {
	CurrentValue = &MACQBuffer[0];
	OldestValue = &MACQBuffer[0];
}

/******* MACQ_PUT *******/
/* Implements a MACQ for the Display
 * Input: 32-bit integer value
 * Output: 
*/
void MACQ_Put (int32_t MACQ_DATA){
	if(CurrentValue == &MACQBuffer[MACQSIZE - 1]){
		CurrentValue = &MACQBuffer[0];								// wrap around
	}
	*CurrentValue = MACQ_DATA; 
	CurrentValue++;
}

/******* MACQ_GET *******/
/* Implements a MACQ for the Display
 * Input: 32-bit integer value
 * Output: 
*/

int32_t MACQ_Get (void){
	if(OldestValue == &MACQBuffer[MACQSIZE - 1]){
		OldestValue = &MACQBuffer[0];								// wrap around
	}
	int32_t temp = *OldestValue;
	OldestValue++;
	return temp;
}

