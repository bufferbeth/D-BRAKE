//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: CONFIG.H
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "dbrakeDefs.h"
 
#define CONFIGDB_VER0 '0'
#define CONFIGDB_VER1 '0'
#define CONFIGDB_VER2 '6'

//---------------------GLOBAL DEFINITIONS--------------------------

  //--------------------------------
  // Table definitions
  //--------------------------------
typedef enum
{
  // address 00 and 01 are test bytes
  //-----------------------------------
  // 0x0005 - header = CREEDVx.xx  (10 bytes)
  //-------------------------------------------
  TableHeader1_Setting =  0x0002,
  TableHeader2_Setting =  0x0003,
  TableHeader3_Setting =  0x0004,
  TableHeader4_Setting =  0x0005,
  TableHeader5_Setting =  0x0006,
  TableHeader6_Setting =  0x0007, 
  TableHeader7_Setting =  0x0008,
  TableHeader8_Setting =  0x0009,
  TableHeader9_Setting =  0x0010,
  TableHeader10_Setting =  0x0011, 
  
  //----------------------------------
  // TABLE 0 START - KEEP IN ORDER FOR INDEX READ/WRITES/
  // 28 BYTES
  // 00x0028 - 0x0044
  //----------------------------------- 
  Key_Setting =		    0x0028,  
  Mode_Setting  =       0x0029,
  MaxForce_Setting =    0x002A,
  
  RT_Ctr_X_Setting =    0x002B,   
  RT_Ctr_H_Setting =    0x002C,
  RT_Ctr_M_Setting =    0x002D,
  RT_Ctr_L_Setting =    0x002E,
  
  FWVersion3 =			0x002F,
  FWVersion2 =			0x0030,
  FWVersion1 =			0x0031,
  FWVersion0 =			0x0032,   

//----------------- 
// 6 Bytes for EEPROM

  PairAddressMSB =		0x0039,
  PairAddressLSB =		0x003a,  
  ScreenColorSetting =	0x003b,
  BackLightOnSetting =	0x003c,
  TempFarenheitOnSetting = 0x003d,   
  ForceMaxSetting = 0x003e, 
  ActiveBrakeEnableSetting = 0x003f, 
  TPMSEnableSetting = 0x0040,
  HybridSetting = 0x0041, 
  BrakeBacklightSetting = 0x0042, 
  SensitivitySetting = 0x0043,
  TableSensorStart =    0x0050,
  TableSensorEnd =		0x007F, 
}UserSettingType;

 

//---------------------GLOBAL VARIABLES--------------------------

typedef union
{
  uint8_t Index[28];
  struct
  {
    //Remote speed settings
    uint8_t Key;
    uint8_t Mode;
    uint8_t MaxForce;
    uint8_t RunTimeCounterX;
    uint8_t RunTimeCounterH;
    uint8_t RunTimeCounterM;
	uint8_t RunTimeCounterL;
	uint8_t FirmwareVersion3;
	uint8_t FirmwareVersion2;	
	uint8_t FirmwareVersion1;
	uint8_t FirmwareVersion0;	
	uint8_t EepromManDevSerial[6];
	uint8_t PairAddress[2];
	//-------------------------
	uint8_t ScreenColor;
	uint8_t BackLightOn; 
	uint8_t TempFarenheitOn; 
	uint8_t ForceMaxSet; 
	uint8_t ActiveBrakeEnable;
	uint8_t TPMSEnable;
	uint8_t Hybrid;  //Sensitivity;
	uint8_t BrakeBacklight; 
	uint8_t SensitivitySet;
	//------------------------
	uint8_t Spare[1];   
  }Item;
}Table0; 

extern Table0 table0; 

typedef struct  
{
    uint8_t ID[3];
    uint8_t Position;	
}SENSOR; 

typedef union
{
  uint8_t Index[48];
  struct
  {
    SENSOR WhichSensor[MAXSENSORS];
  }Item;
}TABLESENSOR; 

extern TABLESENSOR tableSensor;   
  
//---------------------GLOBAL PROTOTYPES--------------------------
void ConfigInit(void); 
void ConfigUpdate(uint8_t value,uint8_t settingAddress);

#endif
//end of config.h






