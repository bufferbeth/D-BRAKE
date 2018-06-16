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
#ifndef APP_LCDCONFIG_H
#define APP_LCDCONFIG_H
 
//---------------------GLOBAL DEFINITIONS-----------------------------------
 
//---------------------GLOBAL VARIABLES-----------------------------------

 
//---------------------GLOBAL FUNCTION PROTOTYPES--------------------------
extern void ConfigSensorTableClear(void);
extern void ConfigSensorUpdate(uint8_t which);
extern void AppLCDConfigTireHandle(void);
extern void AppLCDConfigTireInit(void);
extern uint8_t ConfigSensorPresent(uint8_t whichRam); 

#endif