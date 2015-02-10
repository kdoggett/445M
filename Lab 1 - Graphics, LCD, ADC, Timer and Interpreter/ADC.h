int ADC_Open(unsigned int channelNum,unsigned int period);
unsigned short ADC_In(void);
int ADC_Collect(unsigned int chanNum,unsigned int fs,unsigned short buffer[],unsigned int numberOfSamples);
int ADC_Status(void);