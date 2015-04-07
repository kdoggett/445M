#include "stdint.h"
#include "edisk.h"
#include <stdio.h>
int StreamToFile = 0;

// filename ************** eFile.h *****************************
// Middle-level routines to implement a solid-state disk 
// Jonathan W. Valvano 3/16/11
#define SUCCESS 1
#define FAIL		0
#define MAXCHUNKS 100									// The entire file system (SD Card) can only support 100 Blocks of 512 Bytes.
#define CHUNKSIZE 512									// Each block contains a total of 512 Bytes.
#define DRCTYSIZE 46									// Size of Directory calculated by take (CHUNKSIZE / (7*sizeof(char) + sizeof(int)))

typedef struct CHUNK CHUNK;

typedef union CHUNK_PTR {
	char buffer[4];
	CHUNK* PTR;
}CHUNK_PTR;

struct CHUNK{
//	unsigned char DATA[(CHUNKSIZE - sizeof(int))];
// 	First few bytes of data are reserved for name of file
	unsigned char BLOCK[CHUNKSIZE - 5];
	unsigned char EMPTY;
	union CHUNK_PTR NEXT_FILE;
};

CHUNK DIRECTORY[1];
CHUNK FILESYSTEM[MAXCHUNKS - 1];									// FileSystem is essentially a 2D array that has 100 blocks, each with 512 bytes of DATA
CHUNK* EMPTYFILES[MAXCHUNKS];									// EmptyFiles is essentually a linked list that connect all empty blocks together.

char* FileToBlock (CHUNK FILE, char File_Name[],char FileBuffer[]){
	uint32_t INDEX = 0;
	while(File_Name[INDEX]){
		FILE.BLOCK[INDEX] = File_Name[INDEX];						// File is being identified with a name.
		INDEX++;
	}
	while(INDEX < CHUNKSIZE){
		FileBuffer[INDEX] = 0;
		INDEX++;
	}
	FileBuffer[CHUNKSIZE - 4] = FILE.NEXT_FILE.buffer[0];
	FileBuffer[CHUNKSIZE - 3] = FILE.NEXT_FILE.buffer[1];
	FileBuffer[CHUNKSIZE - 2] = FILE.NEXT_FILE.buffer[2];
	FileBuffer[CHUNKSIZE - 1] = FILE.NEXT_FILE.buffer[3];
	return FileBuffer;
}
	
//---------- eFile_Init-----------------
// Activate the file system, without formating
// Input: none
// Output: 0 if successful and 1 on failure (already initialized)
// since this program initializes the disk, it must run with 
//    the disk periodic task operating
int eFile_Init(void){

}	// initialize file system

//---------- eFile_Format-----------------
// Erase all files, create blank directory, initialize free space manager
// Input: none
// Output: 0 if successful and 1 on failure (e.g., trouble writing to flash)

int eFile_Format(void) { // erase disk, add format
	int FILE, SIZE, COUNT;
	for(FILE = 0; FILE < MAXCHUNKS; FILE++){
		for(SIZE = 0; SIZE < CHUNKSIZE; SIZE++){
			if(FILE == 0){ DIRECTORY->BLOCK[SIZE] = 0; }			// BLOCK 0 refers to the Directory.
			FILESYSTEM[FILE].BLOCK[SIZE] = 0;
		}
		eDisk_WriteBlock(FILESYSTEM[FILE].BLOCK, FILE);
		FILESYSTEM[FILE].EMPTY = 0;
	}
	COUNT = 0;
	for(FILE = 1; FILE < MAXCHUNKS; FILE++){							// Create an array of CHUNK* to keep track of Empty Blocks
		if(FILESYSTEM[FILE].EMPTY == 0){
			EMPTYFILES[COUNT] = &FILESYSTEM[FILE];
			COUNT++;
		}
	}
	return SUCCESS;	
}
//---------- eFile_Create-----------------
// Create a new, empty file with one allocated block
// Input: file name is an ASCII string up to seven characters 
// Output: 0 if successful and 1 on failure (e.g., trouble writing to flash)
int eFile_Create( char name[]){  // create new file, make it empty
	int COUNT = 0;
	while(COUNT < MAXCHUNKS){
		if(EMPTYFILES[COUNT] == 0){ return FAIL;} 
		//else{
			
		}
}

//---------- eFile_WOpen-----------------
int eFile_WOpen(char name[]){


	return 0;
}

//---------- eFile_Write-----------------
int eFile_Write( char data){

	return 0;
}	

//---------- eFile_Close-----------------
int eFile_Close(void){
	
	
	return 0;
}


//---------- eFile_WClose-----------------
int eFile_WClose(void){


	return 0;
}

//---------- eFile_ROpen-----------------
int eFile_ROpen( char name[]){

	return 0;
}
   
//---------- eFile_ReadNext-----------------
int eFile_ReadNext( char *pt){


	return 0;
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


	return 0;
}  

//---------- eFile_Delete-----------------
int eFile_Delete( char name[]){


	return 0;
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

