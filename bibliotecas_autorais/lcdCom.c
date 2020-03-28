
#include <stdint.h>
#include <msp430.h>
#include <lib/libSerial.h>

#define BL  BIT3    // Backlight
#define EN  BIT2    // Enable
#define RS  BIT0    // Configure / RW
#define RW  BIT1    // Read/not(write)

uint8_t PCFaddr = 0x27;

char lcdQualAdr (){// Descobre o endereï¿½o certo da PCF
    PCFaddr = 0;
    if (!i2cTst(0x3F)){     // Verifica a flag de ACK ou NACK do teste
        PCFaddr = 0x3F;     // se der ACK, retorna endereco certo
    }
    if (!i2cTst(0x27)){
        PCFaddr = 0x27;
    }
    return PCFaddr;        // se der NACK nos dois casos, retorna 0
}

void lcdBacklightON(){
    i2cSendByte (PCFaddr, BL);
    return;
}

void lcdBacklightOFF(){
    i2cSendByte (PCFaddr, 0);
    return;
}

void lcdWriteNibble (uint8_t nibble, uint8_t rs){
                                                          // BL   EN  R/W  RS
    i2cSendByte(PCFaddr, nibble<<4 | BL | rs);            // 1    0    0   rs
    i2cSendByte(PCFaddr, nibble<<4 | BL | EN | rs);       // 1    1    0   rs   pulso no enable
    i2cSendByte(PCFaddr, nibble<<4 | BL | rs);            // 1    0    0   rs
}

void lcdWriteByte (uint8_t byte, uint8_t rs){
    lcdWriteNibble (byte >> 4, rs);             // Envia o nibble mais significativo
    lcdWriteNibble (byte, rs);                  // Envia o nibble menos significativo
}

void lcdInit (){// Rotina de configuraï¿½ï¿½o do LCD
    wait_time(16000);                   // Espera 16ms
    lcdWriteNibble(3, 0);      // Entra em modo 8 bits
    wait_time(5000);                    // Espera 5ms
    lcdWriteNibble(3, 0);
    wait_time(150);                     // Espera 150us
    lcdWriteNibble(3, 0);
    lcdWriteNibble(2, 0);      // Seleciona o modo de 4 bits
    wait_time(150);                     // Espera 150us

    lcdWriteByte(0x28,0);               // Configura: 2 linhas
    wait_time(150);
    lcdWriteByte(0x0C,0);               // Display ON, Cursor ON
    wait_time(150);
    lcdWriteByte(0x06,0);               // Modo de incremento
    wait_time(150);

    lcdWriteByte(0x01,0);               // Limpa a tela
    wait_time (1500);
}

void lcdPrint(char *str){               // Escreve um string no LCD
    while (*str){
        lcdWriteByte(*str++, 1);
    }
}

void lcdHex8 (uint8_t hex){                     // Escreve um numero Hexadecimal no lcd
    char LSN = convNibble_hexStr(hex & 0x0F);   // Encontra a parte menos significativa
    char MSN = convNibble_hexStr(hex >> 4);     // Encontra a parte mais significativa
    lcdWriteByte(MSN, 1);                       //
    lcdWriteByte(LSN, 1);
}

void lcdHex16 (uint16_t hex){
    lcdHex8(hex >> 8);
    lcdHex8(hex & 0x00FF);
}
void lcdDec8 (uint8_t dec){
    char str[4];
    char *p = str;

    uint8_t temp = dec/100;
    *p = 48 + temp;
    p++;

    dec -= temp*100;
    temp = (dec)/10;
    *p = 48 + temp;
    p++;

    temp = (dec - temp*10);
    *p = 48 + temp;
    p++;
    *p = 0;

    lcdPrint(str);
}

void lcdDec16 (uint16_t dec){
    char str[6];
    char *p = str;

    uint8_t temp = dec/10000;
    *p = 48 + temp;
    p++;

    dec -= temp*10000;
    temp = dec/1000;
    *p = 48 + temp;
    p++;

    dec -= temp*1000;
    temp = dec/100;
    *p = 48 + temp;
    p++;

    dec -= temp*100;
    temp = dec/10;
    *p = 48 + temp;
    p++;

    temp = (dec - temp*10);
    *p = 48 + temp;
    p++;
    *p = 0;

    lcdPrint(str);
}

void lcdFloat (float x){
    char str[6];
    char *p = str;
    uint8_t digito = x;
    *p++ = 48 + digito;
    *p++ = 44;          //virgula

    x = 10*(x-digito);
    digito = x;
    *p++ = 48 + digito;

    x = 10*(x-digito);
    digito = x;
    *p++ = 48 + digito;

    x = 10*(x-digito);
    digito = x;
    *p++ = 48 + digito;

    *p = 0;
    lcdPrint(str);
}

char convNibble_hexStr (uint8_t nibble){   // Escreve o nibble em hexadecimal
    if (nibble < 10){
        return (48 + nibble);              // números na tabela ASCII estão a partir do n 48 (0)
    }else{
        return (nibble - 10 + 65);         // letras maiúsculas estão a partir de 65 (A)
    }
}

void wait_time(uint16_t time_us){
    TB2CTL = TBSSEL__SMCLK | MC__UP | TBCLR;    // Utilizaï¿½ï¿½o do SMCLK | Modo UP | Clear
    TB2CCR0 = time_us - 1;                      // CCR0 = time*freq_clock - 1

    while(!(TB2CCTL0 & CCIFG));
    TB2CCTL0 &= ~CCIFG;
    return;
}

void wait_long_time(uint16_t time_us){
    TB1CTL = TBSSEL__ACLK | MC__UP | TBCLR | ID__2; // Utilizaï¿½ï¿½o do SMCLK | Modo UP | Clear
    TB1CCR0 = time_us - 1;                          // CCR0 = time*freq_clock - 1

    while(!(TB1CCTL0 & CCIFG));
    TB1CCTL0 &= ~CCIFG;
    return;
}

void lcdCLR(){
    lcdWriteByte(0x01,0);           // Limpa a tela
}

void lcdUpdate(float tensao1V, uint16_t tensao1Hex, float tensao2V, uint16_t tensao2Hex){
    lcdPrint("A0: ");               // A0: 'tensao1V'V 'tensao1Hex'
    lcdFloat(tensao1V);
    lcdPrint("V  ");
    lcdHex16(tensao1Hex);
    lcdWriteByte(0xC0,0);           // Pula para a segunda linha
    lcdPrint("A1: ");
    lcdFloat(tensao2V);
    lcdPrint("V  ");                // A1: 'tensao2V'V 'tensao2Hex'
    lcdHex16(tensao2Hex);
}
