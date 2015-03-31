/* Initialize Filter Pointer */
void Filter_Init(void);

/* Calculate one filter output called at sampling rate 
 * Input: New ADC value
 * Output: Filter Output
*/
int16_t Filter_Calc (int16_t);
