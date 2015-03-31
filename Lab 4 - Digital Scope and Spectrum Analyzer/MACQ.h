#include <stdint.h>

/***** MACQ_INIT ****
* INITIALIZE POINTERS OF MACQ *
*/
void MACQ_Init (void);


/**** MACQ_PUT ******
* MUCH LIKE FIFO PUT BUT WRAPS AROUND *
*/
void MACQ_Put (int32_t);


/**** MACQ_GET ******
* MUCH LIKE FIFO GET BUT WRAPS AROUND *
*/
int32_t MACQ_Get (void);