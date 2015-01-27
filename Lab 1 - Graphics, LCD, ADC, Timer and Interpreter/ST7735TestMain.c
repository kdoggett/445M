// ST7735TestMain.c
// Runs on LM4F120/TM4C123
// Test the functions in ST7735.c by printing basic
// patterns to the LCD.
// Daniel Valvano
// September 12, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected 
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO)
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground
#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"
#include "PLL.h"
#include "tm4c123gh6pm.h"

void DelayWait10ms(uint32_t n);

// test image
// [blue] [green]
// [red ] [white]
const uint16_t Test[] = {
  0x001F, 0xFFFF, 0xF800, 0x07E0
};
// test image2
// [red]   [yellow]  [green]   [blue]    [black]
// [red]   [yellow]  [green]   [blue]    [white]
// [red]   [yellow]  [green]   [blue]    [black]
// [red]   [yellow]  [green]   [blue]    [white]
// [red]   [yellow]  [green]   [blue]    [black]
// [black] [75%grey] [50%grey] [25%grey] [white]
const uint16_t Test2[] = {
  0x0000, 0x4208, 0x8410, 0xC618, 0xFFFF,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0x0000,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0xFFFF,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0x0000,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0xFFFF,
  0x001F, 0x07FF, 0x07E0, 0xF800, 0x0000
};


int main0(void){
  PLL_Init();
  Output_Init();
  printf("hello world");
  while(1){
  }
} 
int mainTFTstable(void){
  PLL_Init();
  ST7735_InitR(INITR_REDTAB);
	ST7735_DrawFastHLine(0, 80, 128, ST7735_YELLOW);
	ST7735_Message(1,1,"Line 1:",1);
	ST7735_Message(1,2,"Line 2:",2);
	ST7735_Message(1,3,"Line 3:",3);
	ST7735_Message(2,1,"Line 4:",4);
	ST7735_Message(2,2,"Line 5:",5);
	ST7735_Message(2,3,"Line 6:",6);
  //ST7735_PlotClear(0,4095);  // range from 0 to 4095s
  while(1){
  }
}
int main2(void){
  int x, y, dx, dy;
//  uint8_t red, green, blue;
  PLL_Init();                           // set system clock to 80 MHz
  // test DrawChar() and DrawCharS()
  ST7735_InitR(INITR_REDTAB);
  ST7735_DrawCharS(0, 0, 'c', ST7735_Color565(255, 0, 0), 0, 1);
  ST7735_DrawCharS(6, 0, 'h', ST7735_Color565(255, 128, 0), 0, 1);
  ST7735_DrawCharS(12, 0, 'a', ST7735_Color565(255, 255, 0), 0, 1);
  ST7735_DrawCharS(18, 0, 'r', ST7735_Color565(128, 255, 0), 0, 1);
  ST7735_DrawCharS(30, 0, 'T', ST7735_Color565(0, 255, 0), 0, 1);
  ST7735_DrawCharS(36, 0, 'E', ST7735_Color565(0, 255, 255), 0, 1);
  ST7735_DrawCharS(42, 0, 'S', ST7735_Color565(0, 0, 255), 0, 1);
  ST7735_DrawCharS(48, 0, 'T', ST7735_Color565(128, 0, 255), 0, 1);

  ST7735_DrawCharS(0, 8, 'c', ST7735_Color565(255, 0, 0), 0, 2);
  ST7735_DrawCharS(12, 8, 'h', ST7735_Color565(255, 128, 0), 0, 2);
  ST7735_DrawCharS(24, 8, 'a', ST7735_Color565(255, 255, 0), 0, 2);
  ST7735_DrawCharS(36, 8, 'r', ST7735_Color565(128, 255, 0), 0, 2);
  ST7735_DrawCharS(60, 8, 'T', ST7735_Color565(0, 255, 0), 0, 2);
  ST7735_DrawCharS(72, 8, 'E', ST7735_Color565(0, 255, 255), 0, 2);
  ST7735_DrawCharS(84, 8, 'S', ST7735_Color565(0, 0, 255), 0, 2);
  ST7735_DrawCharS(96, 8, 'T', ST7735_Color565(128, 0, 255), 0, 2);

  ST7735_DrawCharS(0, 24, 'c', ST7735_Color565(255, 0, 0), 0, 3);
  ST7735_DrawCharS(18, 24, 'h', ST7735_Color565(255, 128, 0), 0, 3);
  ST7735_DrawCharS(36, 24, 'a', ST7735_Color565(255, 255, 0), 0, 3);
  ST7735_DrawCharS(54, 24, 'r', ST7735_Color565(128, 255, 0), 0, 3);
  ST7735_DrawCharS(90, 24, 'T', ST7735_Color565(0, 255, 0), 0, 3);
  ST7735_DrawCharS(108, 24, 'E', ST7735_Color565(0, 255, 255), 0, 3);
  ST7735_DrawCharS(126, 24, 'S', ST7735_Color565(0, 0, 255), 0, 3); // off the screen
  ST7735_DrawCharS(144, 24, 'T', ST7735_Color565(128, 0, 255), 0, 3); // totally off the screen

  ST7735_DrawCharS(0, 48, 'c', ST7735_Color565(255, 0, 0), 0, 4);
  ST7735_DrawCharS(24, 48, 'h', ST7735_Color565(255, 128, 0), 0, 4);
  ST7735_DrawCharS(48, 48, 'a', ST7735_Color565(255, 255, 0), 0, 4);
  ST7735_DrawCharS(72, 48, 'r', ST7735_Color565(128, 255, 0), 0, 4);
  ST7735_DrawCharS(120, 48, 'T', ST7735_Color565(0, 255, 0), 0, 4); // off the screen
  ST7735_DrawCharS(144, 48, 'E', ST7735_Color565(0, 255, 255), 0, 4); // totally off the screen
  ST7735_DrawCharS(168, 48, 'S', ST7735_Color565(0, 0, 255), 0, 4); // totally off the screen
  ST7735_DrawCharS(192, 48, 'T', ST7735_Color565(128, 0, 255), 0, 4); // totally off the screen

  ST7735_DrawCharS(0, 80, 'c', ST7735_Color565(255, 0, 0), 0, 6);
  ST7735_DrawCharS(36, 80, 'h', ST7735_Color565(255, 128, 0), 0, 6);
  ST7735_DrawCharS(72, 80, 'a', ST7735_Color565(255, 255, 0), 0, 6);
  ST7735_DrawCharS(108, 80, 'r', ST7735_Color565(128, 255, 0), 0, 6); // off the screen

  ST7735_DrawChar(0, 135, 'A', ST7735_Color565(255, 128, 128), 0, 2);
  ST7735_DrawChar(12, 135, 'd', ST7735_Color565(255, 0, 0), 0, 2);
  ST7735_DrawChar(24, 135, 'v', ST7735_Color565(255, 128, 0), 0, 2);
  ST7735_DrawChar(48, 135, 'c', ST7735_Color565(255, 255, 0), 0, 2);
  ST7735_DrawChar(60, 135, 'h', ST7735_Color565(128, 255, 0), 0, 2);
  ST7735_DrawChar(72, 135, 'a', ST7735_Color565(0, 255, 0), 0, 2);
  ST7735_DrawChar(84, 135, 'r', ST7735_Color565(0, 255, 255), 0, 2);
  ST7735_DrawChar(108, 135, 't', ST7735_Color565(0, 0, 255), 0, 2);
  ST7735_DrawChar(120, 135, 'e', ST7735_Color565(128, 0, 255), 0, 2); // off the screen
  ST7735_DrawChar(132, 135, 's', ST7735_Color565(255, 0, 255), 0, 2); // totally off the screen
  ST7735_DrawChar(144, 135, 't', ST7735_Color565(255, 255, 255), 0, 2); // totally off the screen

  ST7735_DrawChar(0, 151, 'A', ST7735_Color565(255, 128, 128), 0, 1);
  ST7735_DrawChar(6, 151, 'd', ST7735_Color565(255, 0, 0), 0, 1);
  ST7735_DrawChar(12, 151, 'v', ST7735_Color565(255, 128, 0), 0, 1);
  ST7735_DrawChar(24, 151, 'c', ST7735_Color565(255, 255, 0), 0, 1);
  ST7735_DrawChar(30, 151, 'h', ST7735_Color565(128, 255, 0), 0, 1);
  ST7735_DrawChar(36, 151, 'a', ST7735_Color565(0, 255, 0), 0, 1);
  ST7735_DrawChar(42, 151, 'r', ST7735_Color565(0, 255, 255), 0, 1);
  ST7735_DrawChar(54, 151, 't', ST7735_Color565(0, 0, 255), 0, 1);
  ST7735_DrawChar(60, 151, 'e', ST7735_Color565(128, 0, 255), 0, 1);
  ST7735_DrawChar(66, 151, 's', ST7735_Color565(255, 0, 255), 0, 1);
  ST7735_DrawChar(72, 151, 't', ST7735_Color565(255, 255, 255), 0, 1);
  DelayWait10ms(1000);

  // test DrawBitmap() including clipping test with images off the screen
  ST7735_FillScreen(0xFFFF);            // set screen to white
  ST7735_DrawBitmap(0, 5, Test2, 5, 6);   // top left corner of the screen
  ST7735_DrawBitmap(59, 74, Test2, 5, 6); // center of the screen
  ST7735_DrawBitmap(125, 74, Test2, 5, 6);// clip right
  ST7735_DrawBitmap(59, 162, Test2, 5, 6);// clip bottom
  ST7735_DrawBitmap(-2, 74, Test2, 5, 6); // clip left
  ST7735_DrawBitmap(59, 2, Test2, 5, 6);  // clip top
  DelayWait10ms(1000);

  ST7735_FillScreen(0xFFFF);            // set screen to white
  x = 44;
  y = 159;
  dx = 1;
  dy = 1;
  while(1){
    //ST7735_DrawBitmap(x, y, Logo, 40, 160);
    x = x + dx;
    y = y + dy;
    if((x >= (ST7735_TFTWIDTH - 40 + 15)) || (x <= -15)){
      dx = -1*dx;
    }
    if((y >= (ST7735_TFTHEIGHT + 8)) || (y <= (ST7735_TFTHEIGHT - 8))){
      dy = -1*dy;
    }
  }
}
// private function draws a color band on the screen
void static drawthecolors(uint8_t red, uint8_t green, uint8_t blue){
  static uint16_t y = 0;
  ST7735_DrawFastHLine(0, y, ST7735_TFTWIDTH, ST7735_Color565(red, green, blue));
  y = y + 1;
  if(y >= ST7735_TFTHEIGHT){
     y = 0;
  }
  DelayWait10ms(1);
}
int main3(void){
  uint8_t red, green, blue;
  PLL_Init();                           // set system clock to 80 MHz
  // test DrawChar() and DrawCharS()
  ST7735_InitR(INITR_REDTAB);

  // test display with a colorful demo
  red = 255;
  green = 0;
  blue = 0;
  while(1){
    // transition from red to yellow by increasing green
    for(green=0; green<255; green=green+1){
      drawthecolors(red, green, blue);
    }
    // transition from yellow to green by decreasing red
    for(red=255; red>0; red=red-1){
      drawthecolors(red, green, blue);
    }
    // transition from green to light blue by increasing blue
    for(blue=0; blue<255; blue=blue+1){
      drawthecolors(red, green, blue);
    }
    // transition from light blue to true blue by decreasing green
    for(green=255; green>0; green=green-1){
      drawthecolors(red, green, blue);
    }
    // transition from true blue to pink by increasing red
    for(red=0; red<255; red=red+1){
      drawthecolors(red, green, blue);
    }
    // transition from pink to red by decreasing blue
    for(blue=255; blue>0; blue=blue-1){
      drawthecolors(red, green, blue);
    }
  }
}
void BookExamples(void){ // examples from the book
  int8_t cc = 0x56; // (‘V’)
  int32_t xx = 100;
  int16_t yy = -100;
  float zz = 3.14159265;
  printf("Hello world\n");	    //Hello world
  printf("cc = %c %d\n",cc,cc);	//cc = V 86 
  printf("cc = %#x\n",cc);	    //cc = 0x56
  printf("xx = %c %d\n",xx,xx);	//xx = d 100 
  printf("xx = %#x\n",xx);	    //xx = 0x64
  printf("yy = %d\n",yy);	      //yy = -100 
  printf("%#x   \n",yy);	      //yy = 0xffffff9c
  printf("%e \n",zz);	          //zz = 3.14159e+00
  printf("%E \n",zz);	          //zz = 3.14159E+00
  printf("%f     \n",zz);	      //zz = 3.14159
  printf("%g     \n",zz);	      //zz = 3.14159 (shorter of two, either f or e)
  printf("%3.2f     \n",zz);	  //zz =  3.14
}
#define PF2   (*((volatile uint32_t *)0x40025010))

// Make PF2 an output, enable digital I/O, ensure alt. functions off
void SSR_Init(void){ 
  SYSCTL_RCGCGPIO_R |= 0x20;        // 1) activate clock for Port F
  while((SYSCTL_PRGPIO_R&0x20)==0){}; // allow time for clock to start
                                    // 2) no need to unlock PF2
  GPIO_PORTF_PCTL_R &= ~0x00000F00; // 3) regular GPIO
  GPIO_PORTF_AMSEL_R &= ~0x04;      // 4) disable analog function on PF2
  GPIO_PORTF_DIR_R |= 0x04;         // 5) set direction to output
  GPIO_PORTF_AFSEL_R &= ~0x04;      // 6) regular port function
  GPIO_PORTF_DEN_R |= 0x04;         // 7) enable digital port
}


void Delay1ms(uint32_t n);
int main4(void){ 
  SSR_Init();
  while(1){
    Delay1ms(10);
    PF2 ^= 0x04;
  }
}
int main5(void){ 
  SSR_Init();
  while(1){
    DelayWait10ms(1000);
    PF2 ^= 0x04;
  }
}
int main6(void){ int32_t i,n;
  Output_Init();              // initialize output device
  BookExamples();
  n = 0;
  while(1){
    printf("\ni="); 
    for(i=0; i<1; i++){
      printf("%d ",i+n);
    }

    n = n+10000000; // notice what happens when this goes above 2,147,483,647
  }
}
// Subroutine to wait 10 msec
// Inputs: None
// Outputs: None
// Notes: ...
void DelayWait10ms(uint32_t n){uint32_t volatile time;
  while(n){
    time = 727240*2/91;  // 10msec
    while(time){
	  	time--;
    }
    n--;
  }
}
