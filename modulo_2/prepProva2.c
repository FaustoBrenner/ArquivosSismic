#include <msp430.h> 
#include <stdint.h>
#include <lib/port.h>

#pragma vector = TIMER0_B1_VECTOR;
__interrupt void TB0_CCRn_ISR()
{
    switch (TB0IV){
        case 0x02:          //CCR1
            P6OUT &= ~BIT6;     // Apaga o LED verde
            break;
        case 0x0E:          // TB0IFG (Overflow)
            P6OUT |= BIT6;      // Acende o LED verde
            break;
    }
}

#pragma vector = TIMER1_B1_VECTOR;
__interrupt void TB1_CCRn_ISR()
{
    switch(TB1IV){
    case 0x02:      //CCR1
        P1OUT &= ~BIT0;     //Apaga o led vermelho
        break;
    case 0x0E:      //TB1IFG (Overflow)
        P1OUT |= BIT0;      //Acende o led vermelho
        break;
    }
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~(LOCKLPM5);

	ledVerde();                 // P6.6 - led verde - OUT
	ledVermelho();              // P1.0 - led vermelho - OUT
	botaoS1();                  // P4.1 - botao S1 - IN
	botaoS2();                  // P2.3 - botao S2 - IN

	const uint16_t T1 = 16383;         // T = 0,5 segundo (ACLK)
	const uint16_t D1 = 8191;          // D = 50% T = 0,25 segundo

	const uint16_t T2 = 20970;         // T = 1/50 segundo (SMCLK)
	const uint16_t D2 = 10484;         // D = 50% T = 1/100 segundo

	//configuração do Timer B0 para o PWM do LED verde
	setPWM_ACLK(T1, D1);
	TB0CTL |= TBIE;
    TB0CCTL1 = CCIE;

    //configuração do Timer B1 para o PWM do LED vermelho
	TB1CTL = MC__UP | TBSSEL__SMCLK | TBCLR | TBIE;
	TB1CCTL1 = CCIE;
	TB1CCR0 = T2;
	TB1CCR1 = D2;

	__enable_interrupt();       //GIE = 1

	while (1){
	    debounce(10000);            //evitar efeito de rebote

	    while ((P4IN&BIT1) && (P2IN&BIT3));
	    if (!(P4IN&BIT1)){
	        TB1CCR1 += TB1CCR0/10;
	    }
	    if (!(P2IN&BIT3)){
	        TB1CCR1 -= TB1CCR0/10;
	    }
	}

	return 0;
}
