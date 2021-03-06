#ifndef LIB_SERIAL_H_
#define LIB_SERIAL_H_
//-------------------   INTERRUPÇÃO   -------------------
void i2cInitInt (uint8_t isMaster, uint8_t myAddr);
uint8_t i2cTstInt (uint8_t addr);
uint8_t i2cPutInt (uint8_t addr, uint8_t data);
uint8_t i2cGetInt (uint8_t addr, uint8_t *data);


//--------------------   POOLING   -----------------------
void i2cInitB0 (uint8_t isMaster, uint8_t myAddr);
void i2cInitB1 (uint8_t isMaster, uint8_t myAddr);
uint8_t i2cTst (uint8_t addr);
uint8_t i2cSend (uint8_t dstAddr, uint8_t *dataPtr, uint8_t nBytes);
uint8_t i2cGet(uint8_t addr, uint8_t *dataPtr, uint8_t count);
uint8_t i2cSendByte (uint8_t addr, uint8_t byte);
uint8_t i2cGetByte (uint8_t addr);

void delay(long limite);


#endif /* LIB_PORT_H_ */
