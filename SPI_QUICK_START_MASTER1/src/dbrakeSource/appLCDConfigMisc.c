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
#include "driverSpeaker.h"
#include "driverScreenColor.h"
#include "appLCDConfigMisc.h"

#define MAX_FORCE_SET 9
#define MAX_SENSITIVITY_SET 9

 #define CONFIG_MISC_UP			0
 #define CONFIG_MISC_SELECT		1
 #define CONFIG_MISC_ROTATE		2
 
//---------------------GLOBAL VARIABLES-----------------------------------
uint8_t pairActive = FALSE;  
uint8_t speakerActive = FALSE;  

//---------------------LOCAL VARIABLES------------------------------------
//--------------------------
uint8_t configMiscOffset;
uint8_t changeToDefault;

#define CONFIGMISC_IDLE 0
#define CONFIGMISC_SELECT 1
#define CONFIGMISC_ROTATE 2
uint8_t configMiscState; 
 
//---------------------LOCAL FUNCTION PROTOTYPES--------------------------
 void AppLCDConfigMiscPair(uint8_t state,uint8_t reverse);
 void ConfigMiscSelect(uint8_t which,uint8_t select);
 void AppLCDConfigMiscBlight(uint8_t state,uint8_t reverse);
 void AppLCDConfigMiscTemp(uint8_t state,uint8_t reverse);
 void AppLCDConfigMiscScolor(uint8_t state,uint8_t reverse);
 void AppLCDConifgMiscSelectDeselect(uint8_t which,uint8_t state);
// void BacklightSet(state);
 void AppLCDConfigMiscSpeaker(uint8_t state,uint8_t reverse);
 void AppLCDConfigMiscTPMSEnable(uint8_t state,uint8_t reverse);
 void AppLCDConfigMiscActiveBrakeEnable(uint8_t state,uint8_t reverse);
 void AppLCDConfigMiscForceMaxSet(uint8_t state,uint8_t reverse);
 void AppScreenPlaceBrakeBacklight(uint8_t value);
 void AppScreenPlaceSensitivitySet(uint8_t value);
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

uint8_t currSetupOffset;
uint8_t prevSetupOffset; 
#define MAXSETUPOFFSET 6
void AppScreenSetupSensitivity(uint8_t value,uint8_t reverse,uint8_t newoffset);
 
void AppScreenSetupHybrid(uint8_t state,uint8_t reverse,uint8_t newoffset,uint8_t line);
void AppScreenSetupForceMaxSet(uint8_t state,uint8_t reverse,uint8_t newoffset);
void AppScreenSetupLine(uint8_t linetosetup, uint8_t reverse);
void AppScreenPlaceMaxForceSet(uint8_t value);
void AppScreenPlaceHybrid(uint8_t value);
void AppScreenSetupBrakeBacklight(uint8_t value,uint8_t reverse,uint8_t newoffset);
void AppScreenSetupSensitivitySet(uint8_t state,uint8_t reverse,uint8_t newoffset);
void AppScreenSetupMaxForceSet(uint8_t state,uint8_t reverse,uint8_t newoffset);
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppScreenSetupInit(void)
{
	uint8_t temp,temp2;
	uint8_t i;
	currSetupOffset = 0; 
	prevSetupOffset = 0; 
	AppScreenSetupLine(currSetupOffset,TRUE); 
			temp = table0.Item.PairAddress[0];
			temp2 = temp & 0x0f;
			temp = temp>>4;
			LCDPlaceData(85,16,(uint8_t *)FONTSMALL[temp],5);
			LCDPlaceData(91,16,(uint8_t *)FONTSMALL[temp2],5);
			temp = table0.Item.PairAddress[1];
			temp2 = temp & 0x0f;
			temp = temp>>4;
			LCDPlaceData(97,16,(uint8_t *)FONTSMALL[temp],5);
			LCDPlaceData(103,16,(uint8_t *)FONTSMALL[temp2],5);	
			
//	AppScreenSetupHybrid(table0.Item.Hybrid,FALSE,85,8);		
	AppScreenSetupForceMaxSet(table0.Item.ForceMaxSet,FALSE,85);	
	AppScreenSetupBrakeBacklight(table0.Item.BrakeBacklight,FALSE,108);	
	AppScreenSetupSensitivity(table0.Item.SensitivitySet,FALSE,85);			
}


void AppScreenSetupItemKey(uint8_t screen,uint8_t key)
{
	uint8_t temp,temp2,*wordList[20],testBuffer[3];
	uint8_t offset,i,j,tempData[6],*ptr;
	switch(screen)
	{							 		 
		case SCREEN_SETUPPAIR:
		{
			if (key == KEY_CENTER)
			{
				AppScreenInit(SCREEN_SETUPNEW);			
				pairActive = FALSE;	
			}
			else
			{
				if (key == KEY_LEFT)
				{
/* V01.12 ..... removed					
					testBuffer[0] = 0;
					I2CEEPROMBufferWrite((uint8_t*)testBuffer,PairAddressMSB,1);
					table0.Item.PairAddress[0] = 0;
					testBuffer[0] = 0;
					I2CEEPROMBufferWrite((uint8_t*)testBuffer,PairAddressLSB,1);
					table0.Item.PairAddress[1] = 0;
*/					
					pairActive = TRUE;
					offset = 2;
					tempData[5] = 0x00;
					wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLP];
					wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
					wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLI];
					wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
					wordList[4] = (uint8_t *)FONTSMALL_BLANK;
					wordList[5] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
					wordList[6] = (uint8_t *)FONTSMALL[OFFSET_SMALLC];
					wordList[7] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
					wordList[8] = (uint8_t *)FONTSMALL[OFFSET_SMALLI];
					wordList[9] = (uint8_t *)FONTSMALL[OFFSET_SMALLV];
					wordList[10] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
					wordList[11] = (uint8_t *)FONTSMALL_BLANK;
					wordList[12] = (uint8_t *)FONTSMALL_BLANK;
					wordList[13] = (uint8_t *)FONTSMALL_BLANK;
					wordList[14] = (uint8_t *)FONTSMALL_BLANK;	
					wordList[15] = (uint8_t *)FONTSMALL_BLANK;																	
					for (j=0;j<16;j++)
					{
						ptr = wordList[j];
						for (i=0;i<5;i++)
						{
							tempData[i] = *ptr++;
//							if (reverse == TRUE)
//							{
//								tempData[i] ^= 0xff;
//								tempData[5] = 0xFF;
//							}
							
						}
						LCDPlaceData(offset,32,tempData,6);
						offset+=6;
					}	
					offset = 0;	
					wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLP];
					wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
					wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
					wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLS];
					wordList[4] = (uint8_t *)FONTSMALL[OFFSET_SMALLS];
					wordList[5] = (uint8_t *)FONTSMALL_BLANK;
					wordList[6] = (uint8_t *)FONTSMALL[OFFSET_SMALLS];
					wordList[7] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
					wordList[8] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
					wordList[9] = (uint8_t *)FONTSMALL[OFFSET_SMALLU];
					wordList[10] = (uint8_t *)FONTSMALL[OFFSET_SMALLP];
					wordList[11] = (uint8_t *)FONTSMALL_BLANK;
					wordList[12] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
					wordList[13] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
					wordList[14] = (uint8_t *)FONTSMALL_BLANK;
					wordList[15] = (uint8_t *)FONTSMALL[OFFSET_SMALLB];
					wordList[16] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
					wordList[17] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
					wordList[18] = (uint8_t *)FONTSMALL[OFFSET_SMALLK];
					wordList[19] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];															
					for (j=0;j<20;j++)
					{
						ptr = wordList[j];
						for (i=0;i<5;i++)
						{
							tempData[i] = *ptr++;
//							if (reverse == TRUE)
//							{
//								tempData[i] ^= 0xff;
//								tempData[5] = 0xFF;
//							}
							
						}
						LCDPlaceData(offset,24,tempData,6);
						offset+=6;
					}							
				}
				else
				{
					if (key == KEY_RIGHT)
					{
						pairActive = FALSE;
						offset = 2;
						tempData[5] = 0x00;
						wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLP];
						wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
						wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLI];
						wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
						wordList[4] = (uint8_t *)FONTSMALL_BLANK;

						wordList[5] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
						wordList[6] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
						wordList[7] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
						wordList[8] = (uint8_t *)FONTSMALL_BLANK;						
						wordList[9] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
						wordList[10] = (uint8_t *)FONTSMALL[OFFSET_SMALLC];
						wordList[11] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
						wordList[12] = (uint8_t *)FONTSMALL[OFFSET_SMALLI];
						wordList[13] = (uint8_t *)FONTSMALL[OFFSET_SMALLV];
						wordList[14] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
						wordList[15] = (uint8_t *)FONTSMALL_BLANK;
						for (j=0;j<16;j++)
						{
							ptr = wordList[j];
							for (i=0;i<5;i++)
							{
								tempData[i] = *ptr++;
//								if (reverse == TRUE)
//								{
//									tempData[i] ^= 0xff;
//									tempData[5] = 0xFF;
//								}
								
							}
							LCDPlaceData(offset,32,tempData,6);
							offset+=6;
						}
					}
				}
				temp = table0.Item.PairAddress[0];
				temp2 = temp & 0x0f;
				temp = temp>>4;
				LCDPlaceData(95,32,(uint8_t *)FONTSMALL[temp],5);
				LCDPlaceData(101,32,(uint8_t *)FONTSMALL[temp2],5);
				temp = table0.Item.PairAddress[1];
				temp2 = temp & 0x0f;
				temp = temp>>4;
				LCDPlaceData(107,32,(uint8_t *)FONTSMALL[temp],5);
				LCDPlaceData(113,32,(uint8_t *)FONTSMALL[temp2],5);				
			}			
			break;
		}		
		case SCREEN_SETUPRESET:
		{
			if (key == KEY_CENTER)
			{
				AppScreenInit(SCREEN_SETUPNEW);	
				if (changeToDefault == TRUE)
				{
					table0.Item.ForceMaxSet = 7; 
					table0.Item.MaxForce = 5; 
					table0.Item.SensitivitySet = 9; 
//					table0.Item.Hybrid = FALSE; 
//					ConfigUpdate(table0.Item.Hybrid,HybridSetting);	
					ConfigUpdate(table0.Item.ForceMaxSet,ForceMaxSetting);
					ConfigUpdate(table0.Item.MaxForce,MaxForce_Setting);
					ConfigUpdate(table0.Item.SensitivitySet,SensitivitySetting);
//					AppScreenPlaceHybrid(table0.Item.Hybrid);
//					AppScreenPlaceMaxForceSet(table0.Item.ForceMaxSet);				
				}
			}
			else
			{
				if (key == KEY_LEFT)
				{
//				 	AppScreenSetupHybrid(FALSE,FALSE,79,24);		
					AppScreenSetupForceMaxSet(7,FALSE,79);		
//				 	AppScreenSetupHybrid(FALSE,FALSE,110,24);		
					AppScreenSetupForceMaxSet(7,FALSE,110);		
					AppScreenSetupMaxForceSet(5,FALSE,79);				
					AppScreenSetupMaxForceSet(5,FALSE,110);		
					AppScreenSetupSensitivitySet(9,FALSE,79);			
					AppScreenSetupSensitivitySet(9,FALSE,110);																						
					changeToDefault = TRUE; 					 
				}
				else
				{
					if (key == KEY_RIGHT)
					{
//				 		AppScreenSetupHybrid(table0.Item.Hybrid,FALSE,79,24);		
						AppScreenSetupForceMaxSet(table0.Item.ForceMaxSet,FALSE,79);		
//				 		AppScreenSetupHybrid(FALSE,FALSE,110,24);		
						AppScreenSetupForceMaxSet(7,FALSE,110);		
					 
						AppScreenSetupMaxForceSet(table0.Item.MaxForce,FALSE,79);				
						AppScreenSetupMaxForceSet(5,FALSE,110);		
						AppScreenSetupSensitivitySet(table0.Item.SensitivitySet,FALSE,79);			
						AppScreenSetupSensitivitySet(9,FALSE,110);										
						changeToDefault = FALSE; 							 
					}
				}
			}	
			break;
		}
		case SCREEN_SETUPHYBRID:
		{
			if (key == KEY_CENTER)
			{
				//-------------------
				// CENTER key released
				AppScreenInit(SCREEN_SETUPNEW);
			}
			else
			{
				if (key == KEY_LEFT)
				{
					//-------------------
					// LEFT key released
					// decrement force value
					if (table0.Item.Hybrid == FALSE) 
					{
						table0.Item.Hybrid = TRUE;
						table0.Item.MaxForce = HYBRID_FORCE_SETTING; 
						ConfigUpdate(table0.Item.MaxForce,MaxForce_Setting);
					}
					else
					{
						table0.Item.Hybrid=FALSE;
					}
					
					ConfigUpdate(table0.Item.Hybrid,HybridSetting);
					AppScreenPlaceHybrid(table0.Item.Hybrid);
				}
				else
				{
					if (key == KEY_RIGHT)
					{
						//-------------------
						// RIGHT key released
						// increment force value
						if ((table0.Item.Hybrid == FALSE))
						{
							table0.Item.Hybrid = TRUE;
							table0.Item.MaxForce = HYBRID_FORCE_SETTING; 
							ConfigUpdate(table0.Item.MaxForce,MaxForce_Setting);							
						}
						else
						{
							table0.Item.Hybrid= FALSE;
						}
						ConfigUpdate(table0.Item.Hybrid,HybridSetting);
						AppScreenPlaceHybrid(table0.Item.Hybrid);
					}
				}
			}
			break;
		}		
		case SCREEN_SETUPBRAKEBACKLIGHT:
		{
			if (key == KEY_CENTER)
			{
				//-------------------
				// CENTER key released
				AppScreenInit(SCREEN_SETUPNEW);
			}
			else
			{
				if (key == KEY_LEFT)
				{
					//-------------------
					// LEFT key released
					// decrement force value
					if (table0.Item.BrakeBacklight == FALSE) 
					{
						table0.Item.BrakeBacklight = TRUE;
					}
					else
					{
						table0.Item.BrakeBacklight=FALSE;
					}
					ConfigUpdate(table0.Item.BrakeBacklight,BrakeBacklightSetting);
					AppScreenPlaceBrakeBacklight(table0.Item.BrakeBacklight);
				}
				else
				{
					if (key == KEY_RIGHT)
					{
						//-------------------
						// RIGHT key released
						// increment force value
						if ((table0.Item.BrakeBacklight == FALSE))
						{
							table0.Item.BrakeBacklight = TRUE;
						}
						else
						{
							table0.Item.BrakeBacklight= FALSE;
						}
						ConfigUpdate(table0.Item.BrakeBacklight,BrakeBacklightSetting);
						AppScreenPlaceBrakeBacklight(table0.Item.BrakeBacklight);
					}
				}
			}
			break;
		}				
		
		case SCREEN_SETUPMAXFORCE:
		{
			if (key == KEY_CENTER)
			{
				//-------------------
				// CENTER key released
				AppScreenInit(SCREEN_SETUPNEW);
			}
			else
			{
				if (key == KEY_LEFT)
				{
					//-------------------
					// LEFT key released
					// decrement force value
					if (table0.Item.Hybrid == FALSE)
					{
						if (table0.Item.ForceMaxSet == 0) 
						{
							table0.Item.ForceMaxSet = MAX_FORCE_SET; 
							ConfigUpdate(table0.Item.ForceMaxSet,ForceMaxSetting); 							
						}
						else
						{
							table0.Item.ForceMaxSet--;
							ConfigUpdate(table0.Item.ForceMaxSet,ForceMaxSetting);
						}						
					}									
					AppScreenPlaceMaxForceSet(table0.Item.ForceMaxSet);
				}
				else
				{
					
					if (key == KEY_RIGHT)
					{
						
						// increment force value
						if (table0.Item.Hybrid == TRUE)
						{
							table0.Item.ForceMaxSet = 4; 
							ConfigUpdate(table0.Item.ForceMaxSet,ForceMaxSetting); 
						}
						else
						{
							if (table0.Item.ForceMaxSet < MAX_FORCE_SET) 
							{
								table0.Item.ForceMaxSet++;
								ConfigUpdate(table0.Item.ForceMaxSet,ForceMaxSetting);
							}						
						}									
						AppScreenPlaceMaxForceSet(table0.Item.ForceMaxSet);
					}
				}
			}			
			break;
		}
		case SCREEN_SETUPSENSITIVITY:
		{
			if (key == KEY_CENTER)
			{
				//-------------------
				// CENTER key released
				AppScreenInit(SCREEN_SETUPNEW);
			}
			else
			{
				if (key == KEY_LEFT)
				{
					//-------------------
					// LEFT key released
					// decrement force value
					if ((table0.Item.SensitivitySet == 0)||(table0.Item.SensitivitySet > MAX_SENSITIVITY_SET))
					{
						table0.Item.SensitivitySet = MAX_SENSITIVITY_SET;
					}
					else
					{
						table0.Item.SensitivitySet--;
					}
					ConfigUpdate(table0.Item.SensitivitySet,SensitivitySetting);
					AppScreenPlaceSensitivitySet(table0.Item.SensitivitySet);
				}
				else
				{
					if (key == KEY_RIGHT)
					{
						//-------------------
						// RIGHT key released
						// increment force value
						if ((table0.Item.SensitivitySet >= MAX_SENSITIVITY_SET))
						{
							table0.Item.SensitivitySet = 0;
						}
						else
						{
							table0.Item.SensitivitySet++;
						}
						ConfigUpdate(table0.Item.SensitivitySet,SensitivitySetting);
						AppScreenPlaceSensitivitySet(table0.Item.SensitivitySet);
					}
				}
			}			
			break;
		}		
	}
}

void AppScreenPlaceHybrid(uint8_t value)
{
	uint8_t offset,tempData[6],*ptr,i;
	uint8_t *wordList[20];
	uint8_t j;
 
	offset = 55;
	tempData[5] = 0x00;

	if (value == FALSE)
	{		 
		wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		wordList[3] = (uint8_t *)FONTSMALL_BLANK;
		wordList[4] = (uint8_t *)FONTSMALL_BLANK;
		wordList[5] = (uint8_t *)FONTSMALL_BLANK;
		for (j=0;j<4;j++)
		{
			ptr = wordList[j];
			for (i=0;i<5;i++)
			{
				tempData[i] = *ptr++;
				
			}
			LCDPlaceData(offset,24,tempData,6);
			offset+=6;
		}
	}
	else
	{
		wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
		wordList[2] = (uint8_t *)FONTSMALL_BLANK;
		wordList[3] = (uint8_t *)FONTSMALL_BLANK;
		wordList[4] = (uint8_t *)FONTSMALL_BLANK;
		wordList[5] = (uint8_t *)FONTSMALL_BLANK;
		for (j=0;j<4;j++)
		{
			ptr = wordList[j];
			for (i=0;i<5;i++)
			{
				tempData[i] = *ptr++;				
			}
			LCDPlaceData(offset,24,tempData,6);
			offset+=6;
		}		
	}
}

void AppScreenPlaceBrakeBacklight(uint8_t value)
{
	uint8_t offset,tempData[6],*ptr,i;
	uint8_t *wordList[20];
	uint8_t j;
 
	offset = 65;
	tempData[5] = 0x00;

	if (value == FALSE)
	{		 
		wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		wordList[3] = (uint8_t *)FONTSMALL_BLANK;
		wordList[4] = (uint8_t *)FONTSMALL_BLANK;
		wordList[5] = (uint8_t *)FONTSMALL_BLANK;
		for (j=0;j<4;j++)
		{
			ptr = wordList[j];
			for (i=0;i<5;i++)
			{
				tempData[i] = *ptr++;
				
			}
			LCDPlaceData(offset,24,tempData,6);
			offset+=6;
		}
	}
	else
	{
		wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
		wordList[2] = (uint8_t *)FONTSMALL_BLANK;
		wordList[3] = (uint8_t *)FONTSMALL_BLANK;
		wordList[4] = (uint8_t *)FONTSMALL_BLANK;
		wordList[5] = (uint8_t *)FONTSMALL_BLANK;
		for (j=0;j<4;j++)
		{
			ptr = wordList[j];
			for (i=0;i<5;i++)
			{
				tempData[i] = *ptr++;				
			}
			LCDPlaceData(offset,24,tempData,6);
			offset+=6;
		}		
	}
}

void AppScreenPlaceSensitivitySet(uint8_t value)
{
	
		if ((value >= 0) && (value <= MAX_SENSITIVITY_SET))
		{
			LCDPlaceData(56,24,FONTLARGE[value][TOP],11);
			LCDPlaceData(56,32,FONTLARGE[value][MID],11);
			LCDPlaceData(56,40,FONTLARGE[value][BOT],11);
		}
}

void AppScreenPlaceMaxForceSet(uint8_t value)
{
	
		if ((value >= 0) && (value <= MAX_FORCE_SET))
		{
			LCDPlaceData(56,24,FONTLARGE[value][TOP],11);
			LCDPlaceData(56,32,FONTLARGE[value][MID],11);
			LCDPlaceData(56,40,FONTLARGE[value][BOT],11);
		}
}


void AppScreenSetupKey(uint8_t key)
{
	switch(key)
	{
		case KEY_CENTER:
		{
			switch(currSetupOffset)
			{
				case 0:
				{
					//-------------------
					// CENTER key released 
					if (right_pressed != 0)
					{
						AppScreenInit(SCREEN_DOWNLOADREQUEST);
					}
					else
					{
						AppScreenInit(SCREEN_HOME);
					}
					break;
				}
				case 2:
				{
					AppScreenInit(SCREEN_SETUPPAIR);	
					break;
				}
				case 3:
				{
					AppScreenInit(SCREEN_SETUPBRAKEBACKLIGHT);
					AppScreenPlaceBrakeBacklight(table0.Item.BrakeBacklight);
					break;
				}					
				case 6:
				{
					AppScreenInit(SCREEN_SETUPHYBRID);
					AppScreenPlaceHybrid(table0.Item.Hybrid);
					break;
				}		
				case 4:
				{
					AppScreenInit(SCREEN_SETUPMAXFORCE);
					AppScreenPlaceMaxForceSet(table0.Item.ForceMaxSet);
					break;
				}		
				case 1:
				{
					AppScreenInit(SCREEN_SETUPSENSITIVITY);
					AppScreenPlaceSensitivitySet(table0.Item.SensitivitySet);
					break;
				}						
				default:
				{
					AppScreenInit(SCREEN_SETUPRESET);
//				 	AppScreenSetupHybrid(table0.Item.Hybrid,FALSE,79,24);		
					AppScreenSetupForceMaxSet(table0.Item.ForceMaxSet,FALSE,79);		
//				 	AppScreenSetupHybrid(FALSE,FALSE,110,24);		
					AppScreenSetupForceMaxSet(7,FALSE,110);		
					AppScreenSetupMaxForceSet(table0.Item.MaxForce,FALSE,79);				
					AppScreenSetupMaxForceSet(5,FALSE,110);		
					AppScreenSetupSensitivitySet(table0.Item.SensitivitySet,FALSE,79);			
					AppScreenSetupSensitivitySet(9,FALSE,110);										
					changeToDefault = FALSE; 				
					break;
				}								
			}
			break;
		}
		case KEY_LEFT:
		{
			prevSetupOffset = currSetupOffset;
			if (currSetupOffset ==0)
			{
				currSetupOffset = (MAXSETUPOFFSET-1);
			}
			else
			{
				currSetupOffset--;
			}
//			refreshScreen(ScreenSetupNew,1);				
			AppScreenSetupLine(currSetupOffset,TRUE); 	
			AppScreenSetupLine(prevSetupOffset,FALSE); 			
			break;
		}
		case KEY_RIGHT:
		{
			prevSetupOffset = currSetupOffset;
			currSetupOffset++;
			if (currSetupOffset >= MAXSETUPOFFSET)
			{
				currSetupOffset = 0;
			}
//			refreshScreen(ScreenSetupNew,1);				
			AppScreenSetupLine(currSetupOffset,TRUE); 
			AppScreenSetupLine(prevSetupOffset,FALSE); 	
			break;
		}				
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppScreenSetupLine(uint8_t linetosetup, uint8_t reverse)
{
	uint8_t temp,temp2,*wordList[20];
	uint8_t offset,i,j,tempData[6],*ptr;
	 //-----------------------------
	 // erase line at previous offset
	 // place line at new offset
	 switch (linetosetup)
	 {
		 case 0:
		 {
//			refreshScreen(ScreenSetupNew,1);	
			offset = 2;
			tempData[5] = 0x00;
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLD];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[4] = (uint8_t *)FONTSMALL_BLANK;			
			wordList[5] = (uint8_t *)FONTSMALL_BLANK;
			for (j=0;j<4;j++)
			{
				ptr = wordList[j];
				for (i=0;i<5;i++)
 				{
					 tempData[i] = *ptr++;
					 if (reverse == TRUE)
					 {
						tempData[i] ^= 0xff;  
						tempData[5] = 0xFF; 
					 }
			 
 				}	 		
				LCDPlaceData(offset,0,tempData,6);				 
				offset+=6;
			}				
			break;
		 }
		 case 2:
		 {
//			refreshScreen(ScreenSetupNew,1);	
			offset = 2;
			tempData[5] = 0x00;
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLP];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLI];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
			wordList[4] = (uint8_t *)FONTSMALL_BLANK;			
			wordList[5] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
			wordList[6] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[7] = (uint8_t *)FONTSMALL[OFFSET_SMALLM];
			wordList[8] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
			wordList[9] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
			wordList[10] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];			
			for (j=0;j<11;j++)
			{
				ptr = wordList[j];
				for (i=0;i<5;i++)
 				{
					 tempData[i] = *ptr++;
					 if (reverse == TRUE)
					 {
						tempData[i] ^= 0xff;  
						tempData[5] = 0xFF; 
					 }
			 
 				}	 		
				LCDPlaceData(offset,16,tempData,6);				 
				offset+=6;
			}		
			temp = table0.Item.PairAddress[0];
			temp2 = temp & 0x0f;
			temp = temp>>4;
			LCDPlaceData(85,16,(uint8_t *)FONTSMALL[temp],5);
			LCDPlaceData(91,16,(uint8_t *)FONTSMALL[temp2],5);
			temp = table0.Item.PairAddress[1];
			temp2 = temp & 0x0f;
			temp = temp>>4;
			LCDPlaceData(97,16,(uint8_t *)FONTSMALL[temp],5);
			LCDPlaceData(103,16,(uint8_t *)FONTSMALL[temp2],5);
			
			
			break;
		 }		 
		 case 6:
		 {
//			refreshScreen(ScreenSetupNew,1);
			offset = 2;
			tempData[5] = 0x00;
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLH];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLY];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLB];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
			wordList[4] = (uint8_t *)FONTSMALL[OFFSET_SMALLI];
			wordList[5] = (uint8_t *)FONTSMALL[OFFSET_SMALLD];
			wordList[6] = (uint8_t *)FONTSMALL_BLANK;
			wordList[7] = (uint8_t *)FONTSMALL_BLANK;
			wordList[8] = (uint8_t *)FONTSMALL_BLANK;
			wordList[9] = (uint8_t *)FONTSMALL_BLANK;
			wordList[10] = (uint8_t *)FONTSMALL_BLANK;
			for (j=0;j<11;j++)
			{
				ptr = wordList[j];
				for (i=0;i<5;i++)
				{
					tempData[i] = *ptr++;
					if (reverse == TRUE)
					{
						tempData[i] ^= 0xff;
						tempData[5] = 0xFF;
					}
					
				}
				LCDPlaceData(offset,8,tempData,6);
				offset+=6;
			}
			AppScreenSetupHybrid(table0.Item.Hybrid,reverse,85,8);				
			break;
		 }
		 case 3:
		 {
			offset = 2;
			tempData[5] = 0x00;
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLB];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLK];
			wordList[4] = (uint8_t *)FONTSMALL[OFFSET_SMALLI];
			wordList[5] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
			wordList[6] = (uint8_t *)FONTSMALL[OFFSET_SMALLG];			
			wordList[7] = (uint8_t *)FONTSMALL_BLANK;			
			wordList[8] = (uint8_t *)FONTSMALL[OFFSET_SMALLB];
			wordList[9] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
			wordList[10] = (uint8_t *)FONTSMALL[OFFSET_SMALLC];
			wordList[11] = (uint8_t *)FONTSMALL[OFFSET_SMALLK];
			wordList[12] = (uint8_t *)FONTSMALL[OFFSET_SMALLL];
			wordList[13] = (uint8_t *)FONTSMALL[OFFSET_SMALLI];			
			wordList[14] = (uint8_t *)FONTSMALL[OFFSET_SMALLG];
			wordList[15] = (uint8_t *)FONTSMALL[OFFSET_SMALLH];
			wordList[16] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];		
			for (j=0;j<17;j++)
			{
				ptr = wordList[j];
				for (i=0;i<5;i++)
 				{
					 tempData[i] = *ptr++;
					 if (reverse == TRUE)
					 {
						tempData[i] ^= 0xff;  
						tempData[5] = 0xFF; 
					 }
			 
 				}	 		
				LCDPlaceData(offset,24,tempData,6);				 
				offset+=6;
			}		
			AppScreenSetupBrakeBacklight(table0.Item.BrakeBacklight,reverse,108);	
			break;
		 }		 		 
		 case 4:
		 {
//			refreshScreen(ScreenSetupNew,1);
			offset = 2;
			tempData[5] = 0x00;
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLM];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLX];
			wordList[3] = (uint8_t *)FONTSMALL_BLANK;
			wordList[4] = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
			wordList[5] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
			wordList[6] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
			wordList[7] = (uint8_t *)FONTSMALL[OFFSET_SMALLC];
			wordList[8] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			for (j=0;j<9;j++)
			{
				ptr = wordList[j];
				for (i=0;i<5;i++)
				{
					tempData[i] = *ptr++;
					if (reverse == TRUE)
					{
						tempData[i] ^= 0xff;
						tempData[5] = 0xFF;
					}
					
				}
				LCDPlaceData(offset,32,tempData,6);
				offset+=6;
			} 
			AppScreenSetupForceMaxSet(table0.Item.ForceMaxSet,reverse,85);	
			break;
		 }		 
		 case 1:
		 {
//			refreshScreen(ScreenSetupNew,1);
			offset = 2;
			tempData[5] = 0x00;
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLS];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLS];
			wordList[4] = (uint8_t *)FONTSMALL[OFFSET_SMALLI];
			wordList[5] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
			wordList[6] = (uint8_t *)FONTSMALL[OFFSET_SMALLI];
			wordList[7] = (uint8_t *)FONTSMALL[OFFSET_SMALLV];
			wordList[8] = (uint8_t *)FONTSMALL[OFFSET_SMALLI];
			wordList[9] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
			wordList[10] = (uint8_t *)FONTSMALL[OFFSET_SMALLY];			
			for (j=0;j<11;j++)
			{
				ptr = wordList[j];
				for (i=0;i<5;i++)
				{
					tempData[i] = *ptr++;
					if (reverse == TRUE)
					{
						tempData[i] ^= 0xff;
						tempData[5] = 0xFF;
					}
					
				}
				LCDPlaceData(offset,8,tempData,6);
				offset+=6;
			} 
			AppScreenSetupSensitivity(table0.Item.SensitivitySet,reverse,85);	
			break;
		 }		 		 		 		 
		 default:
		 {
//			refreshScreen(ScreenSetupNew,1);
			offset = 2;
			tempData[5] = 0x00;
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLS];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[4] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];			
			wordList[5] = (uint8_t *)FONTSMALL_BLANK;
			wordList[6] = (uint8_t *)FONTSMALL[OFFSET_SMALLS];
			wordList[7] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[8] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
			wordList[9] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
			wordList[10] = (uint8_t *)FONTSMALL[OFFSET_SMALLI];
			wordList[11] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
			wordList[12] = (uint8_t *)FONTSMALL[OFFSET_SMALLG];
			wordList[13] = (uint8_t *)FONTSMALL[OFFSET_SMALLS];						
			for (j=0;j<14;j++)
			{
				ptr = wordList[j];
				for (i=0;i<5;i++)
				{
					tempData[i] = *ptr++;
					if (reverse == TRUE)
					{
						tempData[i] ^= 0xff;
						tempData[5] = 0xFF;
					}
					
				}
				LCDPlaceData(offset,40,tempData,6);
				offset+=6;
			}
			 break;
		 }
	 }
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppScreenSetupHybrid(uint8_t value,uint8_t reverse,uint8_t newoffset,uint8_t line)
{
	uint8_t offset,tempData[6],*ptr,i;
	uint8_t *wordList[20];
	uint8_t j;
 
	offset = newoffset;
	tempData[5] = 0x00;

	if (value == FALSE)
	{		 
		wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		wordList[3] = (uint8_t *)FONTSMALL_BLANK;
		wordList[4] = (uint8_t *)FONTSMALL_BLANK;
		wordList[5] = (uint8_t *)FONTSMALL_BLANK;
		for (j=0;j<4;j++)
		{
			ptr = wordList[j];
			for (i=0;i<5;i++)
			{
				tempData[i] = *ptr++;
				if (reverse == TRUE)
				{
					tempData[i] ^= 0xff;
					tempData[5] = 0xFF;
				}
				
			}
			LCDPlaceData(offset,line,tempData,6);
			offset+=6;
		}
	}
	else
	{
		wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
		wordList[2] = (uint8_t *)FONTSMALL_BLANK;
		wordList[3] = (uint8_t *)FONTSMALL_BLANK;
		wordList[4] = (uint8_t *)FONTSMALL_BLANK;
		wordList[5] = (uint8_t *)FONTSMALL_BLANK;
		for (j=0;j<4;j++)
		{
			ptr = wordList[j];
			for (i=0;i<5;i++)
			{
				tempData[i] = *ptr++;
				if (reverse == TRUE)
				{
					tempData[i] ^= 0xff;
					tempData[5] = 0xFF;
				}
				
			}
			LCDPlaceData(offset,line,tempData,6);
			offset+=6;
		}		
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppScreenSetupBrakeBacklight(uint8_t value,uint8_t reverse,uint8_t newoffset)
{
	uint8_t offset,tempData[6],*ptr,i;
	uint8_t *wordList[20];
	uint8_t j;
	
	offset = newoffset;
	tempData[5] = 0x00;

	if (value == FALSE)
	{
		wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		wordList[3] = (uint8_t *)FONTSMALL_BLANK;
		wordList[4] = (uint8_t *)FONTSMALL_BLANK;
		wordList[5] = (uint8_t *)FONTSMALL_BLANK;
		for (j=0;j<4;j++)
		{
			ptr = wordList[j];
			for (i=0;i<5;i++)
			{
				tempData[i] = *ptr++;
				if (reverse == TRUE)
				{
					tempData[i] ^= 0xff;
					tempData[5] = 0xFF;
				}
				
			}
			LCDPlaceData(offset,24,tempData,6);
			offset+=6;
		}
	}
	else
	{
		wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
		wordList[2] = (uint8_t *)FONTSMALL_BLANK;
		wordList[3] = (uint8_t *)FONTSMALL_BLANK;
		wordList[4] = (uint8_t *)FONTSMALL_BLANK;
		wordList[5] = (uint8_t *)FONTSMALL_BLANK;
		for (j=0;j<4;j++)
		{
			ptr = wordList[j];
			for (i=0;i<5;i++)
			{
				tempData[i] = *ptr++;
				if (reverse == TRUE)
				{
					tempData[i] ^= 0xff;
					tempData[5] = 0xFF;
				}
				
			}
			LCDPlaceData(offset,24,tempData,6);
			offset+=6;
		}
	}
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppScreenSetupSensitivity(uint8_t value,uint8_t reverse,uint8_t newoffset)
{
	uint8_t offset,tempData[6],*ptr,i,temp;
	
	offset = newoffset;
	temp = value;
	temp = temp & 0x0f;
	
	tempData[5] = 0x00;
	
	ptr = (uint8_t *)FONTSMALL[temp];
	for (i=0;i<5;i++)
	{
		tempData[i] = *ptr++;
		if (reverse == TRUE)
		{
			tempData[i] ^= 0xff;
			tempData[5] = 0xFF;
		}
		
	}
	LCDPlaceData(offset,8,tempData,6);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppScreenSetupSensitivitySet(uint8_t state,uint8_t reverse,uint8_t newoffset)
{
	uint8_t offset,tempData[6],*ptr,i,temp;
	
	offset = newoffset;
	temp = state;
	temp = temp & 0x0f;
	
	tempData[5] = 0x00;
	
	ptr = (uint8_t *)FONTSMALL[temp];
	for (i=0;i<5;i++)
	{
		tempData[i] = *ptr++;
		if (reverse == TRUE)
		{
			tempData[i] ^= 0xff;
			tempData[5] = 0xFF;
		}
		
	}
	LCDPlaceData(offset,40,tempData,6);
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppScreenSetupMaxForceSet(uint8_t state,uint8_t reverse,uint8_t newoffset)
{
	uint8_t offset,tempData[6],*ptr,i,temp;
	
	offset = newoffset;
	temp = state;
	temp = temp & 0x0f;
	
	tempData[5] = 0x00;
	
	ptr = (uint8_t *)FONTSMALL[temp];
	for (i=0;i<5;i++)
	{
		tempData[i] = *ptr++;
		if (reverse == TRUE)
		{
			tempData[i] ^= 0xff;
			tempData[5] = 0xFF;
		}
		
	}
	LCDPlaceData(offset,24,tempData,6);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppScreenSetupForceMaxSet(uint8_t state,uint8_t reverse,uint8_t newoffset)
{
	uint8_t offset,tempData[6],*ptr,i,temp;
	
	offset = newoffset;
	temp = state;
	temp = temp & 0x0f;
	
	tempData[5] = 0x00;
	
	ptr = (uint8_t *)FONTSMALL[temp];
	for (i=0;i<5;i++)
	{
		tempData[i] = *ptr++;
		if (reverse == TRUE)
		{
			tempData[i] ^= 0xff;
			tempData[5] = 0xFF;
		}
		
	}
	LCDPlaceData(offset,32,tempData,6);
}




//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function 
// 
//==============================================================================
uint8_t AppPairingActive(void)
{
	return pairActive; 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function 
// 
//==============================================================================
void SetPairActiveOff(void)
{
	uint8_t temp,temp2;
	pairActive = FALSE; 
//OLD	AppLCDConifgMiscSelectDeselect(6,FALSE); 	
				temp = table0.Item.PairAddress[0];
				temp2 = temp & 0x0f;
				temp = temp>>4;
				LCDPlaceData(95,32,(uint8_t *)FONTSMALL[temp],5);
				LCDPlaceData(101,32,(uint8_t *)FONTSMALL[temp2],5);
				temp = table0.Item.PairAddress[1];
				temp2 = temp & 0x0f;
				temp = temp>>4;
				LCDPlaceData(107,32,(uint8_t *)FONTSMALL[temp],5);
				LCDPlaceData(113,32,(uint8_t *)FONTSMALL[temp2],5);				
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppLCDConfigMisc(uint8_t key)
{
	uint8_t last,testBuffer[2];
	
	last = configMiscOffset;
	switch (key)
	{
		case CONFIG_MISC_UP:
		{
		
			if (configMiscState == CONFIGMISC_IDLE)
			{
				configMiscOffset++;
				if (configMiscOffset >= 12)
				{
					configMiscOffset = 2;
				}
				if (configMiscOffset == 1)
				{
					configMiscOffset = 2; 
				}
				if (last != 0)
				{
					ConfigMiscSelect(last,1);
				}
				ConfigMiscSelect(configMiscOffset,0);
			}
			else
			{
				if ((configMiscState == CONFIGMISC_SELECT)||(configMiscState == CONFIGMISC_ROTATE))
				{
					configMiscState = CONFIGMISC_IDLE;
					if (configMiscOffset >0)
					{
						AppLCDConifgMiscSelectDeselect(configMiscOffset,FALSE); 
					}
					configMiscOffset++;
					if (configMiscOffset >= 12)
					{
						configMiscOffset = 2;
					}
					if (last != 0)
					{
						ConfigMiscSelect(last,1);
					}
					ConfigMiscSelect(configMiscOffset,0);
				}
			}
			break;
		}
		case CONFIG_MISC_SELECT:
		{
			if (configMiscState == CONFIGMISC_IDLE)
			{
				//----- check if this spot is open ....
				
				if (configMiscOffset >0)
				{
					configMiscState = CONFIGMISC_SELECT;
					AppLCDConifgMiscSelectDeselect(configMiscOffset,TRUE); 
				}
			}
			else
			{
				if ((configMiscState == CONFIGMISC_SELECT)||(configMiscState == CONFIGMISC_ROTATE))
				{
					configMiscState = CONFIGMISC_IDLE;
					if (configMiscOffset >0)
					{
						AppLCDConifgMiscSelectDeselect(configMiscOffset,FALSE); 
					}
				}
			}
			break;
		}
		case CONFIG_MISC_ROTATE:
		{
			if ((configMiscState == CONFIGMISC_SELECT)||(configMiscState == CONFIGMISC_ROTATE))
			{
				//----- check if this spot is open ....
				
				if (configMiscOffset >0)
				{
					configMiscState = CONFIGMISC_ROTATE;
					switch(configMiscOffset)
					{
						
						case 2:
						{
							if (speakerActive == TRUE)
							{
								speakerActive = FALSE; 
								SpeakerOff();
							}
							else
							{
								speakerActive = TRUE; 
								SpeakerOn();
							}				 					
							break;
						}				
						case 3:
						{
							
							table0.Item.ScreenColor++;
							if (table0.Item.ScreenColor >= MAX_SCREENCOLOR)
							{
								table0.Item.ScreenColor = 0;
							}
							BacklightSetColor(table0.Item.ScreenColor);
							testBuffer[0] = table0.Item.ScreenColor;
							I2CEEPROMBufferWrite(testBuffer,ScreenColorSetting,1);								
							break;
						}		
						case 4:
						{	
							//--------------backlight can be TRUE/FALSE
							if (table0.Item.BackLightOn == TRUE)
							{
								table0.Item.BackLightOn = FALSE; 
								testBuffer[0] = FALSE;
								I2CEEPROMBufferWrite(testBuffer,BackLightOnSetting,1);	
								BacklightSet(table0.Item.BackLightOn);							
							}
							else
							{
								table0.Item.BackLightOn = TRUE; 
								testBuffer[0] = TRUE;
								I2CEEPROMBufferWrite(testBuffer,BackLightOnSetting,1);	
								BacklightSet(table0.Item.BackLightOn);															
							}						
							break;
						}
						case 5:
						{
							if (table0.Item.TempFarenheitOn == TRUE)
							{
								table0.Item.TempFarenheitOn = FALSE; 
								testBuffer[0] = FALSE;
								I2CEEPROMBufferWrite(testBuffer,TempFarenheitOnSetting,1);									
							}
							else
							{
								table0.Item.TempFarenheitOn = TRUE; 
								testBuffer[0] = TRUE;
								I2CEEPROMBufferWrite(testBuffer,TempFarenheitOnSetting,1);							
							}													
							break;
						}
						case 6:
						{
							if (pairActive == TRUE)
							{
								pairActive = FALSE; 
							}
							else
							{
								pairActive = TRUE; 
							}				 					
							break;
						}
						case 7:
						{
							
							table0.Item.ForceMaxSet++;
							if (table0.Item.ForceMaxSet >= 10)
							{
								table0.Item.ForceMaxSet = 1;
							}
							testBuffer[0] = table0.Item.ForceMaxSet;
							I2CEEPROMBufferWrite(testBuffer,ForceMaxSetting,1);
							break;
						}	
						case 8:
						{
							if (table0.Item.ActiveBrakeEnable == TRUE)
							{
								table0.Item.ActiveBrakeEnable = FALSE;
								testBuffer[0] = FALSE;
								I2CEEPROMBufferWrite(testBuffer,ActiveBrakeEnableSetting,1);
							}
							else
							{
								table0.Item.ActiveBrakeEnable = TRUE;
								testBuffer[0] = TRUE;
								I2CEEPROMBufferWrite(testBuffer,ActiveBrakeEnableSetting,1);
							}
							break;
						}		
						case 9:
						{
							if (table0.Item.TPMSEnable == TRUE)
							{
								table0.Item.TPMSEnable = FALSE;
								testBuffer[0] = FALSE;
								I2CEEPROMBufferWrite(testBuffer,TPMSEnableSetting,1);
							}
							else
							{
								table0.Item.TPMSEnable = TRUE;
								testBuffer[0] = TRUE;
								I2CEEPROMBufferWrite(testBuffer,TPMSEnableSetting,1);
							}
							break;
						}															
					}
					AppLCDConifgMiscSelectDeselect(configMiscOffset,TRUE); 
				}
			}
			break;
		}		
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
 void AppLCDConfigMiscHandle(void)
 {
	uint8_t whichOne;

	//-------------------------------
	// which button changed.
	// 1. if LEFT
	// 2. if CENTER goes from pressed to depressed - go to Home menu
	// 3. if RIGHT
	//-------------------------------
	whichOne = ButtonChanged();
	if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
	{
		pairActive = FALSE; 
		speakerActive = FALSE; 
		SpeakerOff();
			//-------------------
			// CENTER key released
		AppScreenInit(SCREEN_MAIN);
	}
	else
	{
		if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
		{
			//UP
			AppLCDConfigMisc(CONFIG_MISC_UP);
		}
		else
		{
			if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
			{
				AppLCDConfigMisc(CONFIG_MISC_SELECT);
			}
			else
			{
				if (((whichOne & KEY_BRAKE)!=0)&&(brake_pressed == 0))
				{
					//DELETE
					AppLCDConfigMisc(CONFIG_MISC_ROTATE);
				}
			}
		}
	}	 
 }
 
 
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
 void AppLCDConfigMiscInit(void)
 {
	pairActive = FALSE; 
	speakerActive = FALSE; 
	configMiscState = CONFIGMISC_IDLE;
	AppLCDConfigMiscPair(pairActive,FALSE); 
	AppLCDConfigMiscScolor(table0.Item.ScreenColor,FALSE);
	AppLCDConfigMiscBlight(table0.Item.BackLightOn,FALSE);
	AppLCDConfigMiscTemp(table0.Item.TempFarenheitOn,FALSE);	
	AppLCDConfigMiscSpeaker(speakerActive,FALSE);		
	AppLCDConfigMiscForceMaxSet(table0.Item.ForceMaxSet,FALSE);
	AppLCDConfigMiscActiveBrakeEnable(table0.Item.ActiveBrakeEnable,FALSE);
	AppLCDConfigMiscTPMSEnable(table0.Item.TPMSEnable,FALSE);
 }

void AppLCDConifgMiscSelectDeselect(uint8_t which,uint8_t state)
{
	switch (which)
	{
		case 2:
		{
			AppLCDConfigMiscSpeaker(speakerActive,state); 
			break;
		}		
		case 3:
		{
			AppLCDConfigMiscScolor(table0.Item.ScreenColor,state);							
			break;
		}		
		case 4:
		{
			AppLCDConfigMiscBlight(table0.Item.BackLightOn,state);							
			break;
		}
		case 5:
		{
			AppLCDConfigMiscTemp(table0.Item.TempFarenheitOn,state);
			break;
		}
		case 6:
		{
			AppLCDConfigMiscPair(pairActive,state); 
			break;
		}
		case 7:
		{
			AppLCDConfigMiscForceMaxSet(table0.Item.ForceMaxSet,state);
			break;
		}
		case 8:
		{
			AppLCDConfigMiscActiveBrakeEnable(table0.Item.ActiveBrakeEnable,state);
			break;
		}
		case 9:
		{
			AppLCDConfigMiscTPMSEnable(table0.Item.TPMSEnable,state);	
			break;
		}
	}	
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppLCDConfigMiscForceMaxSet(uint8_t state,uint8_t reverse)
{
	uint8_t offset,tempData[6],*ptr,i,temp;
	
	offset = 110;
	temp = table0.Item.ForceMaxSet;
	temp = temp & 0x0f;
 
	tempData[5] = 0x00;
 
	ptr = (uint8_t *)FONTSMALL[temp];
	for (i=0;i<5;i++)
	{
		tempData[i] = *ptr++;
		if (reverse == TRUE)
		{
			tempData[i] ^= 0xff;
			tempData[5] = 0xFF;
		}
		
	}
	LCDPlaceData(offset,8,tempData,6);	 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppLCDConfigMiscActiveBrakeEnable(uint8_t state,uint8_t reverse)
{
	uint8_t offset,i,tempData[6],*ptr;
	offset = 110;
	tempData[5] = 0x00;
	if (state == TRUE)
	{
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		for (i=0;i<5;i++)
		{
			tempData[i] = *ptr++;
			if (reverse == TRUE)
			{
				tempData[i] ^= 0xff;
				tempData[5] = 0xFF;
			}
			
		}
		LCDPlaceData(offset,16,tempData,6);
		offset+=6;
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
		for (i=0;i<5;i++)
		{
			tempData[i] = *ptr++;
			if (reverse == TRUE)
			{
				tempData[i] ^= 0xff;
			}
			
		}
		LCDPlaceData(offset,16,tempData,6);
		offset+= 6;
		ptr = (uint8_t *)FONTSMALL_BLANK;
		for (i=0;i<5;i++)
		{
			tempData[i] = *ptr++;
			if (reverse == TRUE)
			{
				tempData[i] ^= 0xff;
			}
			
		}
		LCDPlaceData(offset,16,tempData,6);
	}
	else
	{
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		for (i=0;i<5;i++)
		{
			tempData[i] = *ptr++;
			if (reverse == TRUE)
			{
				tempData[i] ^= 0xff;
				tempData[5] = 0xFF;
			}
			
		}
		LCDPlaceData(offset,16,tempData,6);
		offset+=6;
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		for (i=0;i<5;i++)
		{
			tempData[i] = *ptr++;
			if (reverse == TRUE)
			{
				tempData[i] ^= 0xff;
			}
			
		}
		LCDPlaceData(offset,16,tempData,6);
		offset+= 6;
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		for (i=0;i<5;i++)
		{
			tempData[i] = *ptr++;
			if (reverse == TRUE)
			{
				tempData[i] ^= 0xff;
			}
			
		}
		LCDPlaceData(offset,16,tempData,6);
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppLCDConfigMiscTPMSEnable(uint8_t state,uint8_t reverse)
{
	uint8_t offset,i,tempData[6],*ptr;
	offset = 110;
	tempData[5] = 0x00;
	if (state == TRUE)
	{
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		for (i=0;i<5;i++)
		{
			tempData[i] = *ptr++;
			if (reverse == TRUE)
			{
				tempData[i] ^= 0xff;
				tempData[5] = 0xFF;
			}
			
		}
		LCDPlaceData(offset,24,tempData,6);
		offset+=6;
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
		for (i=0;i<5;i++)
		{
			tempData[i] = *ptr++;
			if (reverse == TRUE)
			{
				tempData[i] ^= 0xff;
			}
			
		}
		LCDPlaceData(offset,24,tempData,6);
		offset+= 6;
		ptr = (uint8_t *)FONTSMALL_BLANK;
		for (i=0;i<5;i++)
		{
			tempData[i] = *ptr++;
			if (reverse == TRUE)
			{
				tempData[i] ^= 0xff;
			}
			
		}
		LCDPlaceData(offset,24,tempData,6);
	}
	else
	{
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		for (i=0;i<5;i++)
		{
			tempData[i] = *ptr++;
			if (reverse == TRUE)
			{
				tempData[i] ^= 0xff;
				tempData[5] = 0xFF;
			}
			
		}
		LCDPlaceData(offset,24,tempData,6);
		offset+=6;
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		for (i=0;i<5;i++)
		{
			tempData[i] = *ptr++;
			if (reverse == TRUE)
			{
				tempData[i] ^= 0xff;
			}
			
		}
		LCDPlaceData(offset,24,tempData,6);
		offset+= 6;
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		for (i=0;i<5;i++)
		{
			tempData[i] = *ptr++;
			if (reverse == TRUE)
			{
				tempData[i] ^= 0xff;
			}
			
		}
		LCDPlaceData(offset,24,tempData,6);
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppLCDConfigMiscScolor(uint8_t state,uint8_t reverse)
{
	uint8_t offset,i,*ptr,tempData[6];
	uint8_t *wordList[10],j; 
	
	offset = 38;
	tempData[5] = 0x00; 
	switch(state)
	{
		case 1:
		{
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLG];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[4] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
			wordList[5] = (uint8_t *)FONTSMALL_BLANK;			
			break;
		}
		case 2:
		{
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLB];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLL];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLU];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[4] = (uint8_t *)FONTSMALL_BLANK;			
			wordList[5] = (uint8_t *)FONTSMALL_BLANK;			
			break;
		}	
		case 3:
		{
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLQ];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLU];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
			wordList[4] = (uint8_t *)FONTSMALL_BLANK;			
			wordList[5] = (uint8_t *)FONTSMALL_BLANK;			
			break;
		}			
		case 4:
		{
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLD];
			wordList[3] = (uint8_t *)FONTSMALL_BLANK;
			wordList[4] = (uint8_t *)FONTSMALL_BLANK;			
			wordList[5] = (uint8_t *)FONTSMALL_BLANK;			
			break;
		}	
		case 5:
		{
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLY];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLL];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLL];
			wordList[4] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
			wordList[5] = (uint8_t *)FONTSMALL[OFFSET_SMALLW];	
			break;
		}		
		case 6:
		{
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLP];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLU];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLP];
			wordList[4] = (uint8_t *)FONTSMALL[OFFSET_SMALLL];
			wordList[5] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			break;
		}		
		case 7:
		{
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLW];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLH];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLI];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
			wordList[4] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[5] = (uint8_t *)FONTSMALL_BLANK;			
			break;
		}						
		default:
		{
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[4] = (uint8_t *)FONTSMALL_BLANK;			
			wordList[5] = (uint8_t *)FONTSMALL_BLANK;
			break;
		}			
		
	}
			for (j=0;j<6;j++)
			{
				ptr = wordList[j];
				for (i=0;i<5;i++)
 				{
					 tempData[i] = *ptr++;
					 if (reverse == TRUE)
					 {
						tempData[i] ^= 0xff;  
						tempData[5] = 0xFF; 
					 }
			 
 				}	 		
				LCDPlaceData(offset,32,tempData,6);				 
				offset+=6;
			}	
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppLCDConfigMiscBlight(uint8_t state,uint8_t reverse)
{
	uint8_t offset,i,tempData[6],*ptr;
	offset = 38;
	tempData[5] = 0x00;
	if (state == TRUE)
	{
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
				tempData[5] = 0xFF; 
			 }
			 
 		}	 		
		LCDPlaceData(offset,24,tempData,6);
		offset+=6;
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
			 }
			 
 		}	 				
		LCDPlaceData(offset,24,tempData,6);
		offset+= 6;
		ptr = (uint8_t *)FONTSMALL_BLANK;
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
			 }
			 
 		}	 				
		LCDPlaceData(offset,24,tempData,6);
	}
	else
	{
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
				tempData[5] = 0xFF;
			 }
			 
 		}	 		
		LCDPlaceData(offset,24,tempData,6);
		offset+=6;
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
			 }
			 
 		}	 				
		LCDPlaceData(offset,24,tempData,6);
		offset+= 6;
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
			 }
			 
 		}	 				
		LCDPlaceData(offset,24,tempData,6);			
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppLCDConfigMiscTemp(uint8_t state,uint8_t reverse)
{
	uint8_t offset,i,tempData[6],*ptr;
	offset = 38;
	tempData[5] = 0x00;
	if (state == TRUE)  //true is Farenheit
	{
		ptr = (uint8_t *)FONTSMALL_DEGREE;
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
				tempData[5] = 0xFF;
			 }
			 
 		}	 		
		LCDPlaceData(offset,16,tempData,6);		
		offset+=6;
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
			 }
			 
 		}	 		
		LCDPlaceData(offset,16,tempData,6);		
	}
	else
	{
		ptr = (uint8_t *)FONTSMALL_DEGREE;
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
				tempData[5] = 0xFF;		
			 }
			 
 		}	 		
		LCDPlaceData(offset,16,tempData,6);		
		offset+=6;
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLC];
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
			 }
			 
 		}	 		
		LCDPlaceData(offset,16,tempData,6);
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
 void AppLCDConfigMiscPair(uint8_t state,uint8_t reverse)
 {
	uint8_t offset,tempData[6],*ptr,i,temp,temp2; 
	
	offset = 38;
	tempData[5] = 0x00;
	if (state == TRUE)
	{
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
				tempData[5] = 0xFF;
			 }
			 
 		}	 		
		LCDPlaceData(offset,8,tempData,6);
		offset+=6;
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
		for (i=0;i<6;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
			 }
			 
 		}	 				
		LCDPlaceData(offset,8,tempData,6);
		offset+= 6;
		ptr = (uint8_t *)FONTSMALL_BLANK;
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
			 }
			 
 		}	 				
		LCDPlaceData(offset,8,tempData,6);
	}
	else
	{
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
				tempData[5] = 0xFF;		
			 }
			 
 		}	 		
		LCDPlaceData(offset,8,tempData,6);
		offset+=6;
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
			 }
			 
 		}	 				
		LCDPlaceData(offset,8,tempData,6);
		offset+= 6;
		ptr = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
			 }
			 
 		}	 				
		LCDPlaceData(offset,8,tempData,6);			
	}
	//--------------------add pair address if it is 
//	LCDPlaceData(56,8,(uint8_t *)FONTSMALL_HYPEN,6);	
	
	temp = table0.Item.PairAddress[0];
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(56,8,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(62,8,(uint8_t *)FONTSMALL[temp2],5);	
	temp = table0.Item.PairAddress[1];
	temp2 = temp & 0x0f;
	temp = temp>>4;
//	LCDPlaceData(68,8,(uint8_t *)FONTSMALL[temp],5);
//	LCDPlaceData(74,8,(uint8_t *)FONTSMALL[temp2],5);	 
 }
 
 //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
 void AppLCDConfigMiscSpeaker(uint8_t state,uint8_t reverse)
 {
	uint8_t offset,tempData[6],*ptr,i; 
	uint8_t j,*wordList[10]; 
	
	offset = 43;
	tempData[5] = 0x00;
	
	switch(state)
	{
		case 1:
		{
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLL];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLW];
			wordList[3] = (uint8_t *)FONTSMALL_BLANK;
			wordList[4] = (uint8_t *)FONTSMALL_BLANK;			
			wordList[5] = (uint8_t *)FONTSMALL_BLANK;			
			break;
		}
		default:
		{
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLF];
			wordList[3] = (uint8_t *)FONTSMALL_BLANK;
			wordList[4] = (uint8_t *)FONTSMALL_BLANK;			
			wordList[5] = (uint8_t *)FONTSMALL_BLANK;						
			break;
		}
	}
	for (j=0;j<6;j++)
	{
		ptr = wordList[j];
		for (i=0;i<5;i++)
 		{
			 tempData[i] = *ptr++;
			 if (reverse == TRUE)
			 {
				tempData[i] ^= 0xff;  
				tempData[5] = 0xFF; 
			 }
	 
 		}	 		
		LCDPlaceData(offset,40,tempData,6);				 
		offset+=6;
	}
 }
 
 //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 // FUNCTION:
 //------------------------------------------------------------------------------
 // This function
 //
 //==============================================================================
 void ConfigMiscSelect(uint8_t which,uint8_t select)
 {
	uint8_t tempData[6],*ptr,i;
 
	 
	ptr = (uint8_t*)FONTSMALL_START;
	if (select != 0)
	{
		ptr = (uint8_t*)FONTSMALL_BLANK;
	}
	for (i=0;i<6;i++)
 	{
		 tempData[i] = *ptr++;
 	}	 
	if (which <7)
	{
		 LCDPlaceData(0,56-(which*8),tempData,6);
	}
	else
	{
		 LCDPlaceData(68,(which-6)*8,tempData,6);	 
	}
 }
 

 
 
#endif
