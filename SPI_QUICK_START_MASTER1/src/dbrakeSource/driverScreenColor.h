//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: driverSpeaker.h
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor: STM32F103R
// TOOLS: IAR Workbench
// DATE:
// CONTENTS: This file contains
//------------------------------------------------------------------------------
// HISTORY: This file
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#ifndef DRIVER_SCREENCOLOR_H
#define DRIVER_SCREENCOLOR_H

//---------------------GLOBAL DEFINITIONS-----------------------------------
#define MAX_SCREENCOLOR 8

//---------------------GLOBAL VARIABLES-----------------------------------


//---------------------GLOBAL FUNCTION PROTOTYPES--------------------------
void BacklightSetColor(uint8_t color); 
void BacklightToggleLight(uint8_t color);
void BacklightSetHomeColor(uint8_t color); 
#endif




