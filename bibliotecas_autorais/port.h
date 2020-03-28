#ifndef LIB_PORT_H_
#define LIB_PORT_H_

void debounce(uint16_t dt);

void ledVermelho();

void ledVerde();

void botaoS1();

void botaoS2();

int contadorBinario();

int alternaLED();

int frequencimetro ();

void setPWM_ACLK (const uint16_t T, const uint16_t D);

#endif /* LIB_PORT_H_ */
