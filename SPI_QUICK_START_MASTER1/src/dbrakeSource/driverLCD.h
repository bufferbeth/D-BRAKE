/**********************************
*        Macro Defination		  *
***********************************/
 



/**********************************
*      Function Declaration		  *
**********************************/
extern void delayUs(uint16_t us);
extern void delayMs(uint16_t ms);

extern void keyInit(void);

extern void lcdDdataPinsDirectionSet(uint8_t direction);
extern void lcdPinInit(void);

extern void setRST(uint8_t RST_value);
extern void setA0(uint8_t A0_value);
extern void setRW(uint8_t RW_value);
extern void setE(uint8_t E_value);

extern void writeComd(uint8_t comd);
extern void writeData(uint8_t data,uint8_t inverse);

extern void lcdInit(void);

extern void fillScreen(uint8_t data);
extern void cleanScreen(void);
extern void FillScreen3(void);
extern void placeDot(uint16_t x, uint16_t y);

extern void refreshScreen(const uint8_t *gData,uint8_t inverse);
void LCDClearArea(uint8_t x,uint8_t y, uint8_t horizontalLength, uint8_t verticalLength);
void LCDPlaceData(uint8_t x,uint8_t y, const uint8_t *data,uint8_t size);
void LCDPlacePage(uint8_t page,const uint8_t *gData,uint8_t inverse); 

 