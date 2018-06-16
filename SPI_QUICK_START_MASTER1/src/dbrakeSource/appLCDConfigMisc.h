//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: appLCD.h
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor: STM32F103R
// TOOLS: IAR Workbench
// DATE:
// CONTENTS: This file contains
//------------------------------------------------------------------------------
// HISTORY: This file
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#ifndef APP_LCDCONFIGMISC_H
#define APP_LCDCONFIGMISC_H
 
//---------------------GLOBAL DEFINITIONS-----------------------------------
 
//---------------------GLOBAL VARIABLES-----------------------------------

 
//---------------------GLOBAL FUNCTION PROTOTYPES--------------------------
uint8_t AppPairingActive(void);
void AppLCDConfigMiscInit(void);
void AppLCDConfigMiscHandle(void);
void SetPairActiveOff(void);
void AppLCDConfigMisc(uint8_t key);
void AppScreenSetupKey(uint8_t key);
void AppScreenSetupInit(void);
void AppScreenSetupItemKey(uint8_t screen,uint8_t key);
#endif