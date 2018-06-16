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


#ifndef DRIVER_T_SPI
#define DRIVER_T_SPI


//-------------------------------
// SPI RF433 COMMAND CODES. 
#define SigmaX_eepromwrite				0x09
#define SigmaX_eepromread				0x0a
#define SigmaX_Startrssi				0x1b
#define SigmaX_GetRssiValue				0x1c
#define SigmaX_setmodecommand			0x0d
#define SigmaX_calcheckcommand			0x0e
#define SigmaX_idlemodeconfig			0x10
#define SigmaX_pollmodeconfig			0x13
#define SimgaX_syschanconfig			0xc0
#define SigmaX_rxmodeconfig				0x16
#define SigmaX_txmodeconfig				0x15
#define SigmaX_loadtxbuffcommand		0x0b
#define SigmaX_loadtxprebuffcommand		0x0c
#define SigmaX_resetrom					0x10
#define SigmaX_resetsys					0x15
#define SigmaX_getstatuscommand			0x04
#define SigmaX_getrxfilllevelcommand	0x01
#define SigmaX_gettxfilllevelcommand	0x02
#define SigmaX_getrxdatacommand			0x06
#define SigmaX_dummycommand				0x00
#define SigmaX_getrssifilllevelcommand	0x03
#define Sigmax_getrssidatacommand		0x05
#define SigmaX_sramreadblock			0x08
#define SigmaX_sramwritedblock			0x07
#define SigmaX_getflashversion			0x13
#define SigmaX_getTemp					0x19
#define SigmaX_setvoltmon				0x17
 
 
 typedef struct
 {
	 uint8_t ID[3];
	 uint8_t State;
	 uint8_t Change;
	 uint8_t LastPacket[7];
	 uint16_t Pressure;	 //in KPA
	 uint8_t Temperature;  //in celcius
	 uint8_t NewValue;
 }SENSORDATA;

 
// extern SENSORDATA sensor[MAXSENSORS];
//extern SENSORDATA sensorPlaced[MAXSENSORS]; 
extern SENSORDATA sensorDynamic[MAXSENSORS];
 
 
void PressureProvidRSSI(uint8_t *rssiPeak,uint8_t *rssiAvg); 
void RF433Init(void);
void RF433Task(void);
void PressureUpdateTask(void);
uint8_t PressureProvideData(uint8_t which,uint16_t *pressure,uint16_t *temperature,uint8_t staticList);
#endif
