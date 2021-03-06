;/*****************************************************************************/
; OSasm.s: low-level OS commands, written in assembly                       */
; Runs on LM4F120/TM4C123
; A very simple real time operating system with minimal features.
; Daniel Valvano
; January 29, 2015
;
; This example accompanies the book
;  "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
;  ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
;
;  Programs 4.4 through 4.12, section 4.2
;
;Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
;    You may use, edit, run or distribute this file
;    as long as the above copyright notice remains
; THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
; OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; For more information about my classes, my research, and my books, see
; http://users.ece.utexas.edu/~valvano/
; */

        AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8

        EXTERN  RunPt            ; currently running thread
		EXTERN	NextThread
        EXPORT  OS_DisableInterrupts
        EXPORT  OS_EnableInterrupts
		EXPORT	OS_Suspend
        EXPORT  StartOS
		EXPORT	PendSV_Handler


OS_DisableInterrupts
        CPSID   I
        BX      LR


OS_EnableInterrupts
        CPSIE   I
        BX      LR

;needs work, push R4-R11 overwrites PC
OS_Suspend
    CPSID   I                  ; 2) Prevent interrupt during switch
    PUSH    {R4-R11}           ; put current tcbs registers on its stack
	PUSH     {R0-R3}            ; same
    PUSH     {R12}				; same
    LDR     R0, =RunPt         ; 4) R0=pointer to RunPt, old thread
    LDR     R1, [R0]           ;    R1 = RunPt
    STR     SP, [R1]           ; save current SP onto its stack
    LDR     R1, [R1,#4]        ; load next pointer from current tcb
    STR     R1, [R0]           ;    RunPt = R1
    LDR     SP, [R1]           ; 7) new thread SP; SP = RunPt->sp;
    POP     {R4-R11}           ; gets register for next tcb's stack
	POP     {R0-R3}            ; same
    POP     {R12}				; same
    POP     {LR}               ; discard LR from initial stack
    POP     {LR}               ; start location
    POP     {R1}               ; discard PSR
    CPSIE   I                  ; 9) tasks run with interrupts enabled
    BX      LR                 ; start bext thread
	
PendSV_Handler                 ; saves R0-R3,R12,LR,PC,PSR
    CPSID   I                  ; prevent interrupt during switch
    PUSH    {R4-R11}           ; save registers R4-R11 onto arriving thread's stack
    LDR     R0, =RunPt         ; load pointer to arriving thread's tcb pointer
    LDR     R1, [R0]           ; load arriving thread's location
    STR     SP, [R1]           ; save SP into arriving thread's TCB
    LDR     R0, =NextThread    ; load pointer to next thread's tcb pointer
    LDR     R1, [R0]           ; load next thread's location
    LDR     SP, [R1]           ; load next thread's stack pointer
	LDR		R2, =RunPt		   ; load pointer to arriving thread's tcb pointer
	STR		R1, [R2]		   ; save arriving thread's location
    POP     {R4-R11}           ; restore registers r4-11
    CPSIE   I                  ; tasks run with interrupts enabled
    BX      LR                 ; restore R0-R3,R12,LR,PC,PSR

StartOS
    LDR     R0, =RunPt         ; loads address of tcb
    LDR     R2, [R0]           ; loads SP from tcb
    LDR     SP, [R2]           ; puts that tcb's SP in SP register
    POP     {R4-R11}           ; load starting values we put in stack
    POP     {R0-R3}            ; same
    POP     {R12}				; same
    POP     {LR}               ; discard LR from initial stack
    POP     {LR}               ; start location
    POP     {R1}               ; discard PSR
    CPSIE   I                  ; Enable interrupts at processor level
    BX      LR                 ; start first thread

    ALIGN
    END
