


#ifndef DRIVER_X_SPI
#define DRIVER_X_SPI


int SPIXMain(void);
void SPIXConfigure(void);
int SPIXInOut(uint8_t addr,uint8_t *buffer,uint16_t size);
int SPIXInOut2(uint8_t addr,uint8_t *buffer,uint16_t size);


#endif
