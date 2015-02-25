void Debug_Port_Init(void);
void Switch_Init(void);

//debugging heartbeats
#define		PC4_DIO4		(*((volatile unsigned long *)0x40006040))
#define		PC5_DIO5		(*((volatile unsigned long *)0x40006080))
#define		PC6_DIO6		(*((volatile unsigned long *)0x40006100))
#define		PC7_DIO7		(*((volatile unsigned long *)0x40006200))
#define 	PE0_DIO0  	(*((volatile unsigned long *)0x40024004))
#define 	PE1_DIO1  	(*((volatile unsigned long *)0x40024008))
#define 	PE2_DIO2  	(*((volatile unsigned long *)0x40024010))
#define 	PE3_DIO3  	(*((volatile unsigned long *)0x40024020))


//switches and such
#define		PF2		(*((volatile unsigned long *)0x40025010))
#define		PF3		(*((volatile unsigned long *)0x40025020))
#define		PF4		(*((volatile unsigned long *)0x40025040))