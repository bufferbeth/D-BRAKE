//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE:  
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor:  
// TOOLS:  
// DATE:
// CONTENTS: This file contains  
//------------------------------------------------------------------------------
// HISTORY: This file  
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


#ifndef DRIVER_I2C
#define DRIVER_I2C

uint8_t I2CEEPROMBufferWrite(uint8_t *buffer,uint8_t setting,uint8_t count);
uint8_t I2CEEPROMBufferRead(uint8_t *buffer,uint8_t setting,uint8_t count);
void I2Cmain(void);
uint8_t I2CAccelBufferWrite(uint8_t *buffer,uint8_t setting, uint8_t count);
uint8_t I2CAccelBufferRead(uint8_t *buffer,uint8_t setting,uint8_t count);
#endif
