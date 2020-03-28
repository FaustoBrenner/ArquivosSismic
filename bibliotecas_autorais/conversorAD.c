#include <stdint.h>
#include <msp430.h>

uint16_t acdRead(uint8_t pin)
{
    ADCCTL0 &= ~ADCENC;
    ADCCTL0 = ADCON | ADCSHT_2;
    ADCCTL1 = ADCSHP;
    ADCCTL2 = ADCRES_2;
    ADCMCTL0 = pin;

    P1SEL0 |= (1<<pin);
    P1SEL1 |= (1<<pin);

    ADCCTL0 |= ADCENC;

    ADCCTL0 &= ~ADCSC;
    ADCCTL0 |= ADCSC;

    while(!(ADCIFG & ADCIFG0));

    return ADCMEM0;
}

void adcConfig()
{
    ADCCTL0 &= ~ADCENC;

    ADCCTL0 = ADCON | ADCSHT_3;        // LIGA O CONVERSOR E ESCOLHE A QUANTIDADE DE BITS NECESSÁRIAS
    ADCCTL1 = ADCCONSEQ_1 | ADCSHP;    // TIMER INTERNO E SINGLE SEQUENCE OF CHANNELS
    ADCCTL2 = ADCRES_2;                // 12 BITS
    ADCMCTL0 = ADCINCH_1;              // CANAL INICIAL PINO P1.1

    P1SEL0 |= BIT1 | BIT0;             // CONFIGURAÇÃO DOS PINOS DE ENTRADA
    P1SEL1 |= BIT1 | BIT0;

    ADCCTL0 |= ADCENC;                 // ENABLE CONVERSION
    ADCIE = ADCIE0;                    // ATIVA INTERRUPÇÕES
}

void adcConfigSingleCh()
{
    ADCCTL0 &= ~ADCENC;
    ADCCTL0 = ADCON;                     //
    ADCCTL1 = ADCCONSEQ_2 | ADCSHS_2;    // TIMER EXTERNO E REPEAT SINGLE OF CHANNELS
    ADCCTL2 = ADCRES_2;                  // 12 BITS
    ADCMCTL0 = ADCINCH_1;

    P1SEL0 |= BIT1;                      // CONFIGURAÇÃO DO PINO DE ENTRADA
    P1SEL1 |= BIT1;

    ADCCTL0 |= ADCENC;                   // ENABLE CONVERSION
    ADCIE = ADCIE0;
}


void timerConfigB1()
{
    TB1CTL = MC_1 | TBCLR | TBSSEL__ACLK;       // CONFIGURACAO DO TIMER B1
    TB1CCR0 = 2047;                             // A 16 HZ
    TB1CCR1 = 1023;                             // Duty 50%
    TB1CCTL1 = OUTMOD_7;
}

void timerConfigB0()
{
    TB0CTL = MC_1 | TBCLR | TBSSEL__ACLK;       // CONFIGURACAO DO TIMER B1
    TB0CCR0 = 2047;                             // A 16 HZ
    TB0CCTL0 |= CCIE;
}

void getTensoes(uint16_t *canal1, uint16_t *canal2, uint16_t *inputData)
{
    uint8_t count = 8;
    uint16_t Soma1 = 0;
    uint16_t Soma2 = 0;
    while(count--){                             // Soma todos os 8 valores de cada canal
        Soma1 += *inputData++;
        Soma2 += *inputData++;
    }
    *canal1 = Soma1>>3;                         // Divide a soma por 8, obtendo a média
    *canal2 = Soma2>>3;
}

uint16_t media(uint16_t *vetorPtr)
{
    uint8_t count = 8;
    uint16_t media = 0;
    while(count--){
        media += *vetorPtr++;
    }
    media /= 8;
    return media;
}
