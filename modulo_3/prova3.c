#include <stdint.h>
#include <msp430.h>
#include <lib/libSerial.h>
#include <lib/lcdCom.h>
#include <lib/conversorAD.h>

uint8_t passwdMSB;
uint8_t passwdLSB;
uint16_t passwd;

uint8_t findMSB(uint8_t addr)
{
    uint8_t resposta;
    passwdMSB = 0x00;
    i2cSend(addr, &passwdMSB, 1);       // Testa o valor 0x00 para o MSB
    i2cGet(addr, &resposta, 1);

    while(resposta == 0x00)             // Se o escravo nao aceitou o teste
    {
        passwdMSB ++;
        i2cSend(addr, &passwdMSB, 1);   // Testa o proximo valor do MSB
        i2cGet(addr, &resposta, 1);     // Verifica a resposta do escravo
    }

    return passwdMSB;
}

uint8_t findLSB(uint8_t addr)
{
    uint8_t resposta;
    passwdLSB = 0x00;
    i2cSend(addr, &passwdLSB, 1);       // Testa o valor 0x00 para o LSB
    i2cGet(addr, &resposta, 1);

    while(!(resposta == 0x02))          // Se o escravo nao aceitou o teste
    {
        if(resposta == 0)               // Se deu erro no LSB
        {
            i2cSend(addr, &passwdMSB, 1);       // Reenvia o MSB
            i2cGet(addr, &resposta, 1);
            if(!(resposta == 0x01))             // Se a primeira parte da senha falhou
            {                                   // Tem algo errado na logica
                return 0;                       // Retorna 0 como erro
            }
        }
        passwdLSB ++;
        i2cSend(addr, &passwdLSB, 1);   // Testa o proximo valor do LSB
        i2cGet(addr, &resposta, 1);     // Verifica a resposta do escravo
    }

    return passwdLSB;
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~(LOCKLPM5);

    //-------------------     CONFIGURAÇÃO INICIAL DO I2C      ---------------------------
	i2cInitB0 (1, 0x00);                  // B0 -> mestre

	//i2cInitInt(1, 0x00);                // Iniciar interface I2C como mestre

    uint8_t slaveAddr = 32;             // Endereco do escravo encontrado

    //while (i2cTst(slaveAddr)){            // Procura o endereco do escravo
    //    slaveAddr++;
    //}


	//-------------------     CONFIGURAÇÃO INICIAL DO LCD     ----------------------------

	while(!(lcdQualAdr()));               // Verifica a comunicacao mestre-escravo e configura o addr da PCF
	lcdInit();                            // Configura o LCD em modo de 4 bits, duas linha,
	                                      // Display ON, Cursor ON e tela limpa
	lcdBacklightON();                     // Acende a backlight

	findMSB(slaveAddr);

	findLSB(slaveAddr);
	
	passwd = (passwdMSB << 8) + passwdLSB;  // Encontra a senha completa

	lcdCLR();                             // Reinicia o Display

	lcdPrint("SENHA ENCONTRADA:");        // Escreve a senha no lcd
	lcdWriteByte(0xC0,0);                 // Pula para a segunda linha
	lcdPrint("      ");                   // SENHA ENCONTRADA:
	lcdHex16(passwd);                     //        0xFFFF

	while(1);

	return 0;
}
