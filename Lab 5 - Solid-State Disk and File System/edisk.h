
/* Definitions for MMC/SDC command */
#define CMD0    (0x40+0)    /* GO_IDLE_STATE */
#define CMD1    (0x40+1)    /* SEND_OP_COND */
#define CMD8    (0x40+8)    /* SEND_IF_COND */
#define CMD9    (0x40+9)    /* SEND_CSD */
#define CMD10    (0x40+10)    /* SEND_CID */
#define CMD12    (0x40+12)    /* STOP_TRANSMISSION */
#define CMD16    (0x40+16)    /* SET_BLOCKLEN */
#define CMD17    (0x40+17)    /* READ_SINGLE_BLOCK */
#define CMD18    (0x40+18)    /* READ_MULTIPLE_BLOCK */
#define CMD23    (0x40+23)    /* SET_BLOCK_COUNT */
#define CMD24    (0x40+24)    /* WRITE_BLOCK */
#define CMD25    (0x40+25)    /* WRITE_MULTIPLE_BLOCK */
#define CMD41    (0x40+41)    /* SEND_OP_COND (ACMD) */
#define CMD55    (0x40+55)    /* APP_CMD */
#define CMD58    (0x40+58)    /* READ_OCR */

#define SUCCESS 1
#define FAIL		0

//*************** eDisk_Init ***********
// power on, initialize SSI, send clock train (enables SPI mode)

int eDisk_Init(void);

//*************** eDisk_Status ***********
// check status

int eDisk_Status(void);

//*************** eDisk_Read ***********
// Read data from the SD card  (write to RAM)

int eDisk_Read(void);

//*************** eDisk_Write ***********
// (read from RAM) Write data to the SD card

int eDisk_Write(char driverNumber, const char* data, int sector, char count);

//************* Send Command ************

char send_cmd(char,int);

//********* Transmit over SSI ***********

char send_data(char);