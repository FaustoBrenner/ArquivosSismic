#include <stdint.h>
#include <msp430.h>

//----------------------------- INTERRUPCOES -------------------------------

struct{
    uint8_t status;
    uint8_t data;
} UCB0;

void i2cInitInt (uint8_t isMaster, uint8_t myAddr){
    UCB0CTLW0 = UCMODE_3 | UCSWRST;             // Modo: I2C
    UCB0CTLW1 = UCASTP_2;                       // STOP automatico
    UCB0TBCNT = 1;                              // Para recepção de 1 byte

    if (isMaster)                               // Se for mestre
    {
        UCB0CTLW0 |= UCSSEL__SMCLK | UCMST;     // Configura como mestre usando o SMCLK
        UCB0BRW = 30;                           // SCL @ 33kHz
    }
    else                                        // Se for escravo
    {
        UCB0I2COA0 = UCOAEN | myAddr;           // Configura o endereco
    }

    // CONFIG DAS PORTAS
    P1SEL0 |= BIT2|BIT3;                        // P1.2 - SDA
    P1SEL1 &= ~(BIT2|BIT3);                     // P1.3 - SCL

    P1DIR &= ~(BIT2 | BIT3);
    P1REN |= BIT2|BIT3;                         // habilita resistores de pullup
    P1OUT |= BIT2|BIT3;                         // na SCL e na SDA

    UCB0CTLW0 &= ~UCSWRST;                      // Zera o bit de Reset para liberar comunicacao

                                                // Interrupcoes:
    UCB0IE = UCRXIE |                           // RX
             UCTXIE |                           // TX
             UCSTPIE |                          // Stop
             UCNACKIE;                          // NACK
}

uint8_t i2cTstInt (uint8_t addr){
    UCB0.status = 0;

    UCB0I2CSA = addr;                       // Configura end. do escravo e pede
    UCB0CTLW0 |= UCTR + UCTXSTT + UCTXSTP;  // star e stop simultaneos

    __low_power_mode_0();                   // Aguarda o final da comunicacao "while (!UCB0.finished)"
    return UCB0.status;                     // para retornar 0:ACK e 1:NACK
}

uint8_t i2cPutInt (uint8_t addr, uint8_t data){
    UCB0.status = 0;

    UCB0I2CSA = addr;                       // Configura end. do escravo e
    UCB0.data = data;                       // Copia o byte para a interface

    UCB0CTLW0 |= UCTR + UCTXSTT;            // Pede o inicio da comunicacao

    __low_power_mode_0();                   // Aguarda o final da comunicacao
    return UCB0.status;
}

uint8_t i2cGetInt (uint8_t addr, uint8_t *data){
    UCB0.status = 0;

    UCB0I2CSA = addr;                       // Configra end. do escravo

    UCB0CTLW0 &= ~UCTR;                     // Pede start como receptor
    UCB0CTLW0 |= UCTXSTT;

    __low_power_mode_0();                   // Aguarda o fim da comunicacao
    *data = UCB0.data;                      // Grava o byte no ponteiro do usuario

    return UCB0.status;                     // Retorna 0:ACK e 1:NACK
}

#pragma vector = USCI_B0_VECTOR
__interrupt void I2C_ISR(){
    switch (UCB0IV)
    {
    case UCIV__UCNACKIFG:               // Se recebeu NACK,
        UCB0CTLW0 |= UCTXSTP;           // Pede Stop
        UCB0.status = 1;                // e avisa o usuario do nack
        break;

    case UCIV__UCSTPIFG:                // Se enviou o stop,
        __low_power_mode_off_on_exit(); // Volta pro progama principal
        break;                          // Saindo do modo de baixo consumo

    case UCIV__UCRXIFG0:                // Se recebeu alguma coisa no buffer,
        UCB0.data = UCB0RXBUF;          // de recepcao, escreve no ponteiro do usuï¿½rio
        break;

    case UCIV__UCTXIFG0:                // Se for transmitir algo, pegue
        UCB0TXBUF = UCB0.data;          // o byte indicado pelo ponteiro do usuario
        break;

    default:
        break;
    }
}











// -------------------------------- POOLING ----------------------------------


void i2cInitB0 (uint8_t isMaster, uint8_t myAddr){
    UCB0CTLW0 = UCSWRST | UCSSEL__SMCLK | UCMODE_3;         // RESET = 1
    if(isMaster){
        UCB0CTLW0 |= UCMST;      // MODO MESTRE I2C
    }else{
        UCB0CTLW0 &= ~UCMST;      // MODO ESCRAVO I2C
        UCB0I2COA0 = UCOAEN | myAddr;
    }

    UCB0BRW = 30;               // 1MHz/10 = 100 kHz

    // CONFIG DAS PORTAS
    P1SEL0 |= BIT2|BIT3;        // P1.2 - SDA
    P1SEL1 &= ~(BIT2|BIT3);     // P1.3 - SCL

    P1DIR &= ~(BIT2 | BIT3);
    P1REN |= BIT2|BIT3;         // habilita resistores de pullup
    P1OUT |= BIT2|BIT3;         // na SCL e na SDA

    UCB0CTLW0 &= ~(UCSWRST);     // LIBERA O RESET
}

void i2cInitB1 (uint8_t isMaster, uint8_t myAddr){
    UCB1CTLW0 = UCSWRST | UCSSEL__SMCLK | UCMODE_3;         // RESET = 1
        if(isMaster){
            UCB1CTLW0 |= UCMST;      // MODO MESTRE I2C
        }else{
            UCB1CTLW0 &= ~UCMST;      // MODO ESCRAVO I2C
            UCB1I2COA0 = UCOAEN | myAddr;
        }
    UCB1BRW = 30;               // 1MHz/10 = 100 kHz

    // CONFIG DAS PORTAS
    P4SEL0 |= (BIT6|BIT7);        // P4.6 - SDA
    P4SEL1 &= ~(BIT6|BIT7);       // P4.7 - SCL

    P4DIR &= ~(BIT6 | BIT7);
    P4REN |= BIT6|BIT7;         // habilita resistores de pullup
    P4OUT |= BIT6|BIT7;         // na SCL e na SDA

    UCB1CTLW0 &= ~(UCSWRST);     // LIBERA O RESET
}

uint8_t i2cTst (uint8_t addr){
    UCB0IFG = 0;

    UCB0I2CSA = addr;               // configura endereco do escravo
    UCB0CTLW0 |= UCTR + UCTXSTT;    // pede start

    while (!(UCB0IFG & UCTXIFG));
    UCB0TXBUF = 0x00;

    while (!(UCB0IFG & UCNACKIFG)&&!(UCB0IFG & UCTXIFG));   // espera ou NACK ou TXIFG

    UCB0CTLW0 |= UCTXSTP;               // pede o STOP e
    while (UCB0CTLW0 & UCTXSTP);        // aguarda ele ser enviado

    return (UCB0IFG & UCNACKIFG);       //retorna ACK ou NACK
}

uint8_t i2cSend (uint8_t dstAddr, uint8_t *dataPtr, uint8_t nBytes){
    UCB0IFG = 0;

    UCB0I2CSA = dstAddr;            // Enderaï¿½o do escravo
    UCB0CTLW0 |= UCTR +             // Mestre transmissor
            UCTXSTT;                // Gerar Start

    while (!(UCB0IFG & UCTXIFG));   // Espera a flag de TX virar 1 para escrever em TXBUF
    UCB0TXBUF = *dataPtr++;
    nBytes --;

    while (!(UCB0IFG & UCNACKIFG) && !(UCB0IFG & UCTXIFG));

    if (UCB0IFG & UCNACKIFG)
    {                               //NACK?
        UCB0CTLW0 |= UCTXSTP;
        while(UCB0CTLW0 & UCTXSTP); // escravo gerou NACK
        return 1;
    }

    while(nBytes--){
        while( !(UCB0IFG & UCTXIFG) );
        UCB0TXBUF = *dataPtr++;
    }

    UCB0CTLW0 |= UCTXSTP;            // Gerar STOP
    while (UCB0CTLW0 & UCTXSTP);   // Espera o STOP ser enviado
    return 0;
}

uint8_t i2cGet(uint8_t addr, uint8_t *dataPtr, uint8_t count){
    UCB0IFG = 0;                        // Zera a flag

    UCB0I2CSA = addr;                   // Configura o endereco do escravo
    UCB0CTLW0 &= ~UCTR;                 // Mestre receptor
    UCB0CTLW0 |= UCTXSTT;               // Gera Start

    while (UCB0IFG & UCTXSTT);          // Aguarda Start

    if (UCB0IFG & UCNACKIFG){           // Se ocorreu um NACK
        UCB0CTLW0 |= UCTXSTP;           // Pede o Stop
        while (UCB0CTLW0 & UCTXSTP);    // Aguarda Stop
        return 1;                       // Retorna Erro
    }

    while (--count){                    // Recebe os dados
        while(!(UCB0IFG & UCRXIFG));    // Aguarda transmissao de n-1 bytes
        *dataPtr++ = UCB0RXBUF;         // Guarda valor no vetor de resultados
    }
    UCB0CTLW0 |= UCTXSTP;               // Pede STOP antes do ultimo byte

    while (!(UCB0IFG & UCRXIFG));       // Aguarda transmissao do ultimo byte
    *dataPtr++ = UCB0RXBUF;             // Guarda valor no vetor de resultados

    while (UCB0CTLW0 & UCTXSTP);        // Verifica o STOP

    return 0;
}

uint8_t i2cSendByte (uint8_t addr, uint8_t byte){
    return i2cSend(addr, &byte, 1);
}

uint8_t i2cGetByte (uint8_t addr){
    uint8_t byte;
    i2cGet(addr, &byte, 1);
    return byte;
}

