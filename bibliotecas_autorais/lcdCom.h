#ifndef LED_COM_H
#define LED_COM_H

char lcdQualAdr ();
void lcdBacklightON();
void lcdBacklightOFF();
void lcdWriteNibble (uint8_t nibble, uint8_t rs);
void lcdWriteByte (uint8_t byte, uint8_t RS);
void lcdInit ();
void lcdPrint(char *str);
void wait_time(uint16_t time_us);
void wait_long_time(uint16_t time_us);
void lcdCLR();
void lcdHex8 (uint8_t hex);
char convNibble_hexStr (uint8_t nibble);
void lcdHex16 (uint16_t hex);
void lcdDec8 (uint8_t dec);
void lcdDec16 (uint16_t dec);
void lcdFloat (float x);
void lcdUpdate(float tensao1V, uint16_t tensao1Hex, float tensao2V, uint16_t tensao2Hex);

#endif /* LED_COM_H */



