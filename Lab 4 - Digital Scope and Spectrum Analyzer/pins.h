void Debug_Port_Init(void);
void Switch_Init(void);

//debugging heartbeats
#define		DIO4		(*((volatile unsigned long *)0x40006040))	//PC4
#define		DIO5		(*((volatile unsigned long *)0x40006080))	//PC5
#define		DIO6		(*((volatile unsigned long *)0x40006100))	//PC6
#define		DIO7		(*((volatile unsigned long *)0x40006200))	//PC7
#define 	DIO0  	(*((volatile unsigned long *)0x40024004))	//PE0
#define 	DIO1  	(*((volatile unsigned long *)0x40024008))	//PE1
#define 	DIO2  	(*((volatile unsigned long *)0x40024010))	//PE2
#define 	DIO3  	(*((volatile unsigned long *)0x40024020))	//PE3

#define		BIT0	0x01
#define		BIT1	0x02
#define		BIT2	0x04
#define		BIT3	0x08
#define		BIT4	0x10
#define		BIT5	0x20
#define		BIT6	0x40
#define		BIT7	0x80

//switches and such
#define		PF0		(*((volatile unsigned long *)0x40025004))
#define		PF2		(*((volatile unsigned long *)0x40025010))
#define		PF3		(*((volatile unsigned long *)0x40025020))
#define		PF4		(*((volatile unsigned long *)0x40025040))