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
NUM			.equ	3999		;numero a ser convertido
;
			mov		#NUM,R5		;R5 = numero a ser convertido
			mov		#RESP,R6	;R6 = ponteiro para escrever a resposta
			call 	#ALG_ROM	;chamar subrotina
			jmp		$			;travar execução
			nop
;-------------------------------------------------------------------------------
; Subrotina para converter um número decimal em uma string de algarismos romanos
;	entrada:	R5 = numero a ser convertido
;	saida:		R6 = ponteiro do inicio da string de algarismos romanos
;-------------------------------------------------------------------------------
ALG_ROM:

writeM:		cmp		#1000,R5	;compara se R5>1000
			jn		specialC	;caso o numero seja menor que 1000, pula para  o teste de casos especiais de centenas
			sub		#1000,R5	;se o numero for maior, subtrai 1000 de R5
			mov.b	#'M',0(R6)	;adiciona M ao algarismo romano
			inc		R6			;incrementa a posição de R6
			jmp		writeM		;reinicia o loop até que o número seja menor que mil

specialC:
			sub		#900,R5		;verifica o caso especial CM
			jc		writeCM		;caso o numero seja maior que 900, nao é necessario testar outros casos

			add		#900,R5		;reestrutura o valor de R5
			sub		#500,R5		;verifica o caso especial D
			jc		writeD		;se o numero for maior que 500, tambem nao é necessario testar outro caso

			add		#500,R5		;reestrutura o valor de R5
			sub		#400,R5		;verifica o caso especial CD
			jc		writeCD

			add		#400,R5		;reestrutura o valor de R5
			jmp 	writeC		;pula para as centenas

writeCM:	mov.b	#'C',0(R6)	;adiciona C ao algarismo romano
			inc		R6			;incrementa a posição de R6
			mov.b	#'M',0(R6)	;adiciona M ao algarismo romano
			inc		R6			;incrementa a posição de R6
			jmp		specialX

writeCD:	mov.b	#'C',0(R6)	;adiciona C ao algarismo romano
			inc		R6			;incrementa a posição de R6
			mov.b	#'D',0(R6)	;adiciona D ao algarismo romano
			inc		R6			;incrementa a posição de R6
			jmp		specialX

writeD:		mov.b	#'D',0(R6)	;adiciona D ao algarismo romano
			inc		R6			;incrementa a posição de R6

writeC:		cmp		#100,R5		;compara se 100 < R5
			jn		specialX	;caso o numero seja menor que 100, pula para o teste de casos especiais de dezenas
			sub		#100,R5		;se o numero for maior, subtrai 100 de R5
			mov.b	#'C',0(R6)	;adiciona C ao algarismo romano
			inc		R6			;incrementa a posição de R6
			jmp		writeC		;reinicia o loop até que o número seja menor que cem

specialX:
			sub		#90,R5		;verifica o caso especial XC
			jc		writeXC		;caso o numero seja maior que 90, nao é necessario testar outros casos

			add		#90,R5		;reestrutura o valor de R5
			sub		#50,R5		;verifica o caso especial L
			jc		writeL		;se o numero for maior que 50, tambem nao é necessario testar outro caso

			add		#50,R5		;reestrutura o valor de R5
			sub		#40,R5		;verifica o caso especial XL
			jc		writeXL

			add		#40,R5		;reestrutura o valor de R5
			jmp 	writeX		;pula para as centenas

writeXC:	mov.b	#'X',0(R6)	;adiciona X ao algarismo romano
			inc		R6			;incrementa a posição de R6
			mov.b	#'C',0(R6)	;adiciona C ao algarismo romano
			inc		R6			;incrementa a posição de R6
			jmp		specialI

writeXL:	mov.b	#'X',0(R6)	;adiciona X ao algarismo romano
			inc		R6			;incrementa a posição de R6
			mov.b	#'L',0(R6)	;adiciona L ao algarismo romano
			inc		R6			;incrementa a posição de R6
			jmp		specialI

writeL:		mov.b	#'L',0(R6)	;adiciona L ao algarismo romano
			inc		R6			;incrementa a posição de R6

writeX:		cmp		#10,R5		;subtrai 10 de R5
			jn		specialI	;caso o numero seja menor que 10, pula para o teste de casos especiais de unidades
			sub		#10,R5		;se o numero for maior, subtrai 10 de R5
			mov.b	#'X',0(R6)	;adiciona X ao algarismo romano
			inc		R6			;incrementa a posição de R6
			jmp		writeX		;reinicia o loop até que o número seja menor que cem

specialI:
			sub		#9,R5		;verifica o caso especial IX
			jz		writeIX		;caso o numero seja igual a 9, pula para sua escrita em algrom

			add		#9,R5		;reestrutura o valor de R5
			sub		#5,R5		;verifica o caso especial V
			jz		writeV		;se o numero for maior que 5, pula para sua escrita

			add		#5,R5		;reestrutura o valor de R5
			sub		#4,R5		;verifica o caso especial IV
			jz		writeIV

			add		#4,R5		;reestrutura o valor de R5
			jmp 	writeI		;pula para as unidades

writeIX:	mov.b	#'I',0(R6)	;adiciona I ao algarismo romano
			inc		R6			;incrementa a posição de R6
			mov.b	#'X',0(R6)	;adiciona X ao algarismo romano
			inc		R6			;incrementa a posição de R6
			jmp		end

writeIV:	mov.b	#'I',0(R6)	;adiciona I ao algarismo romano
			inc		R6			;incrementa a posição de R6
			mov.b	#'V',0(R6)	;adiciona V ao algarismo romano
			inc		R6			;incrementa a posição de R6
			jmp		end

writeV:		mov.b	#'V',0(R6)	;adiciona V ao algarismo romano
			inc		R6			;incrementa a posição de R6

writeI:		sub		#1,R5		;subtrai 1 de R5
			jn		end			;pula para o fim da subrotina quando a ultima unidade for subtraida
			mov.b	#'I',0(R6)	;adiciona I ao algarismo romano
			inc		R6			;incrementa a posição de R6
			jmp		writeI		;reinicia o loop até a ultima unidade

end:		mov.b	#0,0(R6)	;adiciona o valor #0 ao final da string para indicar seu fim
			mov		#0x2000,R6	;reinicia o ponteiro R6 para o inicio da string
			ret

			.data
			;local para armazenar a resposta
RESP:		.byte	"RRRRRRRRRRRRRRRRRR",0
                                            

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
            
