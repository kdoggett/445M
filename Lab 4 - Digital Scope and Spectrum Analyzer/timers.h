

void Timer3_Init(void);
void Timer3A_Launch(void(*task)(void), int period, unsigned long priority);
void Timer2A_Launch(void(*task)(void), int period, unsigned long priority);
void Timer2_Init(void);