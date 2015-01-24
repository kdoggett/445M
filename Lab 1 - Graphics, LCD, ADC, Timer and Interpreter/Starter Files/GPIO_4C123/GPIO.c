// GPIO.c
// Runs on LM4F120/TM4C123
// Initialize four GPIO pins as outputs.  Continually generate output to
// drive simulated stepper motor.
// Daniel Valvano
// May 28, 2014

/* This example accompanies the books
  "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
  ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
  Volume 1 Program 4.5

"Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Volume 2 Example 2.2, Program 2.8, Figure 2.32

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

// PD3 is an output to LED3, negative logic
// PD2 is an output to LED2, negative logic
// PD1 is an output to LED1, negative logic
// PD0 is an output to LED0, negative logic
#include <stdint.h>
#include "inc/tm4c123gh6pm.h"


#define LEDS (*((volatile uint32_t *)0x4000703C))
// access PD3-PD0

void GPIO_Init(void){  
  SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate port D
  while((SYSCTL_PRGPIO_R&0x08)==0){};   // allow time for clock to stabilize
                                    // 2) no need to unlock PD3-0
  GPIO_PORTD_AMSEL_R &= ~0x0F;      // 3) disable analog functionality on PD3-0
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // 4) GPIO
  GPIO_PORTD_DIR_R |= 0x0F;         // 5) make PD3-0 out
  GPIO_PORTD_AFSEL_R &= ~0x0F;      // 6) regular port function 
  GPIO_PORTD_DEN_R |= 0x0F;         // 7) enable digital I/O on PD3-0
}
int main(void){  
  GPIO_Init();
  while(1){
    LEDS = 10; // 1010, LED is 0101
    LEDS = 9;  // 1001, LED is 0110
    LEDS = 5;  // 0101, LED is 1010
    LEDS = 6;  // 0110, LED is 1001
  }
}

// Program 2.8 from Volume 2
#define LEDS2  (*((volatile uint32_t *)0x4002403C))
int main2(void){
  SYSCTL_RCGCGPIO_R |= 0x10;        // 1) activate clock for Port E
  while((SYSCTL_PRGPIO_R&0x10) == 0){};// ready?
  GPIO_PORTE_DIR_R |= 0x0F;         // PE3-0 is an output
  GPIO_PORTE_AFSEL_R &= ~0x0F;      // regular port function
  GPIO_PORTE_AMSEL_R &= ~0x0F;      // disable analog on PE3-0
  GPIO_PORTE_PCTL_R &= ~0x0000FFFF; // PCTL GPIO on PE3-0 
  GPIO_PORTE_DEN_R |= 0x0F;         // PE3-0 enabled as a digital port
  while(1){
    LEDS2 = 10; // 1010, LED is 0101
    LEDS2 = 9;  // 1001, LED is 0110
    LEDS2 = 5;  // 0101, LED is 1010
    LEDS2 = 6;  // 0110, LED is 1001
  }
}

