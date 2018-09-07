;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
ARY1		.set	0x0200					;Memory Allocation	ARY1
ARY1S		.set	0x0210					;Memory Allocation	ARY1S
ARY2		.set	0x0220					;Memory Allocation	ARY2
ARY2S		.set	0x0230					;Memory Allocation	ARY2S

RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
			clr	R4					;Clear Register
			clr	R5					;Clear Register
			clr	R6					;Clear Register

SORT1		mov.w	#ARY1,	R4		;Intialize R4 to point to ARY1  in the memory
			mov.w	#ARY1S,	R6		;Intialize R6 to point to ARY1S in the memory where the sorted ARY1 will be stored
			call	#ArraySetup1	;Create elements are store them in ARY1
			call 	#COPY			;Copy the elements from the ARY1 space to ARY1S space
			call	#SORT			;Calling Subroutine Sort with parameter passing in R4 abd R6

SORT2		mov.w	#ARY2,	R4		;Intialize R4 to poin to ARY2  in the memory
			mov.w	#ARY2S,	R6		;Intialize R6 to poin to ARY2S in the memory where the sorted ARY2 will be stored
			call	#ArraySetup2	;Creat elements are store them in ARY2
			call 	#COPY			;Copy the elements from the ARY2 space to ARY1S space
			call	#SORT			;Calling Subroutine Sort with parameter passing in R4 abd R6

Mainloop	jmp	Mainloop 			;loop in place for ever

;Array element intitialization Subroutine
ArraySetup1	mov.b	#10,	0(R4)	;Define the number of elements in the array
			mov.b	#29, 	1(R4)	;store an element
			mov.b	#16, 	2(R4)	;store an element
			mov.b	#-55, 	3(R4)	;store an element
			mov.b	#90, 	4(R4)	;store an element
			mov.b	#17, 	5(R4)	;store an element
			mov.b	#63, 	6(R4)	;store an element
			mov.b	#59,	7(R4)	;store an element
			mov.b	#-35, 	8(R4)	;store an element
			mov.b	#27,	9(R4)	;store an element
			mov.b	#55, 	10(R4)	;store an element
			ret

;Array element intitialization Subroutine
ArraySetup2	mov.b	#10, 	0(R4)	;Define the number of elements in the array
			mov.b	#43, 	1(R4)	;store an element
			mov.b	#84, 	2(R4)	;store an element
			mov.b	#29, 	3(R4)	;store an element
			mov.b	#-59, 	4(R4)	;store an element
			mov.b	#-51, 	5(R4)	;store an element
			mov.b	#77, 	6(R4)	;store an element
			mov.b	#79,	7(R4)	;store an element
			mov.b	#69, 	8(R4)	;store an element
			mov.b	#77, 	9(R4)	;store an element
			mov.b	#64, 	10(R4)	;store an element
			ret

;Copy original Array to allocated Array-Sorted space
COPY		mov.b	0(R4), R10		;save n (number of elements) in R10
			inc.b	R10				;increment by 1 to account for the byte n to be copied as well
			mov.w	R4, R5			;copy R4 to R5 so we keep R4 unchanged for later use
			mov.w	R6, R7			;copy R6 to R7 so we keep R6 unchanged for later use
LP			mov.b	@R5+, 0(R7)		;copy elements using R5/R7 as pointers
			inc.w 	R7
			dec		R10
			jnz	LP
			ret

SORT		mov.b	0(R6), R8		;saves the n number of elements into R10
			dec.b	R8				;decrement, because there will be n-1 comparisons
			mov.w	R6, R7			;copy R6 to R7, to keep R6 unchanged and pointing to the top of array for each scan
			mov.b	R8, R9			;copy n-1 to R9 as a loop counter
SCANLOOP	mov.b	@R7+, &0200h
			mov.b	@R7+, &0202h
			cmp.b	&0200h, &0202h
			jge		Greater
Greater		mov.b 	&0200h,

			ret
;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
