#include <msp430.h> 
#include <stdint.h>
#include <lib/port.h>
#include <lib/visto2.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer'
    PM5CTL0 &= ~(LOCKLPM5);

    //CONFIGURAR PINOS DE SAIDA
    P6DIR |= (BIT3 | BIT2 | BIT1 | BIT0);        //P6.3 = S3   P6.2 = S2   P6.1 = S1   P6.0 = S0
    P6OUT |= (BIT0);                             //Configurar a frequ�ncia em 20%
    P6OUT &= ~BIT1;
    ledVerde();                                  //P6.6
    ledVermelho();                               //P1.0

    //CONFIGURAR PINOS DE ENTRADA
    P1DIR &= ~BIT6;                              //P1.6 (TB0.1) = OUT
    P1SEL0 &= ~BIT6;
    P1SEL1 |= BIT6;

    //configurar timers
    TB0CTL = TBSSEL__SMCLK | MC__CONTINUOUS;     //seleciona o SMCLK em modo continuo no TB0
    TBCCTL1 = CAP | CCIS__CCIA | CM__RISING;     //configura CCR1 para o modo de captura

    //programa para calcular a frequencia e identificar a maior
    uint16_t Tb, Tg, Tr;

    while (1){
        P6OUT &= ~(BIT3 | BIT2);                //S3 S2 = 0 0 (VERMELHO)
        Tr = getT();

        P6OUT |= BIT3;                          //S3 S2 = 1 0 (AZUL)
        Tb = getT();

        P6OUT |= BIT2;                          //S3 S2 = 1 1 (VERDE)
        Tg = getT();

        Tr++;

        detectaCor(Tr, Tg, Tb);                 //Compara os per�odos para encontrar a cor dominante
    }

	return 0;
}
