
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: appLCD.c
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor: STM32F103R
// TOOLS: IAR Workbench
// DATE:
// CONTENTS: This file contains

//				tempData[i] = SystemSensor[(8*16*6)+18+i+(j*6)];
//  (8*16*6) -- this 6 is the 6th  block of the 0-7 blocks
//   18 is the offset horizontally from the left of the display
//------------------------------------------------------------------------------
// HISTORY: This file
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#include <asf.h>
#include "dbrakeDefs.h"
#if REMOTEBOARD
#include "driverLCD.h"
#include "appLCD.h"
#include "driverButtons.h"
#include "screenNumbers.h"
#include "AppConversions.h"
#include "driverTSPI.h"
#include "config.h"
#include "appProtocol.h"
#include "appAccel.h"
#include "driveri2c.h"
#include "applcdconfig.h"

 
//---------------------GLOBAL VARIABLES-----------------------------------

 

//---------------------LOCAL VARIABLES------------------------------------
//--------------------------
 

//---------------------LOCAL FUNCTION PROTOTYPES--------------------------
void AppSkeletonBrakeState(void); 

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppSkeletonBrakeState(void)
{
	uint8_t offset,i,*ptr,tempData[6];
	uint8_t *wordList[10],j;
	
 
	wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
	wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLC];
	wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
	wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLU];
	wordList[4] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
	wordList[5] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
	wordList[6] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
	wordList[7] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];	
	wordList[8] = (uint8_t *)FONTSMALL_BLANK;

	offset = 50;
 	for (j=0;j<9;j++)
	{
		ptr = wordList[j];
		for (i=0;i<5;i++)
		{
			tempData[i] = *ptr++;			
		}
		tempData[5] = 0; 
		LCDPlaceData(offset,8,tempData,6);
		offset+=6;
	}
	
	if ((brakeStatus.ActuatorStatus	 & ACTUATORSTATUS_BOTHLIMITSACTIVE)!= 0)
	{
		wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLB];
		wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
		wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLH];
		wordList[4] = (uint8_t *)FONTSMALL_BLANK;

		offset = 0;
 		for (j=0;j<5;j++)
		{
			ptr = wordList[j];
			for (i=0;i<5;i++)
			{
				tempData[i] = *ptr++;			
			}
			tempData[5] = 0; 
			LCDPlaceData(offset,16,tempData,6);
			offset+=6;
		}		
	}	
	if ((brakeStatus.ActuatorStatus	 & ACTUATORSTATUS_HOMEONFAIL)!= 0)
	{
		wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLH];
		wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLM];
		wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
		wordList[4] = (uint8_t *)FONTSMALL_BLANK;

		offset =30;
 		for (j=0;j<5;j++)
		{
			ptr = wordList[j];
			for (i=0;i<5;i++)
			{
				tempData[i] = *ptr++;			
			}
			tempData[5] = 0; 
			LCDPlaceData(offset,16,tempData,6);
			offset+=6;
		}		
	}		
	if ((brakeStatus.ActuatorStatus	 & ACTUATORSTATUS_EXTENDONFAIL)!= 0)
	{
		wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
		wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLX];
		wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
		wordList[4] = (uint8_t *)FONTSMALL_BLANK;

		offset =60;
 		for (j=0;j<5;j++)
		{
			ptr = wordList[j];
			for (i=0;i<5;i++)
			{
				tempData[i] = *ptr++;			
			}
			tempData[5] = 0; 
			LCDPlaceData(offset,16,tempData,6);
			offset+=6;
		}		
	}			
	if ((brakeStatus.ActuatorStatus	 & ACTUATORSTATUS_HOMEOFFFAIL)!= 0)
	{
		wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLH];
		wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLM];
		wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		wordList[4] = (uint8_t *)FONTSMALL_BLANK;

		offset =30;
 		for (j=0;j<5;j++)
		{
			ptr = wordList[j];
			for (i=0;i<5;i++)
			{
				tempData[i] = *ptr++;			
			}
			tempData[5] = 0; 
			LCDPlaceData(offset,24,tempData,6);
			offset+=6;
		}		
	}		
	if ((brakeStatus.ActuatorStatus	 & ACTUATORSTATUS_EXTENDOFFFAIL)!= 0)
	{
		wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
		wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLX];
		wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		wordList[4] = (uint8_t *)FONTSMALL_BLANK;

		offset =60;
 		for (j=0;j<5;j++)
		{
			ptr = wordList[j];
			for (i=0;i<5;i++)
			{
				tempData[i] = *ptr++;			
			}
			tempData[5] = 0; 
			LCDPlaceData(offset,24,tempData,6);
			offset+=6;
		}		
	}		
	if ((brakeStatus.ActuatorStatus	 & ACTUATORSTATUS_ENCODERFAIL)!= 0)
	{
		wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
		wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
		wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLC];
		wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		wordList[4] = (uint8_t *)FONTSMALL_BLANK;

		offset =0;
 		for (j=0;j<5;j++)
		{
			ptr = wordList[j];
			for (i=0;i<5;i++)
			{
				tempData[i] = *ptr++;			
			}
			tempData[5] = 0; 
			LCDPlaceData(offset,24,tempData,6);
			offset+=6;
		}		
	}					
}

#endif 

