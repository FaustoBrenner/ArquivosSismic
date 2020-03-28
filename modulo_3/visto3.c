#include <stdint.h>
#include <msp430.h>
#include <lib/libSerial.h>
#include <lib/lcdCom.h>
#include <lib/conversorAD.h>


uint16_t inputData[16];                 // Dados de entrada do ADC
uint16_t *dataPtr = inputData;          // Ponteiro de dados
uint16_t adcFinished;                   // Flag de sinalização do fim das 8 medidas de cada canal

#pragma vector = ADC_VECTOR
__interrupt void ADC__ISR()
{
    static uint8_t dataCntr = 0;
    *(dataPtr + dataCntr) = ADCMEM0;
    if (dataCntr++ == 15)
    {
        adcFinished = 1;
        dataCntr = 0;
    }
}

#pragma vector=TIMER0_B0_VECTOR
__interrupt void TB0_CCRn_ISR()
{                                       // Unico canal configurado é o CCR0
    ADCCTL0 |= ADCSC;                   // Gera flanco para comecar conversão
    ADCCTL0 &= ~ADCSC;
    ADCCTL0 |= ADCSC;
}


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;           // stop watchdog timer
	PM5CTL0 &= ~(LOCKLPM5);
//-------------------     CONFIGURAÇÃO INICIAL DO LCD     ----------------------------

	i2cInitB0 (1, 0x00);                // B0 -> mestre
	while(!(lcdQualAdr()));             // Verifica a comunicacao mestre-escravo e configura o addr da PCF
	lcdInit();                          // Configura o LCD em modo de 4 bits, duas linha,
	                                    // Display ON, Cursor ON e tela limpa
	lcdBacklightON();                   // Acende a backlight

//-------------------   Variáveis dos valores de tensao em volts e em Hex   ----------
	uint16_t canal1;
	float canal1Volt;

	uint16_t canal2;
	float canal2Volt;


//-------------------   Configuracao inicial do ADC e do Timer a 16Hz  ---------------
	adcConfig();
	timerConfigB0();
	__enable_interrupt();

//-------------------       Loop infinito de atualização do display    ---------------
	while(1)
	{
	    adcFinished = 0;                                        // Reinicia a flag de final de captura
	    while(!adcFinished);                                    // Aguarda o final das 16 capturas

	    getTensoes(&canal1, &canal2, inputData);            // Separa os dados entre os canais 1 e 2
	    canal1Volt = (3.3)*(canal1/0x0FFF);                     // A partir do valor de referência, calcula a tensão medida em volts
	    canal2Volt = (3.3)*(canal2/0x0FFF);

	    lcdCLR();                                               // Reinicia o Display
	    lcdUpdate(canal1Volt, canal1, canal2Volt, canal2);  // Atualiza os valores no Display
	}
	return 0;
}
