int OS_AddPeriodicThread(void(*task)(void),unsigned long period,unsigned long priority);
void PortF_Init(void);
void OS_ClearPeriodicTime(void);
unsigned long OS_ReadPeriodicTime(void);