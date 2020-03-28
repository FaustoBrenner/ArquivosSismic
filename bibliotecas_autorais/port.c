#include <stdint.h>
#include <msp430.h>

void debounce(uint16_t dt){
    volatile uint16_t i = dt;
    while (i--);
    return;
}

void ledVermelho(){
    P1OUT &= ~(BIT0);
    P1DIR |= BIT0;
    return;
}

void ledVerde(){
    P6OUT &= ~(BIT6);
    P6DIR |= BIT6;
    return;
}


void botaoS1(){
    P4DIR &= ~(BIT1);
    P4REN |= BIT1;
    P4OUT |= BIT1;
    return;
}

void botaoS2(){
    P2DIR &= ~(BIT3);
    P2REN |= BIT3;
    P2OUT |= BIT3;
    return;
}

int contadorBinario()
{
    botaoS1();                      //P4.1
    botaoS2();                      //P2.3
    ledVerde();                     //P6.6
    ledVermelho();                  //P1.0

    while(1)
    {
        debounce(10000);
        while ((P4IN&BIT1) && (P2IN&BIT3));

        if (!(P4IN&BIT1))
        {
            if (P6OUT&BIT6)         //Se o LSbyte estiver setado
            {
                P6OUT ^= BIT6;      //apenas subtraimos o LSbyte, sem mudancas no MSbyte
            }
            else                    //Se o LSbyte não estive setado
            {
                P1OUT ^= BIT0;      //teremos que alternar o MSbyte e o LSbyte
                P6OUT ^= BIT6;
            }
        }

        if (!(P2IN&BIT3))
        {
            if (P6OUT&BIT6)          //Se o LSbyte estiver setado
            {
             P1OUT ^= BIT0;          //teremos que alternar o MSbyte e o LSbyte
             P6OUT ^= BIT6;
             }
             else                    //Se o LSbyte não estive setado
             {
              P6OUT ^= BIT6;         //apenas adicionamos ao LSbyte, sem mudancas no MSbyte
              }
        }

        debounce(10000);

        while(!(P4IN&BIT1) || !(P2IN&BIT3));
    }

}

int alternaLED()
{
    botaoS1();                      //P4.1
    botaoS2();                      //P2.3
    ledVerde();                     //P6.6
    ledVermelho();                  //P1.0

    while(1)
    {
        debounce(10000);            //evitar efeito de rebote

        while ((P4IN&BIT1) && (P2IN&BIT3));

        debounce(10000);            //evitar efeito de rebote


        if (!(P4IN&BIT1) && !(P2IN&BIT3))
        {
            P1OUT &= ~(BIT0);       //Se ambos estiverem pressionados, desligo os dois leds
            P6OUT &= ~(BIT6);
        }
        else
        {
            if(!(P4IN&BIT1))
            {
                P6OUT ^= (BIT6);    //Se S1 for pressionado, altero o estado do led verde
            }
            else
            {
                P1OUT ^= (BIT0);    //Se S2 for pressionado, altero o estado do led vermelho
            }
        }

        debounce (10000);           //evitar efeito de rebote

        while (!(P4IN&BIT1) || !(P2IN&BIT3));
    }
    return 0;
}

int frequencimetro (){
        //configurar pino P1.6 para ser a entrada da captura e  P1.1 para ser a saida do ACLK
        P1DIR &= ~BIT6;
        P1DIR |= BIT1;
        P1SEL0 &= ~(BIT6 | BIT0);
        P1SEL1 |= (BIT6 | BIT1);

        //CONFIGURANDO O TIMER DE CAPTURA
        TB0CTL = TBSSEL__SMCLK | MC__CONTINUOUS;
        TBCCTL1 = CAP | CCIS__CCIA |CM__RISING;

        uint8_t edgeCnt;
        uint16_t freq;


        while (1){
            edgeCnt = 16;
            TB0CTL |= TBCLR;
            while(edgeCnt--){
                TB0CCTL1 &= ~CCIFG;
                while(!(TB0CCTL1 & CCIFG));
            }
            freq = 1048578/(TB0CCR1 >> 4);
        }
        return 0;
}

void setPWM_ACLK (const uint16_t T, const uint16_t D){
    TB0CTL = MC__UP | TBSSEL__ACLK | TBCLR;
    TB0CCR0 = T;
    TB0CCR1 = D;
}

void setPWM_SMCLK (const uint16_t T, const uint16_t D){
    TB0CTL = MC__UP | TBSSEL__SMCLK | TBCLR;
    TB0CCR0 = T;
    TB0CCR1 = D;
}

