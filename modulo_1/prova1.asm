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
main:		mov		#code,R4
			mov		#msg,R5
			call	#Scrambler
			jmp		$

;-------------------------------------------------------------------------------
;Subrotina Scrambler
;Input:
;		R5 - ponteiro apontando o início da mensagem
;		R4 - ponteiro apontando o início do código de embaralhamento
;-------------------------------------------------------------------------------

Scrambler:	push	R6			;guarda o valor do usuario de R6
			push	R7			;guarda o valor de R7

			mov		R5,R6		;R6 será nosso ponteiro auxiliar

guardar4:	add		#6,R4		;desloca o ponteiro R4 para o último elemento do código
			mov		#4,R7		;R7 será nosso contador
			push.b	@R5+		;guarda os 4 próximos elementos da string na pilha
			push.b	@R5+
			push.b	@R5+
			push.b	@R5+
			mov		R6,R5		;aponta R5 para seu início

findPos:	add		@R4,R6		;desloca o ponteiro auxiliar para a posição indicada no codigo
			pop.b	0(R6)		;aloca o último elemento da pilha na posição

			mov		R5,R6		;reinicia o vetor R6 na posição 0

			decd	R4			;R4 passa a apontar para o proximo elemento do código (estamos percorrendo de trás pra frente)
			dec		R7			;contador de 4 em 4
			cmp		#1,R7		;quando o contador atingir 0, passamos aos próximos 4 elementos
			jhs		findPos

			incd	R4
			add		#4,R5		;incrementa os ponteiros para para a próxima posição
			mov		R5,R6		;

			tst.b	0(R5)		;verifica se o próximo elemento é o final da string
			jeq		end
			jmp		guardar4	;se não for o final, passamos aos próximos 4 elementos da string

end:		pop		R7
			pop		R6
			ret

;---------------------------------------------------------------------------------------------
			.data
code:		.word	3,0,2,1
msg:		.byte	"suco de laranja.",0
;embaralhada:	uocsd e aarlj.an,0

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
            
