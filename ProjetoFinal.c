#include <stdint.h>
#include <msp430.h>
#include <lib/conversorAD.h>
#include <lib/lcdCom.h>
#include <lib/port.h>
#include <lib/libSerial.h>

uint16_t inputData[8];
uint16_t *sensorPtr = inputData;
uint16_t sensor;                            // Tensao medida pelo sensor
uint8_t adcFinished;

#pragma vector = ADC_VECTOR
__interrupt void ADC__ISR()
{
    static uint8_t sensorCntr = 0;
    *(sensorPtr + sensorCntr) = ADCMEM0;    // Salva valor da conversao
    if (sensorCntr++ == 7)
    {
        sensorCntr = 0;                     // Apos 8 conversoes
        adcFinished = 1;                    // finaliza medicao
    }
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void TB0_CCRn_ISR()
{                                           // Unico canal configurado é o CCR0
    ADCCTL0 |= ADCSC;                       // Gera flanco para comecar conversão
    ADCCTL0 &= ~ADCSC;                      // do sensor de gás e fumaça
    ADCCTL0 |= ADCSC;
}

#pragma vector = PORT4_VECTOR
__interrupt void P4__ISR()
{
    switch(P4IV)
    {
    case 0x04:
        __low_power_mode_off_on_exit();     // Caso o botão S1 seja precionado, sai do modo de alerta
        break;
    default:
        break;
    }
}

void alerta()
{
    TB0CCTL0 &= ~CCIE;                      // Interrompe capturas do ADC
    //espSendMail();                        // Envia email pelo ESP

    P2OUT |= BIT1;                          // Acende led vermelho
    P6OUT &= ~BIT6;                         // Apaga led verde

    lcdCLR();
    lcdAlerta(sensor);                      // Disponibiliza mensagem de alerta no lcd

    P4IE |= BIT1;                           // Ativa interrupções pelo botão S1
    __low_power_mode_2();                   // Aguarda o pressionar do botão para voltar ao modo ativo
    P4IE &= ~BIT1;                          // Desativa interrupções pelo botão S1

    TB0CCTL0 |= CCIE;                       // Reinicia capturas do ADC

    P6OUT |= BIT6;                          // Em situação normal, led verde permanece aceso
    P2OUT &= ~BIT1;                         // e o vermelho apagado
}

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;

    // configurar GPIOs de controle
    P2OUT &= ~(BIT1);
    P2DIR |= BIT1;                          // led vermelho -> P1.0 (indica alerta de incêndio)
    ledVerde();                             // led verde    -> P6.6 (indica situação segura)
    botaoS1();                              // botao S1     -> P4.1 (botão para cessar alerta)
    P4IES |= BIT1;                          // interrupção do botão ativada com flanco de descida

    // configurar ADC
    adcConfig();                            // Configura ADC com single channel
    timerConfigB0();                        // Timer a 8Hz
    static const uint16_t limiar = 0x07FF;  // Limiar de tensão para situação de emergência

    // configurar ESP

    // configurar LCD
    i2cInitB0 (1, 0x00);                    // B0 -> mestre
    while(!(lcdQualAdr()));                 // Verifica a comunicacao mestre-escravo e configura o addr da PCF
    lcdInit();                              // Configura o LCD em modo de 4 bits, duas linha,
                                            // Display ON, Cursor ON e tela limpa
    lcdBacklightON();                       // Acende a backlight

    __enable_interrupt();

    while(1)
    {
        // Realiza medidas do sensor
        adcFinished = 0;                    // Reinicia a flag de final de captura
        while(!adcFinished);                // Aguarda o final das 8 capturas

        sensor = media(inputData);          // Calcula a media dos valores medidos

        if(sensor > limiar)                 // Se o valor medido estiver acima do limiar
        {
            alerta();                       // Inicia procedimento de alerta
        }

        lcdCLR();
        lcdNormal(sensor);                  // LCD disponibiliza mensagem de situação normal
    }
}
