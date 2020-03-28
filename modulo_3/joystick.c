#include <stdint.h>
#include <msp430.h>
#include <lib/libSerial.h>
#include <lib/lcdCom.h>
#include <lib/conversorAD.h>

uint16_t Yaxis;
uint16_t Xaxis;
uint16_t vetorY[500];
uint16_t vetorX[500];
uint8_t cont;

void atualizaVetor (uint16_t vetor[500], uint16_t valor)
{
    uint16_t *ptr;
    uint16_t *ptrAnt;
    ptr = vetor + 499;
    ptrAnt = ptr - 1;
    uint16_t temp = 500;

    while(temp--)
    {
        *ptr-- = *ptrAnt--;
    }

    *ptr = valor;
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;           // stop watchdog timer
	PM5CTL0 &= ~(LOCKLPM5);

	// Configuração do conversor AD
	ADCCTL0 &= ~ADCENC;         // Começar configuração
	ADCCTL0 = ADCSHT_3 |        // tcap = 3,5us -> SHT = 17
	          ADCON;            // Liga a conversao
	ADCCTL1 = ADCSHP |          // Seleciona a saida do MODCLK
	          ADCCONSEQ_1;      // Sequence-of-channels
	ADCCTL2 = ADCRES_2;         // Resolução de 12 bits
	ADCMCTL0 = ADCINCH_7 |      // Último canal de entrada
	           ADCSREF_2;       // Vref externa (5V)

	P1SEL0 |= BIT0 | BIT6 | BIT7;   // P1.0 -> Veref+ = 5V
	P1SEL1 |= BIT0 | BIT6 | BIT7;   // P1.6 -> Eixo X   P1.7 -> Eixo Y

	ADCIE = ADCIE0;             // Ativa interrupções do ADC
	ADCCTL0 |= ADCENC;          // Ativa a conversão

	// Configuração do timer
	TB0CTL = MC_1 | TBCLR | TBSSEL__SMCLK;      // Modo de contagem UP do SMCLK
	TB0CCR0 = 10485;                            // Com período de 10ms
	TB0CCTL0 |= CCIE;

	while(1)
	{
	    cont = 0;
	    while (cont < 3);

	    atualizaVetor(vetorY, Yaxis);
	    atualizaVetor(vetorX, Xaxis);
	}
	return 0;
}

#pragma vector = ADC_VECTOR
__interrupt void ADC__ISR()
{
    if (ADCMCTL0 & BIT2)        // Verifica que INCH não é Veref (P1.0)
    {
        if (ADCMCTL0 & BIT0)    // Se for o eixo Y (P1.7)
        {
            Yaxis = ADCMEM0;
        }
        else                    // Se for o eixo X (P1.6)
        {
            Xaxis = ADCMEM0;
        }
        cont ++;
    }
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TB0_CCRn_ISR()
{                                       // Unico canal configurado é o CCR0
    ADCCTL0 |= ADCSC;                   // Gera flanco para comecar conversão
    ADCCTL0 &= ~ADCSC;
    ADCCTL0 |= ADCSC;
}
