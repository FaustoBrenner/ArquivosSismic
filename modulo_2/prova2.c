#include <msp430.h> 
#include <stdint.h>
#include <lib/port.h>
uint8_t F1 = 0;

#pragma vector = TIMER0_B1_VECTOR;
__interrupt void TB0_CCRn_ISR()
{
    if (F1){
        switch (TB0IV){
            case 0x02:          // CCR1
                P1OUT &= ~BIT0;     // Apaga o LED vermelho
                break;
            case 0x0E:          // TB0IFG (Overflow)
                P1OUT |= BIT0;      // Acende o LED vermelho
                P6OUT ^= BIT6;
                break;
        }
    }
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~(LOCKLPM5);

    ledVerde();                 // P6.6 - led verde - OUT
    ledVermelho();              // P1.0 - led vermelho - OUT
    botaoS1();                  // P4.1 - botao S1 - IN

    const uint16_t T = 49152;         // T = 1,5 segundo (ACLK)
    const uint16_t D = 24576;        // D = 50% T = 0,75 segundo

    //configura��o do Timer B0
    TB0CTL = MC__UP | TBSSEL__ACLK | TBCLR | TBIE;
    TB0CCR0 = T;
    TB0CCR1 = D;
    TB0CCTL1 = CCIE;

    __enable_interrupt();       //GIE = 1


    while (1){
        debounce(10000);            //evitar efeito de rebote

        F1 = 0;

        while (P4IN&BIT1);
        debounce(10000);

        F1 = 1;

        while (P4IN&BIT1);

    }

    return 0;
}
