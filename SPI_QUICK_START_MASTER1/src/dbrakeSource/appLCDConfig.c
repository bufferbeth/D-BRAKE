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

#define CONFIG_DELETE	0
#define CONFIG_UP		1
#define CONFIG_DOWN		2
#define CONFIG_SELECT   3 
//---------------------GLOBAL VARIABLES-----------------------------------
 
extern const uint8_t SystemSensor[1024];

//---------------------LOCAL VARIABLES------------------------------------
//--------------------------
uint8_t configTireOffset; 

#define CONFIGTIRE_IDLE 0
#define CONFIGTIRE_SELECT 1
uint8_t configTireState; 
 
//---------------------LOCAL FUNCTION PROTOTYPES--------------------------
void AppLCDConfigTirePressure(uint8_t key);	  
void ConfigTireSelect(uint8_t which,uint8_t select);
void ConfigTirePlaceReading(uint8_t which,uint8_t *select,uint8_t position); 
void AppLCDConfigTireUpdate(void);
 
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function 
// 
//==============================================================================
void ConfigSensorTableClear(void)
{
	uint8_t i,offset; 
	for (i=0;i<MAXSENSORS;i++)
	{
		offset = i; 
 		//---------------------------
 		// place position in flash
 		ConfigUpdate(0xff,TableSensorStart + (offset*4));
 		ConfigUpdate(0xff,TableSensorStart + (offset*4)+1);
 		ConfigUpdate(0xff,TableSensorStart + (offset*4)+2);
 		ConfigUpdate(0xff,TableSensorStart + (offset*4)+3);
 		if (I2CEEPROMBufferRead((uint8_t*)&tableSensor.Item.WhichSensor[offset],TableSensorStart + (offset*4),4)!= 0)
 		{
				
 		}
	}
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppLCDConfigTireInit(void)
{
	uint8_t i,digits[3],offset,numDigits,j; 

	configTireState = CONFIGTIRE_IDLE;
 	configTireOffset = 0;

	for (i=0;i<MAXSENSORS;i++)
	{
		//------------------------
		// read table if the value is not 0xff 
		// place a 000 in the location. 
 		//---------------------------
		 
		 
		if (tableSensor.Item.WhichSensor[i].ID[0] != 0xff)
		{
			ConfigTirePlaceReading(i+1,(uint8_t *)FONTSMALL[0],0);
			ConfigTirePlaceReading(i+1,(uint8_t *)FONTSMALL[0],1);
			ConfigTirePlaceReading(i+1,(uint8_t *)FONTSMALL[0],2);	    
		}
		else
		{
			ConfigTirePlaceReading(i+1,(uint8_t *)FONTSMALL_FULL,0);
			ConfigTirePlaceReading(i+1,(uint8_t *)FONTSMALL_FULL,1);
			ConfigTirePlaceReading(i+1,(uint8_t *)FONTSMALL_FULL,2);	  			
		}
	}
	for (i=0;i<MAXSENSORS;i++)
	{	
			
		offset = ConfigSensorPresent(i);		
		if (offset >0)
		{	 
			numDigits = AppGetPressureConverted(offset-1,digits,1);
			if((digits[0]!=' ')||(digits[1]!=' ')||(digits[2]!=' '))
			{
				for (j=0;j<3;j++)
				{
					if(digits[j] == ' ')
					{
						ConfigTirePlaceReading(offset,(uint8_t *)FONTSMALL_BLANK,j);	
					}	
					else
					{
						ConfigTirePlaceReading(offset,(uint8_t *)FONTSMALL[digits[j]],j);
					}
				}					

			}	
		}
	}
}

void AppLCDConfigTireUpdate(void)
{
	uint8_t i,j,numDigits,offset,digits[3]; 

 
	for (i=0;i<MAXSENSORS;i++)
	{
		//------------------------
		// read table if the value is not 0xff 
		// place a 000 in the location. 
 		//---------------------------
		 
		 
		if (tableSensor.Item.WhichSensor[i].ID[0] != 0xff)
		{
			ConfigTirePlaceReading(i+1,(uint8_t *)FONTSMALL[0],0);
			ConfigTirePlaceReading(i+1,(uint8_t *)FONTSMALL[0],1);
			ConfigTirePlaceReading(i+1,(uint8_t *)FONTSMALL[0],2);	    
		}
		else
		{
			ConfigTirePlaceReading(i+1,(uint8_t *)FONTSMALL_FULL,0);
			ConfigTirePlaceReading(i+1,(uint8_t *)FONTSMALL_FULL,1);
			ConfigTirePlaceReading(i+1,(uint8_t *)FONTSMALL_FULL,2);	  			
		}
	}
	for (i=0;i<MAXSENSORS;i++)
	{	
			
		offset = ConfigSensorPresent(i);		
		if (offset >0)
		{	 
			numDigits = AppGetPressureConverted(offset-1,digits,1);
			if((digits[0]!=' ')||(digits[1]!=' ')||(digits[2]!=' '))
			{
				for (j=0;j<3;j++)
				{
					if(digits[j] == ' ')
					{
						ConfigTirePlaceReading(offset,(uint8_t *)FONTSMALL_BLANK,j);	
					}	
					else
					{
						ConfigTirePlaceReading(offset,(uint8_t *)FONTSMALL[digits[j]],j);
					}
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
uint8_t ConfigSensorPresent(uint8_t whichRam)
{
	uint8_t offset,done,i; 
	uint8_t id0,id1,id2; 
	
	offset = 0; 
	id0 = sensorDynamic[whichRam].ID[0]; 
	id1 = sensorDynamic[whichRam].ID[1]; 
	id2 = sensorDynamic[whichRam].ID[2]; 
	
	done = 0; 
	i = 0; 
	while ((done==0)&&(i<MAXSENSORS))
	{
		if (tableSensor.Item.WhichSensor[i].ID[0] == id0)
		{
			if (tableSensor.Item.WhichSensor[i].ID[1] == id1)
			{
				if (tableSensor.Item.WhichSensor[i].ID[2] == id2)
				{
					done = 1; 
				}	
			}
		}
		i++;
	}
	if (done == 1)
	{
		offset = i; 
	}
	return offset; 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void ConfigSensorUpdate(uint8_t which)
{
	uint8_t offset,digits[3],numDigits,i; 
	uint8_t id0,id1,id2; 
	
	 
	id0 = sensorDynamic[which].ID[0]; 
	id1 = sensorDynamic[which].ID[1]; 
	id2 = sensorDynamic[which].ID[2]; 	
	//--------------------------
	// tire pressure just came in 
	// sensor[] has the dynamic reading 
	// .... need to see if on the static flash list. 
	// * status LSB nibble needs to be a 0x03 
	//-----------------------------
	if ((sensorDynamic[which].LastPacket[5] &0x0f) == 0x03)
	{	
//		if ((sensorDynamic[which].NewValue) != 0x00)
//		{		
			sensorDynamic[which].NewValue = 0; 		
			offset = ConfigSensorPresent(which);		
			if (configTireState == CONFIGTIRE_SELECT)
			{
				//-------------------------------
				// configTireOffset goes from 1 to 12 (MAXSENSORS)
				if (offset > 0)
				{
					if (configTireOffset != offset)
					{
						offset--;
						//-------------------------
						// take it off the flash list 
						// update to new location 
						ConfigUpdate(0xff,TableSensorStart + (offset*4));
						ConfigUpdate(0xff,TableSensorStart + (offset*4)+1);
						ConfigUpdate(0xff,TableSensorStart + (offset*4)+2);
						ConfigUpdate(0xff,TableSensorStart + (offset*4)+3);
						//--------------------------------------------
						// place the hypens.
						ConfigTirePlaceReading(offset+1,(uint8_t *)FONTSMALL_FULL,0);
						ConfigTirePlaceReading(offset+1,(uint8_t *)FONTSMALL_FULL,1);
						ConfigTirePlaceReading(offset+1,(uint8_t *)FONTSMALL_FULL,2);												
					}
				}
				offset = configTireOffset; 
				if (offset >0)
				{
					offset--;
				}
 				//---------------------------
				// place position in flash 
				ConfigUpdate(id0,TableSensorStart + (offset*4));
				ConfigUpdate(id1,TableSensorStart + (offset*4)+1);
				ConfigUpdate(id2,TableSensorStart + (offset*4)+2);
				ConfigUpdate(offset+1,TableSensorStart + (offset*4)+3);
				if (I2CEEPROMBufferRead(&tableSensor.Item.WhichSensor[offset],TableSensorStart + (offset*4),4)!= 0)
				{ 
	  
				}	
				offset++;
			}
			numDigits = AppGetPressureConverted(which,digits,0);	
			for (i=0;i<3;i++)
			{
				if(digits[i] == ' ')
				{
					ConfigTirePlaceReading(offset,(uint8_t *)FONTSMALL_BLANK,i);	
				}	
				else
				{
					ConfigTirePlaceReading(offset,(uint8_t *)FONTSMALL[digits[i]],i);
				}
			}	
			//----------------------
			// take out of SELECT MODE
			configTireState = CONFIGTIRE_IDLE;
									
//		}
	}
	if ((sensorDynamic[which].LastPacket[5] &0x0f) == 0x02)
	{
		offset = ConfigSensorPresent(which); 	
	}
	if ((sensorDynamic[which].LastPacket[5] &0x0f) == 0x00)
	{
//		if ((sensorDynamic[which].NewValue) != 0x00)
//		{		
			sensorDynamic[which].NewValue = 0; 
			offset = ConfigSensorPresent(which); 
			//---------------------------------
			// update the value on the dispay
			//---------------------------------
			if (offset >0)
			{
				numDigits = AppGetPressureConverted(which,digits,0);			
				for (i=0;i<3;i++)
				{
					if(digits[i] == ' ')
					{
						ConfigTirePlaceReading(offset,(uint8_t *)FONTSMALL_BLANK,i);	
					}	
					else
					{
						ConfigTirePlaceReading(offset,(uint8_t *)FONTSMALL[digits[i]],i);
					}
				}
			}
						
//		}
	}
}



//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
 void AppLCDConfigTireHandle(void)
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
		if (brake_pressed != 0)
		{
			AppLCDConfigTirePressure(CONFIG_DELETE);
		}
		else
		{
			//-------------------
			// CENTER key released
			AppScreenInit(SCREEN_MAIN);
		}
	}
	else
	{
		if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
		{
			//UP
				AppLCDConfigTirePressure(CONFIG_UP);
		}
		else
		{
			if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
			{
					AppLCDConfigTirePressure(CONFIG_SELECT);
			}
			else
			{
				if (((whichOne & KEY_BRAKE)!=0)&&(brake_pressed == 0)&&
				(center_pressed!=0)) 
				{
					//DELETE
					AppLCDConfigTirePressure(CONFIG_DELETE);
				}
			}
		}
	}
}

void ConfigTireSelect(uint8_t which,uint8_t select)
{
	uint8_t tempData[6],*ptr,i; 
	
	ptr = (uint8_t *)FONTSMALL_START;
	if (select != 0)
	{
		ptr = (uint8_t *)FONTSMALL_BLANK;
	}
	
	switch(which)
	{
		case 1:
		{
				ptr = (uint8_t*)FONTSMALL_START;
				if (select != 0)
				{
					ptr = (uint8_t*)FONTSMALL_BLANK;
				}
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(12,48,tempData,6);  			
			break;
		}	
		case 12:
		{
				ptr = (uint8_t*)FONTSMALL_START;
				if (select != 0)
				{
					ptr = (uint8_t*)FONTSMALL_BLANK;
				}
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(85,48,tempData,6);  			
			break;
		}			
//--------------------------------------------------------------		
		case 2:
		{
				ptr = (uint8_t*)FONTSMALL_START;
				if (select != 0)
				{
					ptr = (uint8_t*)FONTSMALL_BLANK;
				}
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(12,40,tempData,6);  			
			break;
		}	
		case 11:
		{
				ptr = (uint8_t*)FONTSMALL_START;
				if (select != 0)
				{
					ptr = (uint8_t*)FONTSMALL_BLANK;
				}
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(85,40,tempData,6);  			
			break;
		}	
		
		
//------------------------------------------------		
		case 3:
		{
				ptr = (uint8_t*)FONTSMALL_START;
				if (select != 0)
				{
					ptr = (uint8_t*)FONTSMALL_BLANK;
				}
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(12,24,tempData,6);  			
			break;
		}	
		case 10:
		{
				ptr = (uint8_t*)FONTSMALL_START;
				if (select != 0)
				{
					ptr = (uint8_t*)FONTSMALL_BLANK;
				}
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(85,24,tempData,6);  			
			break;
		}			
//----------------------------------------------------------		
		case 4:
		{
				ptr = (uint8_t*)FONTSMALL_START;
				if (select != 0)
				{
					ptr = (uint8_t*)FONTSMALL_BLANK;
				}
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(0,16,tempData,6);  			
			break;
		}						
		case 5:
		{
				ptr = (uint8_t*)FONTSMALL_START;
				if (select != 0)
				{
					ptr = (uint8_t*)FONTSMALL_BLANK;
				}		
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(25,16,tempData,6);  			
			break;
		}	
		case 8:
		{
				ptr = (uint8_t*)FONTSMALL_START;
				if (select != 0)
				{
					ptr = (uint8_t*)FONTSMALL_BLANK;
				}	
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(70,16,tempData,6);  			
			break;
		}						
		case 9:
		{
				ptr = (uint8_t *)FONTSMALL_START;
				if (select != 0)
				{
					ptr = (uint8_t *)FONTSMALL_BLANK;
				}		
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(96,16,tempData,6);  			
			break;
		}			
		
		case 6:
		{
				ptr = (uint8_t *)FONTSMALL_START;
				if (select != 0)
				{
					ptr = (uint8_t *)FONTSMALL_BLANK;
				}		
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(12,8,tempData,6);  			
			break;
		}	
		case 7:
		{
				ptr = (uint8_t *)FONTSMALL_START;
				if (select != 0)
				{
					ptr = (uint8_t *)FONTSMALL_BLANK;
				}	
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(83,8,tempData,6);  			
			break;
		}							
		
																				
		default:
		{
			LCDPlaceData(24,24,(uint8_t *)FONTSMALL_START,5);
			break;
		}		
	}	
}
void ConfigTireDeselect(uint8_t which)
{
	
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppLCDConfigTirePressure(uint8_t key)	 
{
	uint8_t last,offset; 
	
	last = configTireOffset; 
	switch (key)
	{
		case CONFIG_DELETE:
		{
			if (configTireOffset >0)
			{
				offset = configTireOffset-1; 
				if ((tableSensor.Item.WhichSensor[configTireOffset-1].ID[0] != 0xff)||
					(tableSensor.Item.WhichSensor[configTireOffset-1].ID[1] != 0xff))
				{
					configTireState = CONFIGTIRE_IDLE;
					ConfigUpdate(0xff,TableSensorStart + (offset*4));
					ConfigUpdate(0xff,TableSensorStart + (offset*4)+1);
					ConfigUpdate(0xff,TableSensorStart + (offset*4)+2);
					ConfigUpdate(0xff,TableSensorStart + (offset*4)+3);
					if (I2CEEPROMBufferRead(&tableSensor.Item.WhichSensor[offset],TableSensorStart + (offset*4),4)!= 0)
					{ 
	  
					}						
					ConfigTirePlaceReading(configTireOffset,(uint8_t *)FONTSMALL_FULL,0);
					ConfigTirePlaceReading(configTireOffset,(uint8_t *)FONTSMALL_FULL,1);
					ConfigTirePlaceReading(configTireOffset,(uint8_t *)FONTSMALL_FULL,2);
					AppLCDConfigTireUpdate(); 
				}
			}			
			break;
		}
		case CONFIG_UP:
		{
			if (configTireState == CONFIGTIRE_IDLE)
			{	
				configTireOffset++; 
				if (configTireOffset >= 13)
				{
					configTireOffset = 1; 
				}
				if (last != 0)
				{
					ConfigTireSelect(last,1); 
				}
				ConfigTireSelect(configTireOffset,0); 
			}
			else
			{
				if (configTireState == CONFIGTIRE_SELECT)
				{
					configTireState = CONFIGTIRE_IDLE;
					if (configTireOffset >0)
					{
						AppLCDConfigTireUpdate(); 
					}
					configTireOffset++; 
					if (configTireOffset >= 13)
					{
						configTireOffset = 1; 
					}
					if (last != 0)
					{
						ConfigTireSelect(last,1); 
					}
					ConfigTireSelect(configTireOffset,0); 					
				}				
			}			
			break;
		}
		case CONFIG_SELECT:
		{
			if (configTireState == CONFIGTIRE_IDLE)
			{
				//----- check if this spot is open .... 
				
				if (configTireOffset >0)
				{
					if ((tableSensor.Item.WhichSensor[configTireOffset-1].ID[0] == 0xff)&&
						(tableSensor.Item.WhichSensor[configTireOffset-1].ID[1] == 0xff))
					{
						configTireState = CONFIGTIRE_SELECT;
						ConfigTirePlaceReading(configTireOffset,(uint8_t *)FONTSMALL_HYPEN,0);
						ConfigTirePlaceReading(configTireOffset,(uint8_t *)FONTSMALL_HYPEN,1);
						ConfigTirePlaceReading(configTireOffset,(uint8_t *)FONTSMALL_HYPEN,2);
					}
				}
			}
			else
			{
				if (configTireState == CONFIGTIRE_SELECT)
				{
					configTireState = CONFIGTIRE_IDLE;
					if (configTireOffset >0)
					{
						AppLCDConfigTireUpdate();
					}
				}				
			}
			break;
		}
	}
/*
	if (whichScreen == SCREEN_TOWCAR)
	{
 		refreshScreen(TowMenu,1);
		//---------------------------
		// load the first 3 pressure sensor data on 
		// screen.
		// 1. get the data 
		// 2. convert it from KPA to PSI multiply by 145038 and divide by 1000000
		// 3. see how many digits. 
		//---------------------------------
		for (k=0; k<4;k++)
		{
			switch (k)
			{
				case 0:
				{
					horoffset = 11; 
					numDigits = AppGetPressureConverted(0,digits);
#if TEST_PRESSURE
					//---------testing 
					numDigits = 3; 
					digits[0] = 1;
					digits[1] = 2; 
					digits[2] = 3; 
#endif					
					break;
				}
				case 1:
				{
					horoffset = 11;
					numDigits = AppGetPressureConverted(1,digits);
#if TEST_PRESSURE					
					//---------testing
					numDigits = 3;
					digits[0] = 4;
					digits[1] = 5;
					digits[2] = 6;	
#endif									
					break;
				}				
				case 2:
				{
					horoffset = 80; 
					numDigits = AppGetPressureConverted(2,digits);	 
#if TEST_PRESSURE						
					//---------testing
					numDigits = 3;
					digits[0] = 7;
					digits[1] = 8;
					digits[2] = 9;		
#endif											
					break;
				}
				case 3:
				{
					horoffset = 80;
					numDigits = AppGetPressureConverted(3,digits);	 
#if TEST_PRESSURE					
					//---------testing
					numDigits = 3;
					digits[0] = 0;
					digits[1] = 0;
					digits[2] = 3;	
#endif									
					break;
				}				
			}
			
			 
			if ((numDigits>0)&&((k==0)||(k==2))) 
			{
				for (j=0;j<numDigits;j++)
				{
					myvalue = digits[j]; 
					if (myvalue != ' ')
					{
						ptr = FONTMID[myvalue][TOP];
						ptr2 = FONTMID[myvalue][MID];
						//----------------------------------
						// Get the 4 bits of data from the screen data.
						// from SCREEN HOME at
						for (i=0;i<9;i++)
						{
							tempData[i] = TowMenu[(8*16*2)+11+i];
							tempData[i] &= 0xE0;
							temp = *ptr++;
							tempData[i] = temp<<3;
							tempData[i] &= 0xF8;
							temp = *ptr2++;
							temp2 = temp>>5; 
							tempData[i] |= temp2;
						}
						LCDPlaceData(horoffset,16,tempData,9);  
						ptr = FONTMID[myvalue][MID];
						//----------------------------------
						// Get the 4 bits of data from the screen data.
						// from SCREEN HOME at
						for (i=0;i<9;i++)
						{
							tempData[i] = TowMenu[(8*16*2)+11+i];
							tempData[i] &= 0x07;
							temp = *ptr++;
							temp = temp<<3;
							tempData[i] |= temp;
						}			
						LCDPlaceData(horoffset,24,tempData,9);
					}
					horoffset+=8;
				} 		
			}
			
 
			
			if ((numDigits>0)&&((k==1)||(k==3))) 
			{
				for (j=0;j<numDigits;j++)
				{
					myvalue = digits[j]; 
					if (myvalue != ' ')
					{
						ptr = FONTMID[myvalue][TOP];
						//----------------------------------
						// Get the 4 bits of data from the screen data. 
						// from SCREEN HOME at 
						for (i=0;i<9;i++)
						{
							tempData[i] = TowMenu[(8*16*4)+11+i];	
							tempData[i] &= 0xE0;
							temp = *ptr++;
							temp = temp&0x1f; 
							tempData[i] |= temp;
						}
						LCDPlaceData(horoffset,(8*4),tempData,9);  //FONTMID[value][TOP],9);
						LCDPlaceData(horoffset,(8*4)+8,FONTMID[myvalue][MID],9);						
					}
					horoffset+=8;
				} 		
			}			
		}
	}
*/	
}

void ConfigTirePlaceReading(uint8_t which,uint8_t *select,uint8_t position)
{
	uint8_t tempData[6],*ptr,i,j; 
	
	j = position; 
	ptr = select; 
	
	switch(which)
	{
		case 1:
		{
//			for (j=0;j<3;j++)
//			{
				ptr = select;
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(18+(j*6),48,tempData,6);  			
//			}
			break;
		}	
		case 12:
		{
//			for (j=0;j<3;j++)
//			{
				ptr = select;
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(91+(j*6),48,tempData,6);  			
//			}
			break;
		}			
		
 
//--------------------------------------------------------------		
		case 2:
		{
//			for (j=0;j<3;j++)
//			{
				ptr = select;
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(18+(j*6),40,tempData,6);  			
//			}
			break;
		}	
		case 11:
		{
//			for (j=0;j<3;j++)
//			{
				ptr = select;
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(91+(j*6),40,tempData,6);  			
//			}
			break;
		}	
		
//------------------------------------------------		
		case 3:
		{
//			for (j=0;j<3;j++)
//			{
				ptr = select;
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(18+(j*6),24,tempData,6);  			
//			}
			break;
		}	
		case 10:
		{
//			for (j=0;j<3;j++)
//			{
				ptr = select;
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(91+(j*6),24,tempData,6);  			
//			}
			break;
		}			
//----------------------------------------------------------		
		case 4:
		{
//			for (j=0;j<3;j++)
//			{
				ptr = select;
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(5+(j*6),16,tempData,6);  			
//			}
			break;
		}						
		case 5:
		{
//			for (j=0;j<3;j++)
//			{
				ptr = select;	
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(31+(j*6),16,tempData,6);  			
//			}
			break;
		}	
		case 8:
		{
//			for (j=0;j<3;j++)
//			{
				ptr = select;
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(76+(j*6),16,tempData,6);  			
//			}
			break;
		}						
		case 9:
		{
//			for (j=0;j<3;j++)
//			{
				ptr = select;
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(102+(j*6),16,tempData,6);  			
//			}
			break;
		}			
		
		case 6:
		{
//			for (j=0;j<3;j++)
//			{
				ptr = select;
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(18+(j*6),8,tempData,6);  			
//			}
			break;
		}	
		case 7:
		{
//			for (j=0;j<3;j++)
//			{
				ptr = select;
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(91+(j*6),8,tempData,6);  			
//			}
			break;
		}							
		
																				
		default:
		{
			LCDPlaceData(24,24,(uint8_t *)FONTSMALL_HYPEN,5);
			break;
		}		
	}	
} 

#endif
