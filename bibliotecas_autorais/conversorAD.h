#ifndef CONVERSOR_AD_H
#define CONVERSOR_AD_H

uint16_t acdRead(uint8_t pin);
void adcConfig();
void adcConfigSingleCh();
void timerConfigB1();
void timerConfigB0();
void getTensoes(uint16_t *canal1, uint16_t *canal2, uint16_t *inputData);
uint16_t media(uint16_t *vetorPtr);

#endif /* CONVERSOR_AD_H */
