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
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
LAB3		mov.w	#14, R4		;set up the initial a value in R4
			clr.w	R5
			clr.w	R6
			clr.w	R7
			clr.w	R8

XCALC		mov.w	R4, R5			;copy the value of a into R5
			call	#ABSOL			;compute absolute value of a in R5

SUMMATION	mov.w	R5, R6						;upper limit of summation with a in R6
			call	#FACTORIAL					;get and save factorial value of R6 in R6
			add.w	R6, R6						;finish XCALC by doubling the value and adding it to the accumulator in R8
			add.w	R6, R8						;repeat the loop with a-1, a-2, (in R5), til lower value (a = 0)
			dec.w	R5							;save XCALC results in R5
			cmp.w	#0, R5
			jge		SUMMATION
			mov.w	R8, R5

FCALC		mov.w	R5, R7			;start F calculation
			add.w	#50, R5			;finish FCALC by adding the constant 50 and dividing by 4
			rrc		R5				;hint: shift twice to the right with carry = 0**
			rrc		R5

MAINLOOP	jmp		MAINLOOP		;loop in place (infinite loop)

;SUBROUTINES

;ABSOL subroutine tests the value in R5, if negative, the inverse positive is stored in R5
ABSOL:
			tst.w   R5
			jge     ABSOLDONE
			inv.w   R5
			inc.w	R5

ABSOLDONE	ret

;FACTIORAL subroutine calculates the factorial of R5 and stores the result in R6
;the initial value of R5 is pushed to the stack and then popped from the stack at the end
FACTORIAL:
			push	R5
			clr		R6			;clear R6
			cmp.w	#0, R5		;check if n=0
			jne		NEXT		;n<> 0 keep checking
			add.w	#1, R6		;n = 0 set 0! = 1
			jmp		DONE		;jump to DONE

NEXT		cmp.w	#1, R5
			jeq		ONE
			cmp.w	#1, R5		;
			jge		NEXT1		;n>0 go to NEXT1 to complete n! computation
			mov.w	#0, R6		;n<0 n! = 0
			jmp		DONE		;jump to done

ONE			mov.w	#1, R6
			jmp		DONE

NEXT1		mov.w	R5, R6		;R6 saves the result

FAC			dec.w	R5			;n-1
			cmp.w	#1, R5		;n-1=1?
			jeq		DONE		;yes, jump to done
			call	#MULTIPLICATION
								;no, compute R6 = n*(n-1)
								;initially R6 = n, R5 = n-1
								;later R6 = n*(n-1)(n-2)..(n-m), R5 = (n-m-1)
			jmp		FAC

DONE		pop		R5
			ret

;multiplication subroutine, multiplies R5 * R6 and stores the value in R6
MULTIPLICATION:		;multiplication subroutine
			clr.w	R7
			cmp.w	#0, R6
			jeq		ZERO

MP			add.w	R5, R7
			dec.w	R6
			cmp.w	#0, R6
			jne 	MP

ZERO		mov.w	R7,R6
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
            
