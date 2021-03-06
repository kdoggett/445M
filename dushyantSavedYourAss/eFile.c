#include "stdint.h"
#include "edisk.h"
#include <stdio.h>
int StreamToFile = 0;

// filename ************** eFile.h *****************************
// Middle-level routines to implement a solid-state disk 
// Jonathan W. Valvano 3/16/11
#define SUCCESS 0
#define FAIL		1
#define MAXBLOCKS 100									// The entire file system (SD Card) can only support 100 Blocks of 512 Bytes.
#define BLOCKSIZE 512									// Each block contains a total of 512 Bytes.
#define DRCTYSIZE 4									// Size of Directory calculated by take (BLOCKSIZE / (7*sizeof(char) + sizeof(int)))

typedef struct BLOCK{
	unsigned char DATA[BLOCKSIZE - 1];
	unsigned char NEXT_FILE_INDEX;
}BLOCK;

typedef struct EMPTY{
	unsigned char NEXT_FILE_INDEX
}EMPTY; 

typedef struct ID{
	char FILE_NAME[7];
	unsigned char FILE_INDEX;
}ID;

typedef struct DIRECTORY{
	ID FILE[DRCTYSIZE];
	unsigned char FIRSTEMPTYFILE;																// EmptyFiles is essentually a linked list that connect all empty blocks together.
}DIRECTORY;

typedef struct Layout{
	DIRECTORY Directory[1];
	BLOCK FILES[MAXBLOCKS - 1];											// FileSystem is essentially a 2D array that has 100 blocks, each with 512 bytes of DATA
}Layout;

Layout FILESYSTEM;
unsigned char CurrentOpenBlock[BLOCKSIZE];
unsigned char OpenBlock;
uint16_t CurrentBlockPosition[BLOCKSIZE];
char FileSystemFlag = 0;

unsigned char* CreateEmptyBuffer (unsigned char FileBuffer[], uint16_t BufferSize){
	int INDEX;
	for(INDEX = 0; INDEX < BufferSize; INDEX++){
		FileBuffer[INDEX] = 0;
	}
	return FileBuffer;
}

void AddEmptyIndex(unsigned char Empty_Index){
	uint32_t FirstEmptyBlock = FILESYSTEM.Directory[0].FIRSTEMPTYFILE;
	BLOCK CurrentBlock = FILESYSTEM.FILES[FirstEmptyBlock];
	while(CurrentBlock.NEXT_FILE_INDEX != 0){
		CurrentBlock = FILESYSTEM.FILES[CurrentBlock.NEXT_FILE_INDEX];
	}
	CurrentBlock.NEXT_FILE_INDEX = Empty_Index;
	CurrentBlock = FILESYSTEM.FILES[CurrentBlock.NEXT_FILE_INDEX];
	CurrentBlock.NEXT_FILE_INDEX = 0;	
}

unsigned char* FileToBlock (BLOCK File, unsigned char FileBuffer[]){
	uint32_t INDEX = 0;
	while(INDEX < BLOCKSIZE - 1){
		FileBuffer[INDEX] = File.DATA[INDEX];
	}
	return FileBuffer;
}

void BlockToFile(BLOCK File, unsigned char FileBuffer[]){
	uint32_t INDEX = 0;
	while(INDEX < BLOCKSIZE - 1){
		File.DATA[INDEX] = FileBuffer[INDEX];
	}
	File.NEXT_FILE_INDEX = FileBuffer[BLOCKSIZE -1];
}

void AddFileToDirectory(char* File_Name, unsigned char Block_Index){
	OpenBlock = 0;
	eDisk_ReadBlock(CurrentOpenBlock, OpenBlock);
	uint16_t index = 0;
	while(CurrentBlockPosition[OpenBlock] < BLOCKSIZE - 1){
		while(File_Name[index] != 0){
			CurrentOpenBlock[CurrentBlockPosition[OpenBlock]] = File_Name[index];
			index++;
			CurrentBlockPosition[OpenBlock]++;
		}
		CurrentBlockPosition[OpenBlock] = Block_Index;
	}
	eDisk_WriteBlock(CurrentOpenBlock, OpenBlock);
	for(index = 0; File_Name[index] != 0; index++){
		FILESYSTEM.Directory[0].FILE[0].FILE_NAME[index] = File_Name[index];
	}
	FILESYSTEM.Directory[0].FILE[0].FILE_INDEX = Block_Index;
}

//---------- eFile_Init-----------------
// Activate the file system, without formating
// Input: none
// Output: 0 if successful and 1 on failure (already initialized)
// since this program initializes the disk, it must run with 
//    the disk periodic task operating
int eFile_Init(void){
	FileSystemFlag = 1;
}	// initialize file system

//---------- eFile_Format-----------------
// Erase all files, create blank directory, initialize free space manager
// Input: none
// Output: 0 if successful and 1 on failure (e.g., trouble writing to flash)

int eFile_Format(void) { // erase disk, add format
	char FILE;
	unsigned char *EmptyBuffer = CreateEmptyBuffer(EmptyBuffer, BLOCKSIZE);
// BLOCK 1 IS THE FIRST EMPTY BLOCK. INITIALIZE VALUE ACCORDINGLY //
	FILESYSTEM.Directory[0].FIRSTEMPTYFILE = 1;
	FILESYSTEM.FILES[1].NEXT_FILE_INDEX = 0;
/*****************************************************************/
// FILL 100 BLOCKS WITH AN EMPTY BUFER OF SIZE = BLOCKSIZE AND ADD LINK THEM TOGETHER VIA FREE SPACE MANAGEMENT //
	for(FILE = 0; FILE < MAXBLOCKS; FILE++){
		for(int i = 0; i < BLOCKSIZE - 1; i++){
			FILESYSTEM.FILES[FILE].DATA[i] = 0;
		}
		FILESYSTEM.FILES[FILE].NEXT_FILE_INDEX = 0;
		if(FILE > 1){
			AddEmptyIndex(FILE);
		}
		EmptyBuffer = FileToBlock(FILESYSTEM.FILES[FILE], EmptyBuffer);
		if(eDisk_WriteBlock(EmptyBuffer, FILE)){
			return FAIL;
		}
	}
/*****************************************************************/
// EMPTY DATA IN DIRECTORY TO CREATE A "BLANK" DIRECTORY //
	for(FILE = 0; FILE < DRCTYSIZE; FILE++){
		for(int i = 0; i < 7; i ++){
			FILESYSTEM.Directory[0].FILE[FILE].FILE_NAME[i] = 0;
		}
		FILESYSTEM.Directory[0].FILE[FILE].FILE_INDEX = 0;
	}
	if(eDisk_WriteBlock(EmptyBuffer, 0)){
		return FAIL;
	}
/******************************************************************/
	return SUCCESS;	
}
//---------- eFile_Create-----------------
// Create a new, empty file with one allocated block
// Input: file name is an ASCII string up to seven characters 
// Output: 0 if successful and 1 on failure (e.g., trouble writing to flash)
int eFile_Create(char name[]){  // create new file, make it empty
	if(FileSystemFlag == 1){
		unsigned char EmptyBlockNumber = FILESYSTEM.Directory[0].FIRSTEMPTYFILE;
		CurrentBlockPosition[EmptyBlockNumber] = 0;
		OpenBlock = EmptyBlockNumber;
		eDisk_ReadBlock(CurrentOpenBlock, OpenBlock);
		while(name[CurrentBlockPosition[OpenBlock]] != 0){
			CurrentOpenBlock[CurrentBlockPosition[OpenBlock]] = name[CurrentBlockPosition[OpenBlock]];
			CurrentBlockPosition[OpenBlock]++;
		}
		eDisk_WriteBlock(CurrentOpenBlock, OpenBlock);
		BlockToFile(FILESYSTEM.FILES[OpenBlock], CurrentOpenBlock);
		AddFileToDirectory(name, OpenBlock);
		return SUCCESS;
	}
	return FAIL;
}

//---------- eFile_WOpen-----------------
int eFile_WOpen(char name[]){
	if(FileSystemFlag == 1){
		for(int i = 0; i < DRCTYSIZE; i++){
			if(strcmp(FILESYSTEM.Directory[0].FILE[i].FILE_NAME,name ) == 0){
				OpenBlock = FILESYSTEM.Directory[0].FILE[i].FILE_INDEX;
			}
		}
		if(OpenBlock == 0){return 1;}
		return(eDisk_ReadBlock(CurrentOpenBlock,OpenBlock));
	}
	return FAIL;
}

//---------- eFile_Write-----------------
int eFile_Write( char data){
	if(FileSystemFlag == 1){
		if(CurrentBlockPosition[OpenBlock] == BLOCKSIZE - 1){					//Create a new file
			unsigned char EmptyBlockNumber = FILESYSTEM.Directory[0].FIRSTEMPTYFILE;
			CurrentOpenBlock[BLOCKSIZE - 1] = EmptyBlockNumber;
			eDisk_WriteBlock(CurrentOpenBlock, OpenBlock);
			OpenBlock = EmptyBlockNumber;
			CurrentBlockPosition[OpenBlock] = 0;		
		}
		CurrentOpenBlock[CurrentBlockPosition[OpenBlock]] = data;
		CurrentBlockPosition[OpenBlock]++;
		return(eDisk_WriteBlock(CurrentOpenBlock, OpenBlock));
	}
	return 1;
}	

//---------- eFile_Close-----------------
int eFile_Close(void){
	FileSystemFlag = 0;
	return 0;
}


//---------- eFile_WClose-----------------
int eFile_WClose(void){
	if(FileSystemFlag == 1){
		return(eDisk_WriteBlock(CurrentOpenBlock,OpenBlock));
	}
	return FAIL;
}

//---------- eFile_ROpen-----------------
int eFile_ROpen( char name[]){
	if(FileSystemFlag == 1){
		for(int i = 0; i < DRCTYSIZE; i++){
			if(strcmp(FILESYSTEM.Directory[0].FILE[i].FILE_NAME, name) == 0){
				OpenBlock = FILESYSTEM.Directory[0].FILE[i].FILE_INDEX;
				eDisk_ReadBlock(CurrentOpenBlock, OpenBlock);
				return 0;
			}
		}
	}
	return 1;
}  

//---------- eFile_ReadNext-----------------
int eFile_ReadNext( char *pt){
	if(FileSystemFlag == 1){
		return CurrentOpenBlock[CurrentBlockPosition[OpenBlock] + 1];
	}
}
                              
//---------- eFile_RClose-----------------
// close the reading file
// Input: none
// Output: 0 if successful and 1 on failure (e.g., wasn't open)
int eFile_RClose(void){


	return 0;
}

//---------- eFile_Directory-----------------
int eFile_Directory(void(*fp)(unsigned char)){
	eDisk_ReadBlock(CurrentOpenBlock , 0);
	int i = 0;
	while(FILESYSTEM.Directory[0].FILE[i].FILE_NAME[0] != 0){
		for(int j = 0; j < 7; j++){
			fp(FILESYSTEM.Directory[0].FILE[i].FILE_NAME[j]);
		}
		i++;
	}
	return 0;
}  

//---------- eFile_Delete-----------------
int eFile_Delete( char name[]){
	for(int i = 0; i < DRCTYSIZE; i++){
		if(strcmp(FILESYSTEM.Directory[0].FILE[i].FILE_NAME, name) == 0){
			OpenBlock = FILESYSTEM.Directory[0].FILE[i].FILE_INDEX;
			AddEmptyIndex(OpenBlock);
			for(int j = 0; j < 7; j++){
				FILESYSTEM.Directory[0].FILE[i].FILE_NAME[j] = 0;
			}
			FILESYSTEM.Directory[0].FILE[i].FILE_INDEX = 0;
			unsigned char *EmptyBuffer = CreateEmptyBuffer(EmptyBuffer, BLOCKSIZE);
			eDisk_WriteBlock(EmptyBuffer, 0);
			return 0;
		}
	}
	return 1;
}
//---------- eFile_RedirectToFile-----------------
int eFile_RedirectToFile(char *name){
	eFile_Create(name);
	if(eFile_WOpen(name)) return 1;
	StreamToFile = 1;
	return 0;
}

//---------- eFile_EndRedirectToFile-----------------
int eFile_EndRedirectToFile(void){
	StreamToFile = 0;
	if(eFile_WClose()) return 1;	
	return 0;
}

int fputc(int ch, FILE *f){
	if(StreamToFile){
		if(eFile_Write(ch)){
			eFile_EndRedirectToFile();
			return 1;
		}
		return 0;
	}
	UART_OutChar(ch);
	return 0;
}

int fgetc(FILE *f){
	return (UART_InChar());
}

