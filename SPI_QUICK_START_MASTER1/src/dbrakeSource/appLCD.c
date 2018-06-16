//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: appLCD.c
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor: STM32F103R
// TOOLS: IAR Workbench
// DATE:
// CONTENTS: This file contains
//------------------------------------------------------------------------------
// HISTORY: This file
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#include <asf.h>
#include "dbrakeDefs.h"
#include "radio.h"
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
#include "sx1272-Fsk.h"
#include "sx1272-Lora.h"
#include "appLCDConfig.h"
#include "appLCDConfigMisc.h"
#include "appLCDBrakeState.h"
#include "driverScreenColor.h"
#include "appMotor.h"


#define TEST_PRESSURE 0
extern const uint8_t ScreenSetupNew[1024];
extern const uint8_t ScreenSetupMaxForce[1024];


extern const uint8_t MainMenu[1024];
extern const uint8_t MainMenuToolHighlight[512];
extern const uint8_t MainMenuTireHighlight[512];
extern const uint8_t MainMenuHomeHighlight[512];
extern const uint8_t ScreenConfig[1024];
extern const uint8_t ScreenForce[1024];
extern const uint8_t ScreenHome[1024];
#if REMOTE_TIREON
extern const uint8_t ScreenCoach[1024];
extern const uint8_t TowMenuDegreeF[1024];
extern const uint8_t TowMenuDegreeC[1024];
extern const uint8_t CabMenuPSI[1024];
extern const uint8_t CabMenuDegreeC[1024];
extern const uint8_t SystemTireRadioRemote1[1024];
extern const uint8_t SystemTireRadioRemote2[1024];
extern const uint8_t SystemTireRadioBrake1[1024];
extern const uint8_t SystemTireRadioBrake2[1024];
#endif

extern const uint8_t SystemScreenMenu[1024];
extern const uint8_t TowMenu[1024];

extern const uint8_t SystemStatusRoot[1024];

extern const uint8_t BrakeStatusRoot[1024];
extern const uint8_t ConfigSenseHighlight[512];
extern const uint8_t ConfigMiscHighlight[512];
extern const uint8_t ConfigHomeHighlight[512];
extern const uint8_t SystemSensor[1024];
extern const uint8_t ScreenHomeCommError[1024];
extern const uint8_t ScreenMisc[1024];
extern const uint8_t ScreenHomeVoltageError[1024];
extern const uint8_t ScreenHomeSetupError[1024];
extern const uint8_t ScreenHomeSetupGoneWrong[1024];

extern const uint8_t ScreenSkeleton[1024];
extern const uint8_t ScreenHomeBreakawayError[1024];
extern const uint8_t ScreenHomeManualBrake[1024];
 
extern const uint8_t ScreenSetupBrakeBacklight[1024]; 
//---------------------GLOBAL VARIABLES-----------------------------------
uint8_t appScreenEvent;
#define APPSCREENEVENT_KEYCHANGE	0x01


//---------------------LOCAL VARIABLES------------------------------------
//--------------------------
uint8_t pressureSwap=0; 	

uint8_t appScreen=0;
uint8_t appScreenHomeType; 

//------------systemOffset to handle how i entered the menu with the double key press
uint8_t systemOffset; 

//--------MAIN screen handling of the keys
//---screenMainOffset 
// used to handle left and right movements and highlighting in the screen.
// values can be 0,1,2 (all others are defaulted to 0)
uint8_t screenMainOffset; 

uint8_t fskRadioReading;
uint8_t carRadioReading[MAXSENSORS];

#define MAX_FORCE 9
//---------------------LOCAL FUNCTION PROTOTYPES--------------------------
void AppScreenPlaceForce(uint8_t value,uint8_t which);
#define SCREEN_FORCE_X	1
#define SCREEN_FORCE_Y  64


#define FONT_LARGE 2

void AppScreenConfigHighlight(uint8_t value);
void AppScreenMenuHighlight(uint8_t value);
void AppScreenUpdateStatusRoot(void);
void AppScreenUpdateCarRadio(void);	
void AppScreenUpdateStatusBrake(void);
void AppScreenPlaceTemperature(void);
uint8_t GetMask(uint8_t offset,uint8_t i);
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppScreenProcessKeyChange(void)
{
	uint8_t whichOne; 
	

	switch(appScreen)
	{
		case SCREEN_DOWNLOADREQUEST:
		{
			//-----------------------
			// which button changed.
			// 1. if LEFT goes from pressed to depressed - go to FORCE menu
			// 2. if CENTER goes from pressed to depressed - go to Main menu
			// 3. if RIGHT goes from pressed to depressed - go to COACH menu
			//-----------------------
			whichOne = ButtonChanged();
  			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				 
				AppScreenInit(SCREEN_HOME);
			}
			else
			{
				if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
				{
					if (remoteDownloadApp.version == 0)
					{
					}
					else
					{
						DownloadStart();
						AppScreenInit(SCREEN_DOWNLOADING);		
					}
					
				}
				else
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
  
					}
				}
			}
			break;
		}		
		case SCREEN_INFO:
		{
			//-----------------------
			// which button changed.
			// 1. if LEFT goes from pressed to depressed - go to FORCE menu
			// 2. if CENTER goes from pressed to depressed - go to Main menu
			// 3. if RIGHT goes from pressed to depressed - go to COACH menu
			//-----------------------
			whichOne = ButtonChanged();
			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				
				AppScreenInit(SCREEN_HOME);
			}
			else
			{
			 
			}
			break;
		}		
		case SCREEN_HOME:
		{
			//-----------------------
			// which button changed.
			// 1. if LEFT goes from pressed to depressed - go to FORCE menu
			// 2. if CENTER goes from pressed to depressed - go to Main menu
			// 3. if RIGHT goes from pressed to depressed - go to COACH menu
			//-----------------------	
			whichOne = ButtonChanged();
			if ((appScreenHomeType ==SCREEN_HOME_BREAKAWAYTIP)&&(brakeState != BRAKESTATE_ACTIVE))
			{
				if ((center_pressed == 0)&&(left_pressed ==0)&&(right_pressed==0))
				{
					appScreenHomeType = SCREEN_HOME_BASE;
				}
			}
			else
			{
//				if (appScreenHomeType != SCREEN_HOME_BREAKAWAYTIP)
//				{
					if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
					{
						//-------------------
						// CENTER key released 
//NOTTIRES 08-14-16						AppScreenInit(SCREEN_MAIN);
						AppScreenInit(SCREEN_SETUPNEW);
					}
					else
					{
						if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
						{
							//-------------------
							// CENTER key released
							AppScreenInit(SCREEN_FORCE);
						}
						else
						{
							if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
							{
								//-------------------
								// CENTER key released
//NOTTIRES 08-14-16				AppScreenInit(SCREEN_COACH);
								BacklightToggleLight(table0.Item.ScreenColor);
							}				
						}				
					}
//				}
			}
			break;
		}
		case SCREEN_FORCE:
		{
			//-----------------------
			// which button changed.
			// 1. if LEFT decrements the FORCE
			// 2. if CENTER goes from pressed to depressed - go to Home menu
			// 3. if RIGHT increments the FORCE
			//-----------------------	
			whichOne = ButtonChanged();
			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				//-------------------
				// CENTER key released 
				AppScreenInit(SCREEN_HOME);
			}
			else
			{
				if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
				{
					//-------------------
					// LEFT key released
					// decrement force value
					if (table0.Item.Hybrid == FALSE)
					{
						if (table0.Item.MaxForce == 0) 
						{
 							table0.Item.Hybrid = TRUE; 
							ConfigUpdate(table0.Item.Hybrid,HybridSetting);
							table0.Item.MaxForce = 3; 
							ConfigUpdate(table0.Item.MaxForce,MaxForce_Setting); 
							table0.Item.ForceMaxSet = 4; 
							ConfigUpdate(table0.Item.ForceMaxSet,ForceMaxSetting); 							
						}
						else
						{
							table0.Item.MaxForce--;
							ConfigUpdate(table0.Item.MaxForce,MaxForce_Setting);
						}						
					}					
					AppScreenPlaceForce(table0.Item.MaxForce,SCREEN_FORCE);
				}
				else
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
						//-------------------
						// RIGHT key released
						// increment force value
						if (table0.Item.Hybrid == TRUE)
						{
							table0.Item.Hybrid = FALSE; 
							ConfigUpdate(table0.Item.Hybrid,HybridSetting);
							table0.Item.MaxForce = 0; 
							ConfigUpdate(table0.Item.MaxForce,MaxForce_Setting); 
						}
						else
						{
							if (table0.Item.MaxForce < MAX_FORCE) 
							{
								table0.Item.MaxForce++;
								ConfigUpdate(table0.Item.MaxForce,MaxForce_Setting);
							}						
						}					
						AppScreenPlaceForce(table0.Item.MaxForce,SCREEN_FORCE);							 
					}				
				}				
			}
			break;
		}
#if REMOTE_TIREON 
		case SCREEN_MAIN:
		{
			//-------------------------------
			// which button changed.
			// 1. if LEFT ROTATE BOXES LEFT
			// 2. if CENTER goes from pressed to depressed - go to Home menu
			// 3. if RIGHT ROTATE BOXES RIGHT
			//-------------------------------
			whichOne = ButtonChanged();
			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				//-------------------
				// CENTER key released
				switch(screenMainOffset)
				{ 
					case 0:
					{
						//---------------------------------
						// tools 
						AppScreenInit(SCREEN_CONFIG);
						systemOffset = 0; 							
						break;
					}
					case 1:
					{
						AppScreenInit(SCREEN_HOME);
						break;
					}
					default:
					{
						AppScreenInit(SCREEN_COACH);
						break;						
					}
				}
			}
			else
			{
				if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
				{
					if (right_pressed != 0)
					{
							AppScreenInit(SCREEN_SYSTEM);
							systemOffset = 0; 	
					}
					else
					{
						if (screenMainOffset != 0)
						{
							screenMainOffset--;
						}
						AppScreenMenuHighlight(screenMainOffset);
						
					}
				}
				else
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
						//-------------------
						if (left_pressed != 0)
						{
								AppScreenInit(SCREEN_SYSTEM);
								systemOffset = 0; 	
						}		
						else
						{
							screenMainOffset++;
							if (screenMainOffset >2)
							{
								screenMainOffset=2;
							}
							AppScreenMenuHighlight(screenMainOffset);
						
						}												 			 
					}				
				}				
			}
			
			break;
		}
#endif 		
		case SCREEN_SETUPNEW:
		{
			//-------------------------------
			// which button changed.
			// 1. if LEFT ROTATE BOXES LEFT
			// 2. if CENTER goes from pressed to depressed - go to Home menu
			// 3. if RIGHT ROTATE BOXES RIGHT
			//-------------------------------
			whichOne = ButtonChanged();
			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				//-------------------
				// CENTER key released 
				AppScreenSetupKey(KEY_CENTER);	
			}
			else
			{
				if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
						 AppScreenInit(SCREEN_INFO);
					}
					else
					{
						AppScreenSetupKey(KEY_LEFT);
					}
				}
				else
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
						AppScreenSetupKey(KEY_RIGHT);	
					}				
				}				
			}
			break;
		}		
		case SCREEN_SETUPSENSITIVITY:
		case SCREEN_SETUPBRAKEBACKLIGHT:
		case SCREEN_SETUPPAIR:
		case SCREEN_SETUPHYBRID:
		case SCREEN_SETUPMAXFORCE:
		case SCREEN_SETUPRESET:
		{
			//-------------------------------
			// Load in the current force setting.
			//------------------------------
			whichOne = ButtonChanged();
			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				//-------------------
				// CENTER key released
				AppScreenSetupItemKey(appScreen,KEY_CENTER);
			}
			else
			{
				if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
				{
					AppScreenSetupItemKey(appScreen,KEY_LEFT);
				}
				else
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
						AppScreenSetupItemKey(appScreen,KEY_RIGHT);
					}
				}
			}
			break;
		}	
#if REMOTE_TIREON				
		case SCREEN_SYSTEM:
		{
			//-------------------------------
			// which button changed.
			// 1. if LEFT ROTATE BOXES LEFT
			// 2. if CENTER goes from pressed to depressed - go to Home menu
			// 3. if RIGHT ROTATE BOXES RIGHT
			//-------------------------------
			whichOne = ButtonChanged();
			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				//-------------------
				// CENTER key released 
				AppScreenInit(SCREEN_MAIN);
			}
			else
			{
				if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
				{
						AppScreenInit(SCREEN_STATUS_ROOT);
						systemOffset = 0;				 
				}
				else
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
						AppScreenInit(SCREEN_STATUS_SKELETON);
						systemOffset = 0; 
					}				
				}				
			}
			break;
		}
		case SCREEN_STATUS_SKELETON:
		{
			//-------------------------------
			// which button changed.
			// 1. if LEFT ROTATE BOXES LEFT
			// 2. if CENTER goes from pressed to depressed - go to Home menu
			// 3. if RIGHT ROTATE BOXES RIGHT
			//-------------------------------
			whichOne = ButtonChanged();
			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				//-------------------
				// CENTER key released 
				AppScreenInit(SCREEN_MAIN);
			}
			else
			{
				if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
				{
						AppScreenInit(SCREEN_SYSTEM);
						systemOffset = 0;				 
				}
				else
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
						AppScreenInit(SCREEN_MAIN);
						systemOffset = 0; 
					}				
				}				
			}
			break;
		}		
		
		case SCREEN_STATUS_ROOT:
		{
			//-------------------------------
			// which button changed.
			// 1. if LEFT ROTATE BOXES LEFT
			// 2. if CENTER goes from pressed to depressed - go to Home menu
			// 3. if RIGHT ROTATE BOXES RIGHT
			//-------------------------------
			whichOne = ButtonChanged();
			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				//-------------------
				// CENTER key released
				AppScreenInit(SCREEN_STATUS_ROOT);
				AppScreenUpdateStatusRoot();
				systemOffset = 0; 
			}
			else
			{
				if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
				{
					
					AppScreenInit(SCREEN_TIRE_REMOTE1);					 
				}
				else
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
						//-------------------
						// RIGHT key released
						AppScreenInit(SCREEN_SYSTEM);
						systemOffset = 0;						
					}
				}
			}
			break;
		}		
#endif 		
#if REMOTE_TIREON		
		case SCREEN_TIRE_REMOTE1:
		{
			whichOne = ButtonChanged();
			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				//-------------------
				// CENTER key released - update display
				refreshScreen(SystemTireRadioRemote1,1);
				AppScreenUpdateCarRadio();				 
			}
			else
			{
				if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
				{
					AppScreenInit(SCREEN_TIRE_REMOTE2);	
				}
				else
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
						//-------------------
						// RIGHT key released
						AppScreenInit(SCREEN_SYSTEM);
						systemOffset = 0;
					}
				}
			}
			break;
		}		
		case SCREEN_TIRE_REMOTE2:
		{
			whichOne = ButtonChanged();
			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				//-------------------
				// CENTER key released - update display
				refreshScreen(SystemTireRadioRemote2,1);
				AppScreenUpdateCarRadio();
			}
			else
			{
				if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
				{
					AppScreenInit(SCREEN_BRAKE_STATUS);
				}
				else
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
						appScreen = SCREEN_TIRE_REMOTE1;
						refreshScreen(SystemTireRadioRemote1,1);
						AppScreenUpdateCarRadio();
					}
				}
			}
			break;
		}		
		case SCREEN_COACH:
		{
			//-------------------------------
			// which button changed.
			// 1. if LEFT  
			// 2. if CENTER goes from pressed to depressed - go to Home menu
			// 3. if RIGHT  
			//-------------------------------
			whichOne = ButtonChanged();
			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				//-------------------
				// CENTER key released 
				AppScreenInit(SCREEN_MAIN);
			}
			else
			{
				if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
				{
					AppScreenInit(SCREEN_TOWCAR);				 
				}
				else
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
						//-------------------
						// RIGHT key released
						AppScreenInit(SCREEN_HOME);			 			 
					}				
				}				
			}
			break;
		}		
		case SCREEN_TOWCAR:
		{
			//-------------------------------
			// which button changed.
			// 1. if LEFT  
			// 2. if CENTER goes from pressed to depressed - go to Home menu
			// 3. if RIGHT  
			//-------------------------------
			whichOne = ButtonChanged();
			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				//-------------------
				// CENTER key released 
				AppScreenInit(SCREEN_MAIN);
			}
			else
			{
				if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
				{
					AppScreenInit(SCREEN_COACH);				 
				}
				else
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
						//-------------------
						// RIGHT key released
						AppScreenInit(SCREEN_HOME);			 			 
					}				
				}				
			}
			break;
		}
		case SCREEN_BRAKE_STATUS:
		{
			whichOne = ButtonChanged();
			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				//-------------------
				// CENTER key released - update display
				refreshScreen(BrakeStatusRoot,1);
				AppScreenUpdateStatusBrake();
			}
			else
			{
				if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
				{
					 
				}
				else
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
						appScreen = SCREEN_TIRE_REMOTE2;
						refreshScreen(SystemTireRadioRemote2,1);
						AppScreenUpdateCarRadio();
					}
				}
			}
			break;
		}	
		case SCREEN_CONFIG:
		{
			//-------------------------------
			// which button changed.
			// 1. if LEFT ROTATE BOXES LEFT
			// 2. if CENTER goes from pressed to depressed - go to Home menu
			// 3. if RIGHT ROTATE BOXES RIGHT
			//-------------------------------
			whichOne = ButtonChanged();
			if (((whichOne & KEY_CENTER)!=0)&&(center_pressed == 0))
			{
				//-------------------
				// CENTER key released
				switch(screenMainOffset)
				{ 
					case 0:
					{
						//---------------------------------
						// tools 
						AppScreenInit(SCREEN_SENSOR);
						systemOffset = 0; 							
						break;
					}
					case 1:
					{
						AppScreenInit(SCREEN_HOME);
						break;
					}
					default:
					{
						AppScreenInit(SCREEN_MISC);
						break;						
					}
				}
			}
			else
			{
				if (((whichOne & KEY_LEFT)!=0)&&(left_pressed == 0))
				{
					if (right_pressed != 0)
					{
//							AppScreenInit(SCREEN_SYSTEM);
//							systemOffset = 0; 	
					}
					else
					{
						if (screenMainOffset != 0)
						{
							screenMainOffset--;
						}
						AppScreenConfigHighlight(screenMainOffset);
						
					}
				}
				else
				{
					if (((whichOne & KEY_RIGHT)!=0)&&(right_pressed == 0))
					{
						//-------------------
						if (left_pressed != 0)
						{
//								AppScreenInit(SCREEN_SYSTEM);
//								systemOffset = 0; 	
						}		
						else
						{
							screenMainOffset++;
							if (screenMainOffset >2)
							{
								screenMainOffset=2;
							}
							AppScreenConfigHighlight(screenMainOffset);
						
						}												 			 
					}				
				}				
			}
			break;
		}	
		case SCREEN_SENSOR:
		{
			AppLCDConfigTireHandle();
			break;
		}	
		case SCREEN_MISC:
		{
			AppLCDConfigMiscHandle();
			break;
		}	
#endif						
	}	
	
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function 
// 
//==============================================================================
void AppScreenInit(uint8_t which) 
{
	uint8_t i,offset,temp; 
	uint32_t ltemp;
	 
	 
	uint8_t *wordList[20];
	uint8_t j,tempData[6],*ptr;
		
	appScreen = which; 
	switch(appScreen)
	{
		case SCREEN_DOWNLOADREQUEST:
		{
			refreshScreen(ScreenDownloadRequest,1);
			offset = 72;
			temp = FWVER3 & 0x0f;
			LCDPlaceData(offset,24,(uint8_t *)FONTSMALL[temp],5);
			temp = FWVER2  & 0x0f;
			LCDPlaceData(offset+6,24,(uint8_t *)FONTSMALL[temp],5);
			LCDPlaceData(offset+12,24,(uint8_t *)FONTSMALL_DOT,5);
			temp = FWVER1 & 0x0f;
			LCDPlaceData(offset+18,24,(uint8_t *)FONTSMALL[temp],5);
			temp = FWVER0 & 0x0f;
			LCDPlaceData(offset+24,24,(uint8_t *)FONTSMALL[temp],5);	
			
			if (remoteDownloadApp.version != 0)
			{	
				ltemp = remoteDownloadApp.version;
				ltemp = ltemp>>20;
				ltemp = ltemp & 0x0f;
				temp = ltemp;
				LCDPlaceData(offset,32,(uint8_t *)FONTSMALL[temp],5);
				ltemp = remoteDownloadApp.version;
				ltemp = ltemp>>16;
				ltemp = ltemp & 0x0f;
				temp = ltemp;
				LCDPlaceData(offset+6,32,(uint8_t *)FONTSMALL[temp],5);
				LCDPlaceData(offset+12,32,(uint8_t *)FONTSMALL_DOT,5);
				ltemp = remoteDownloadApp.version;
				ltemp = ltemp>>12;
				ltemp = ltemp & 0x0f;
				temp = ltemp;
				LCDPlaceData(offset+18,32,(uint8_t *)FONTSMALL[temp],5);
				ltemp = remoteDownloadApp.version;
				ltemp = ltemp>>8;
				ltemp = ltemp & 0x0f;
				temp = ltemp;
				LCDPlaceData(offset+24,32,(uint8_t *)FONTSMALL[temp],5);		
			}
			else
			{
				offset = 52;
				tempData[5] = 0x00;
				wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
				wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
				wordList[2] = (uint8_t *)FONTSMALL_BLANK;
				wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLD];
				wordList[4] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
				wordList[5] = (uint8_t *)FONTSMALL[OFFSET_SMALLW];
				wordList[6] = (uint8_t *)FONTSMALL[OFFSET_SMALLN];
				wordList[7] = (uint8_t *)FONTSMALL[OFFSET_SMALLL];
				wordList[8] = (uint8_t *)FONTSMALL[OFFSET_SMALLO];
				wordList[9] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
				wordList[10] = (uint8_t *)FONTSMALL[OFFSET_SMALLD];
			
				for (j=0;j<11;j++)
				{
					ptr = wordList[j];
					for (i=0;i<5;i++)
					{
						tempData[i] = *ptr++;
					}
					LCDPlaceData(offset,32,tempData,6);
					offset+=6;
				}		
			}
			break;
		}		
		case SCREEN_INFO:
		{
			refreshScreen(ScreenInfoNew,1);
			offset = 31;
			temp = brakesVersionDate[0] & 0x0f;
			LCDPlaceData(offset,16,(uint8_t *)FONTSMALL[temp],5);
			temp = brakesVersionDate[1]  & 0x0f;
			LCDPlaceData(offset+6,16,(uint8_t *)FONTSMALL[temp],5);
			temp = brakesVersionDate[2]  & 0x0f;
			LCDPlaceData(offset+16,16,(uint8_t *)FONTSMALL[temp],5);
			temp = brakesVersionDate[3]  & 0x0f;
			LCDPlaceData(offset+22,16,(uint8_t *)FONTSMALL[temp],5);			
			
			offset = 94;
			temp = FWVER3 & 0x0f;
			LCDPlaceData(offset,16,(uint8_t *)FONTSMALL[temp],5);
			temp = FWVER2  & 0x0f;
			LCDPlaceData(offset+6,16,(uint8_t *)FONTSMALL[temp],5);
			temp = FWVER1  & 0x0f;
			LCDPlaceData(offset+16,16,(uint8_t *)FONTSMALL[temp],5);
			temp = FWVER0  & 0x0f;
			LCDPlaceData(offset+22,16,(uint8_t *)FONTSMALL[temp],5);					
			break;
		}		
		case SCREEN_DOWNLOADING:
		{
			refreshScreen(ScreenDownloading,1);  			
			break;
		}			
		case SCREEN_IDLE:
		{
			break;
		}
		case SCREEN_HOME:
		{
			refreshScreen(ScreenHome,1);  			
			AppScreenPlaceForce(table0.Item.MaxForce,SCREEN_HOME);	
			AppScreenUpdateHome(); 			
			break;
		}	
		case SCREEN_SETUPNEW:
		{
			refreshScreen(ScreenSetupNew,1);	 
			screenMainOffset = 1; 
			AppScreenSetupInit();
			break;
		}		
		case SCREEN_FORCE:
		{	
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			refreshScreen(ScreenForce,1);  	
//			LCDPlaceData(56,24,FONTLARGE[1][TOP],11);				
//			LCDPlaceData(56,32,FONTLARGE[1][MID],11);
//			LCDPlaceData(56,40,FONTLARGE[1][BOT],11);
			AppScreenPlaceForce(table0.Item.MaxForce,SCREEN_FORCE);			
			break;
		}		
		case SCREEN_SETUPSENSITIVITY:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			screenMainOffset = 1;
			refreshScreen(ScreenSetupSensitivitySettings,1);
			break;
		}		
		case SCREEN_SETUPPAIR:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			screenMainOffset = 1;
			refreshScreen(ScreenSetupPair,1);
			break;
		}		
		case SCREEN_SETUPHYBRID:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			screenMainOffset = 1;
			refreshScreen(ScreenSetupHybrid,1);
			break;
		}		
		case SCREEN_SETUPBRAKEBACKLIGHT:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			screenMainOffset = 1;
			refreshScreen(ScreenSetupBrakeBacklight,1);
			break;
		}				
		case SCREEN_SETUPMAXFORCE:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			screenMainOffset = 1;
			refreshScreen(ScreenSetupMaxForce,1);
			break;
		}		
		case SCREEN_SETUPRESET:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			screenMainOffset = 1;
			refreshScreen(ScreenSetupResetSettings,1);
			break;
		}	
#if REMOTE_TIREON			
		case SCREEN_MAIN:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			screenMainOffset = 1; 
			refreshScreen(MainMenu,1);			
			break;
		}
		case SCREEN_SYSTEM:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			refreshScreen(SystemScreenMenu,1);
			
			break;
		}		
		case SCREEN_STATUS_ROOT:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			refreshScreen(SystemStatusRoot,1);
			AppScreenUpdateStatusRoot();			
			break;
		}
		case SCREEN_BRAKE_STATUS:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			refreshScreen(BrakeStatusRoot,1);
			AppScreenUpdateStatusBrake();
			break;
		}					
		case SCREEN_TIRE_REMOTE1:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			for (i=0;i<MAXSENSORS;i++)
			{
				carRadioReading[i] = 0;
			}
			refreshScreen(SystemTireRadioRemote1,1);
			AppScreenUpdateCarRadio();						
			break;
		}	
		case SCREEN_TIRE_REMOTE2:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			refreshScreen(SystemTireRadioRemote2,1);
			AppScreenUpdateCarRadio();
			break;
		}	
#endif 			
#if REMOTE_TIREON 			
		case SCREEN_COACH:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			if (pressureSwap == 0)
			{
				refreshScreen(CabMenuPSI,1);
				AppScreenPlacePressure( );
			}
			else
			{
				refreshScreen(CabMenuDegreeC,1);
				AppScreenPlaceTemperature( );
			}
			break;
		}
		case SCREEN_TOWCAR:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			if (pressureSwap == 0)
			{
				refreshScreen(TowMenu,1);
				AppScreenPlacePressure( );
			}
			else
			{
				refreshScreen(TowMenuDegreeC,1);
				AppScreenPlaceTemperature( );
			}
			break;
		}		
		case SCREEN_CONFIG:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			screenMainOffset = 1;
			refreshScreen(ScreenConfig,1);
			break;
		}			
		case SCREEN_SENSOR:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			screenMainOffset = 0;
			refreshScreen(SystemSensor,1);
			AppLCDConfigTireInit();
			break;
		}	
		case SCREEN_MISC:
		{
			//-------------------------------
			// Load in the current force setting.
			//-------------------------------
			screenMainOffset = 0;
			refreshScreen(ScreenMisc,1);
			AppLCDConfigMiscInit();
			break;
		}
		case SCREEN_STATUS_SKELETON:
		{
			screenMainOffset = 0;
			refreshScreen(ScreenSkeleton,1);
			AppSkeletonBrakeState();
			break;			
		}	
#endif 							
	}
}


void AppScreenConfigHighlight(uint8_t value)
{
	//------------------------------
	// value can be 0,1,2 
	// value of 1 is on HOME 
	//          0 IS TOOLS 
	//          2 is tires
	//     all others is on HOME. 
	//------------------------------
	switch(value)
	{
		case 0:
		{
			LCDPlacePage(2,&ConfigSenseHighlight[0],true); 
			LCDPlacePage(3,&ConfigSenseHighlight[128],true); 	
			LCDPlacePage(4,&ConfigSenseHighlight[256],true); 		
			LCDPlacePage(5,&ConfigSenseHighlight[384],true); 	
			break;
		}
		case 1:
		{
			LCDPlacePage(2,&ConfigHomeHighlight[0],true); 
			LCDPlacePage(3,&ConfigHomeHighlight[128],true); 	
			LCDPlacePage(4,&ConfigHomeHighlight[256],true); 		
			LCDPlacePage(5,&ConfigHomeHighlight[384],true); 	
			break;
		}
		default:
		{
			LCDPlacePage(2,&ConfigMiscHighlight[0],true); 
			LCDPlacePage(3,&ConfigMiscHighlight[128],true); 	
			LCDPlacePage(4,&ConfigMiscHighlight[256],true); 		
			LCDPlacePage(5,&ConfigMiscHighlight[384],true); 					
			break;
		}
		
	}	
}

void AppScreenMenuHighlight(uint8_t value)
{
	//------------------------------
	// value can be 0,1,2 
	// value of 1 is on HOME 
	//          0 IS TOOLS 
	//          2 is tires
	//     all others is on HOME. 
	//------------------------------
	switch(value)
	{
		case 0:
		{
			LCDPlacePage(2,&MainMenuToolHighlight[0],true); 
			LCDPlacePage(3,&MainMenuToolHighlight[128],true); 	
			LCDPlacePage(4,&MainMenuToolHighlight[256],true); 		
			LCDPlacePage(5,&MainMenuToolHighlight[384],true); 	
			break;
		}
		case 1:
		{
			LCDPlacePage(2,&MainMenuHomeHighlight[0],true); 
			LCDPlacePage(3,&MainMenuHomeHighlight[128],true); 	
			LCDPlacePage(4,&MainMenuHomeHighlight[256],true); 		
			LCDPlacePage(5,&MainMenuHomeHighlight[384],true); 	
			break;
		}
		default:
		{
			LCDPlacePage(2,&MainMenuTireHighlight[0],true); 
			LCDPlacePage(3,&MainMenuTireHighlight[128],true); 	
			LCDPlacePage(4,&MainMenuTireHighlight[256],true); 		
			LCDPlacePage(5,&MainMenuTireHighlight[384],true); 					
			break;
		}
		
	}	
}








void AppScreenPlaceForce(uint8_t value, uint8_t whichScreen)
{
	uint8_t tempData[15],i,temp,*ptr; 
	
	if (whichScreen == SCREEN_FORCE)
	{
		if (table0.Item.Hybrid == TRUE)
		{
				LCDPlaceData(56,24,FONTLARGE_HTOP,11);				
				LCDPlaceData(56,32,FONTLARGE_HMID,11);
				LCDPlaceData(56,40,FONTLARGE_HBOT,11);			
		}
		else
		{
			if ((value >= 0) && (value <= MAX_FORCE))
			{
				LCDPlaceData(56,24,FONTLARGE[value][TOP],11);				
				LCDPlaceData(56,32,FONTLARGE[value][MID],11);
				LCDPlaceData(56,40,FONTLARGE[value][BOT],11);
			}
		}
	}
	if (whichScreen == SCREEN_HOME)
	{
		if (table0.Item.Hybrid == TRUE)
		{
				ptr = (uint8_t*)FONTMID_HTOP;
				//----------------------------------
				// Get the 4 bits of data from the screen data. 
				// from SCREEN HOME at 
				for (i=0;i<9;i++)
				{
					tempData[i] = ScreenHome[(8*16)+12+i];	
					tempData[i] &= 0xE0;
					temp = *ptr++;
					temp = temp&0x1f; 
					tempData[i] |= temp;
				}
				LCDPlaceData(12,8,tempData,9);   
				LCDPlaceData(12,16,FONTMID_HMID,9);			
		}
		else
		{
			if ((value >= 0) && (value <= MAX_FORCE))
			{		
				ptr = (uint8_t*)FONTMID[value][TOP];
				//----------------------------------
				// Get the 4 bits of data from the screen data. 
				// from SCREEN HOME at 
				for (i=0;i<9;i++)
				{
					tempData[i] = ScreenHome[(8*16)+12+i];	
					tempData[i] &= 0xE0;
					temp = *ptr++;
					temp = temp&0x1f; 
					tempData[i] |= temp;
				}
				LCDPlaceData(12,8,tempData,9);  //FONTMID[value][TOP],9);
				LCDPlaceData(12,16,FONTMID[value][MID],9);
	//			LCDPlaceData(56,40,FONTMID[value][BOT],9);
			}
			else
			{
	//			LCDPlaceData(56,32,0xff,9);
	//			LCDPlaceData(56,40,0xff,9);
			}
		}
	}	
	
} 

void AppScreenUpdateStatusRoot(void)
{
	uint8_t temp,temp2,offset; 
	uint16_t x,y,z,itemp;	
	uint8_t myrssiPeak,myrssiAvg;
	//------------------------
	// UPDATE THE TIRE RADIO status 
	PressureProvidRSSI(&myrssiPeak,&myrssiAvg);	
	if ((statusData.TireRadio & 0x01)!= 0)
	{
		LCDPlaceData(24,8,(uint8_t *)FONTSMALL_ONE,5);  
	}
	else
	{
		LCDPlaceData(24,8,(uint8_t *)FONTSMALL_ZERO,5);  		
	}
	if ((statusData.TireRadio & 0x02)!= 0)
	{
		LCDPlaceData(30,8,(uint8_t *)FONTSMALL_ONE,5);
	}
	else
	{
		LCDPlaceData(30,8,(uint8_t *)FONTSMALL_ZERO,5);
	}	
	if ((statusData.TireRadio & 0x04)!= 0)
	{
		LCDPlaceData(36,8,(uint8_t *)FONTSMALL_ONE,5);
	}
	else
	{
		LCDPlaceData(36,8,(uint8_t *)FONTSMALL_ZERO,5);
	}	
 
	temp = myrssiPeak;
	temp2 = temp & 0x0f;
	temp = temp>>4;
	offset = 48;
	LCDPlaceData(offset,8,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+6,8,(uint8_t *)FONTSMALL[temp2],5);
	temp = myrssiAvg;
	temp2 = temp & 0x0f;
	temp = temp>>4;
	offset = 68;
	LCDPlaceData(offset,8,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+6,8,(uint8_t *)FONTSMALL[temp2],5);		 	
	//------------------------
	// UPDATE THE EEPROM status
	if ((statusData.EEPROM & 0x01)!= 0)
	{
		LCDPlaceData(24,24,(uint8_t *)FONTSMALL_ONE,5);
	}
	else
	{
		LCDPlaceData(24,24,(uint8_t *)FONTSMALL_ZERO,5);
	}
	if ((statusData.EEPROM & 0x02)!= 0)
	{
		LCDPlaceData(30,24,(uint8_t *)FONTSMALL_ONE,5);
	}
	else
	{
		LCDPlaceData(30,24,(uint8_t *)FONTSMALL_ZERO,5);
	}
	if ((statusData.EEPROM & 0x04)!= 0)
	{
		LCDPlaceData(36,24,(uint8_t *)FONTSMALL_ONE,5);
	}
	else
	{
		LCDPlaceData(36,24,(uint8_t *)FONTSMALL_ZERO,5);
	}	
	//------------------------
	// place manufacture, device and serial number 
	offset = 48;
	temp = table0.Item.EepromManDevSerial[0];
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset,24,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+6,24,(uint8_t *)FONTSMALL[temp2],5);
	temp = table0.Item.EepromManDevSerial[1];
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset+12,24,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+18,24,(uint8_t *)FONTSMALL[temp2],5);	
	
	offset = 75;
	temp = table0.Item.EepromManDevSerial[2];
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset,24,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+6,24,(uint8_t *)FONTSMALL[temp2],5);
	temp = table0.Item.EepromManDevSerial[3];
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset+12,24,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+18,24,(uint8_t *)FONTSMALL[temp2],5);	
	offset = 105;
	temp = table0.Item.EepromManDevSerial[4];
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset,24,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+6,24,(uint8_t *)FONTSMALL[temp2],5);
	temp = table0.Item.EepromManDevSerial[5];
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset+12,24,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+18,24,(uint8_t *)FONTSMALL[temp2],5);		
	
	//-----------------ACCELEROMETER
	//------------------------
	// UPDATE THE EEPROM status
	if ((statusData.Accelerometer & 0x01)!= 0)
	{
		LCDPlaceData(24,32,(uint8_t *)FONTSMALL_ONE,5);
	}
	else
	{
		LCDPlaceData(24,32,(uint8_t *)FONTSMALL_ZERO,5);
	}
 
	//------------------------
	// place x y z values 
	AccelProvideReading(&x,&y,&z); 
	offset = 48;
	itemp = x>>8;
	temp = itemp;
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset,32,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+6,32,(uint8_t *)FONTSMALL[temp2],5);
	itemp = x & 0x0f;
	temp = itemp;
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset+12,32,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+18,32,(uint8_t *)FONTSMALL[temp2],5);	
	
	offset = 75;
	itemp = y>>8;
	temp = itemp;
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset,32,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+6,32,(uint8_t *)FONTSMALL[temp2],5);
	itemp = y & 0x0f;
	temp = itemp;
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset+12,32,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+18,32,(uint8_t *)FONTSMALL[temp2],5);	
	
	offset = 105;
	itemp = z>>8;
	temp = itemp;
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset,32,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+6,32,(uint8_t *)FONTSMALL[temp2],5);
	itemp = z & 0x0f;
	temp = itemp;
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset+12,32,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+18,32,(uint8_t *)FONTSMALL[temp2],5);		
	
	//------------------------
	//UPDATE VERSION
	offset = 40;
	temp = FWVER3 & 0x0f; 
	LCDPlaceData(offset,40,(uint8_t *)FONTSMALL[temp],5);	
	temp = FWVER2 & 0x0f;
	LCDPlaceData(offset+6,40,(uint8_t *)FONTSMALL[temp],5);	
	temp = FWVER1 & 0x0f;
	LCDPlaceData(offset+18,40,(uint8_t *)FONTSMALL[temp],5);
	temp = FWVER0 & 0x0f;
	LCDPlaceData(offset+24,40,(uint8_t *)FONTSMALL[temp],5);	
	offset = 76;
	temp = MONTHMSB & 0x0f;
	LCDPlaceData(offset,40,(uint8_t *)FONTSMALL[temp],5);
	temp = MONTHLSB & 0x0f;
	LCDPlaceData(offset+6,40,(uint8_t *)FONTSMALL[temp],5);
	temp = DAYMSB & 0x0f;
	LCDPlaceData(offset+18,40,(uint8_t *)FONTSMALL[temp],5);
	temp = DAYLSB & 0x0f;
	LCDPlaceData(offset+24,40,(uint8_t *)FONTSMALL[temp],5);	
	temp = YEARMSB & 0x0f;
	LCDPlaceData(offset+36,40,(uint8_t *)FONTSMALL[temp],5);
	temp = YEARLSB & 0x0f;
	LCDPlaceData(offset+42,40,(uint8_t *)FONTSMALL[temp],5);		
}	

void AppScreenCarRadioReadingIn(uint8_t which,uint8_t increment)
{
	uint8_t mytemp,temp;
	
	if ((which < MAXSENSORS)&&(increment==TRUE))
	{
		carRadioReading[which]++;
		if (carRadioReading[which] >= 100)
		{
			carRadioReading[which] = 0; 
		}
	}	
	if (appScreen == SCREEN_TIRE_REMOTE1)	
	{
		if (which <4)
		{
			mytemp = carRadioReading[which];
			if (mytemp >9)
			{
				temp = mytemp/10;
				mytemp = mytemp - (temp *10);
				LCDPlaceData(116,(8*which)+16,(uint8_t *)FONTSMALL[temp],5);
			}
			LCDPlaceData(122,(8*which)+16,(uint8_t *)FONTSMALL[mytemp],5);			
		}
	}
	if (appScreen == SCREEN_TIRE_REMOTE2)
	{
		if ((which <8)&&(which>=4))
		{
			mytemp = carRadioReading[which];
			if (mytemp >9)
			{
				temp = mytemp/10;
				mytemp = mytemp - (temp *10);
				LCDPlaceData(116,(8*(which-4))+16,(uint8_t *)FONTSMALL[temp],5);
			}
			LCDPlaceData(122,(8*(which-4))+16,(uint8_t *)FONTSMALL[mytemp],5);
		}
	}	
	if (appScreen == SCREEN_SENSOR)
	{
		ConfigSensorUpdate(which);
	}
}



void AppScreenFSKReadingIn(uint8_t *buffer,uint8_t myoffset)
{
	uint8_t mytemp,temp,temp2,offset;
	
 	fskRadioReading++;
	if (fskRadioReading >= 100)
	{
		fskRadioReading = 0; 
	}

	if (appScreen == SCREEN_STATUS_ROOT)	
	{
		mytemp = fskRadioReading;
		if (mytemp >9)
		{
			temp = mytemp/10;
			mytemp = mytemp - (temp *10);
			LCDPlaceData(30,16,(uint8_t *)FONTSMALL[temp],5);
		}
		LCDPlaceData(36,16,(uint8_t *)FONTSMALL[mytemp],5);	
		//-------------------------------
		// place some of data on screen 
		//-------------------------------
	//------------------------
	// place manufacture, device and serial number
	offset = 48;
	temp = buffer[0];
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset,16,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+6,16,(uint8_t *)FONTSMALL[temp2],5);
	temp = buffer[1];
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset+12,16,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+18,16,(uint8_t *)FONTSMALL[temp2],5);
	
	offset = 75;
	temp = buffer[2];
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset,16,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+6,16,(uint8_t *)FONTSMALL[temp2],5);
	temp = buffer[3];
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset+12,16,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+18,16,(uint8_t *)FONTSMALL[temp2],5);
	
	offset = 105;
	temp = buffer[4];
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset,16,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+6,16,(uint8_t *)FONTSMALL[temp2],5);
	temp = buffer[5];
	temp2 = temp & 0x0f;
	temp = temp>>4;
	LCDPlaceData(offset+12,16,(uint8_t *)FONTSMALL[temp],5);
	LCDPlaceData(offset+18,16,(uint8_t *)FONTSMALL[temp2],5);		
		
				
	}
}

//-----------------------------------------brake screens 

void AppScreenUpdateStatusBrake(void)
{
	uint8_t temp,temp2,offset,mytemp,digit;
	uint16_t x,y,z,itemp,myrx,mytx;
//	uint8_t myrssiPeak,myrssiAvg;
	//------------------------
	// UPDATE THE TIRE RADIO status	
//	PressureProvidRSSI(&myrssiPeak,&myrssiAvg);

	if (appScreen == SCREEN_BRAKE_STATUS)
	{
		if ((statusBrake.TireRadio & 0x01)!= 0)
		{
			LCDPlaceData(24,8,(uint8_t *)FONTSMALL_ONE,5);
		}
		else
		{
			LCDPlaceData(24,8,(uint8_t *)FONTSMALL_ZERO,5);
		}
		if ((statusBrake.TireRadio & 0x02)!= 0)
		{
			LCDPlaceData(30,8,(uint8_t *)FONTSMALL_ONE,5);
		}
		else
		{
			LCDPlaceData(30,8,(uint8_t *)FONTSMALL_ZERO,5);
		}
		if ((statusBrake.TireRadio & 0x04)!= 0)
		{
			LCDPlaceData(36,8,(uint8_t *)FONTSMALL_ONE,5);
		}
		else
		{
			LCDPlaceData(36,8,(uint8_t *)FONTSMALL_ZERO,5);
		}
	//----------------------------------
	// add count of messages on screen 
	if (whichRadio == WHICHRADIO_LORA)
	{
		LORAGetCounts(&mytx,&myrx);
	}
	else
	{
		FSKGetCounts(&mytx,&myrx);
	}
		mytemp = mytx; 
		digit = 0;
		if (mytemp >9999)
		{
			temp = mytemp/10000;
			mytemp = mytemp - (temp *10000);
			digit = 1; 
			LCDPlaceData(30,16,(uint8_t *)FONTSMALL[temp],5);
		}						
		if (mytemp >999)
		{
			temp = mytemp/1000;
			mytemp = mytemp - (temp *1000);
			digit = 1; 
			LCDPlaceData(36,16,(uint8_t *)FONTSMALL[temp],5);
		}	
		else
		{
			if (digit!= 0)
			{
				LCDPlaceData(36,16,(uint8_t *)FONTSMALL[0],5);
			}
		}					
		if (mytemp >99)
		{
			temp = mytemp/100;
			mytemp = mytemp - (temp *100);
			digit = 1; 
			LCDPlaceData(42,16,(uint8_t *)FONTSMALL[temp],5);
		}		
		else
		{
			if (digit!= 0)
			{
				LCDPlaceData(42,16,(uint8_t *)FONTSMALL[0],5);
			}
		}
		if (mytemp >9)
		{
			temp = mytemp/10;
			mytemp = mytemp - (temp *10);
			LCDPlaceData(48,16,(uint8_t *)FONTSMALL[temp],5);
		}
		else
		{
			if (digit!= 0)
			{
				LCDPlaceData(48,16,(uint8_t *)FONTSMALL[0],5);
			}
		}		
		LCDPlaceData(54,16,(uint8_t *)FONTSMALL[mytemp],5);			
	 
		mytemp = myrx; 
		if (mytemp >9)
		{
			temp = mytemp/10;
			mytemp = mytemp - (temp *10);
			LCDPlaceData(78,16,(uint8_t *)FONTSMALL[temp],5);
		}
		LCDPlaceData(84,16,(uint8_t *)FONTSMALL[mytemp],5);		
	/*	
		temp = myrssiPeak;
		temp2 = temp & 0x0f;
		temp = temp>>4;
		offset = 48;
		LCDPlaceData(offset,8,FONTSMALL[temp],5);
		LCDPlaceData(offset+6,8,FONTSMALL[temp2],5);
		temp = myrssiAvg;
		temp2 = temp & 0x0f;
		temp = temp>>4;
		offset = 68;
		LCDPlaceData(offset,8,FONTSMALL[temp],5);
		LCDPlaceData(offset+6,8,FONTSMALL[temp2],5);
	*/	
		//------------------------
		// UPDATE THE EEPROM status
		if ((statusBrake.EEPROM & 0x01)!= 0)
		{
			LCDPlaceData(24,24,(uint8_t *)FONTSMALL_ONE,5);
		}
		else
		{
			LCDPlaceData(24,24,(uint8_t *)FONTSMALL_ZERO,5);
		}
		if ((statusBrake.EEPROM & 0x02)!= 0)
		{
			LCDPlaceData(30,24,(uint8_t *)FONTSMALL_ONE,5);
		}
		else
		{
			LCDPlaceData(30,24,(uint8_t *)FONTSMALL_ZERO,5);
		}
		if ((statusBrake.EEPROM & 0x04)!= 0)
		{
			LCDPlaceData(36,24,(uint8_t *)FONTSMALL_ONE,5);
		}
		else
		{
			LCDPlaceData(36,24,(uint8_t *)FONTSMALL_ZERO,5);
		}
		//------------------------
		// place manufacture, device and serial number
		offset = 48;
		temp = brakesEEPROMData[0];
		temp2 = temp & 0x0f;
		temp = temp>>4;
		LCDPlaceData(offset,24,(uint8_t *)FONTSMALL[temp],5);
		LCDPlaceData(offset+6,24,(uint8_t *)FONTSMALL[temp2],5);
		temp = brakesEEPROMData[1];
		temp2 = temp & 0x0f;
		temp = temp>>4;
		LCDPlaceData(offset+12,24,(uint8_t *)FONTSMALL[temp],5);
		LCDPlaceData(offset+18,24,(uint8_t *)FONTSMALL[temp2],5);
	
		offset = 75;
		temp = brakesEEPROMData[2];
		temp2 = temp & 0x0f;
		temp = temp>>4;
		LCDPlaceData(offset,24,(uint8_t *)FONTSMALL[temp],5);
		LCDPlaceData(offset+6,24,(uint8_t *)FONTSMALL[temp2],5);
		temp = brakesEEPROMData[3];
		temp2 = temp & 0x0f;
		temp = temp>>4;
		LCDPlaceData(offset+12,24,(uint8_t *)FONTSMALL[temp],5);
		LCDPlaceData(offset+18,24,(uint8_t *)FONTSMALL[temp2],5);
		offset = 105;
		temp = brakesEEPROMData[4];
		temp2 = temp & 0x0f;
		temp = temp>>4;
		LCDPlaceData(offset,24,(uint8_t *)FONTSMALL[temp],5);
		LCDPlaceData(offset+6,24,(uint8_t *)FONTSMALL[temp2],5);
		temp = brakesEEPROMData[5];
		temp2 = temp & 0x0f;
		temp = temp>>4;
		LCDPlaceData(offset+12,24,(uint8_t *)FONTSMALL[temp],5);
		LCDPlaceData(offset+18,24,(uint8_t *)FONTSMALL[temp2],5);
	
		//-----------------ACCELEROMETER
		//------------------------
		// UPDATE THE EEPROM status
		if ((statusBrake.Accelerometer & 0x01)!= 0)
		{
			LCDPlaceData(24,32,(uint8_t *)FONTSMALL_ONE,5);
		}
		else
		{
			LCDPlaceData(24,32,(uint8_t *)FONTSMALL_ZERO,5);
		}
	
		//------------------------
		// place x y z values
	//	AccelProvideReading(&x,&y,&z);
		x = brakesAccelData[0];
		y = brakesAccelData[1];
		z = brakesAccelData[2];	
		offset = 48;
		itemp = x>>8;
		temp = itemp;
		temp2 = temp & 0x0f;
		temp = temp>>4;
		LCDPlaceData(offset,32,(uint8_t *)FONTSMALL[temp],5);
		LCDPlaceData(offset+6,32,(uint8_t *)FONTSMALL[temp2],5);
		itemp = x & 0x0f;
		temp = itemp;
		temp2 = temp & 0x0f;
		temp = temp>>4;
		LCDPlaceData(offset+12,32,(uint8_t *)FONTSMALL[temp],5);
		LCDPlaceData(offset+18,32,(uint8_t *)FONTSMALL[temp2],5);
	
		offset = 75;
		itemp = y>>8;
		temp = itemp;
		temp2 = temp & 0x0f;
		temp = temp>>4;
		LCDPlaceData(offset,32,(uint8_t *)FONTSMALL[temp],5);
		LCDPlaceData(offset+6,32,(uint8_t *)FONTSMALL[temp2],5);
		itemp = y & 0x0f;
		temp = itemp;
		temp2 = temp & 0x0f;
		temp = temp>>4;
		LCDPlaceData(offset+12,32,(uint8_t *)FONTSMALL[temp],5);
		LCDPlaceData(offset+18,32,(uint8_t *)FONTSMALL[temp2],5);
	
		offset = 105;
		itemp = z>>8;
		temp = itemp;
		temp2 = temp & 0x0f;
		temp = temp>>4;
		LCDPlaceData(offset,32,(uint8_t *)FONTSMALL[temp],5);
		LCDPlaceData(offset+6,32,(uint8_t *)FONTSMALL[temp2],5);
		itemp = z & 0x0f;
		temp = itemp;
		temp2 = temp & 0x0f;
		temp = temp>>4;
		LCDPlaceData(offset+12,32,(uint8_t *)FONTSMALL[temp],5);
		LCDPlaceData(offset+18,32,(uint8_t *)FONTSMALL[temp2],5);
	
		//------------------------
		//UPDATE VERSION
		offset = 40;
		temp = brakesVersionDate[0] & 0x0f;
		LCDPlaceData(offset,40,(uint8_t *)FONTSMALL[temp],5);
		temp = brakesVersionDate[1]  & 0x0f;
		LCDPlaceData(offset+6,40,(uint8_t *)FONTSMALL[temp],5);
		temp = brakesVersionDate[2]  & 0x0f;
		LCDPlaceData(offset+18,40,(uint8_t *)FONTSMALL[temp],5);
		temp = brakesVersionDate[3]  & 0x0f;
		LCDPlaceData(offset+24,40,(uint8_t *)FONTSMALL[temp],5);
		offset = 76;
		temp = brakesVersionDate[4]  & 0x0f;
		LCDPlaceData(offset,40,(uint8_t *)FONTSMALL[temp],5);
		temp = brakesVersionDate[5]  & 0x0f;
		LCDPlaceData(offset+6,40,(uint8_t *)FONTSMALL[temp],5);
		temp = brakesVersionDate[6]  & 0x0f;
		LCDPlaceData(offset+18,40,(uint8_t *)FONTSMALL[temp],5);
		temp = brakesVersionDate[7]  & 0x0f;
		LCDPlaceData(offset+24,40,(uint8_t *)FONTSMALL[temp],5);
		temp = brakesVersionDate[8]  & 0x0f;
		LCDPlaceData(offset+36,40,(uint8_t *)FONTSMALL[temp],5);
		temp = brakesVersionDate[9]  & 0x0f;
		LCDPlaceData(offset+42,40,(uint8_t *)FONTSMALL[temp],5);
	}
}

void PlaceDownloadPacket(uint16_t packetnumber)
{
	uint16_t mytemp;
	uint8_t temp,digit;
	 
	
	
		mytemp = packetnumber; 
		digit = 0;
		if (mytemp >9999)
		{
			temp = mytemp/10000;
			mytemp = mytemp - (temp *10000);
			digit = 1; 
			LCDPlaceData(30,16,(uint8_t *)FONTSMALL[temp],5);
		}						
		if (mytemp >999)
		{
			temp = mytemp/1000;
			mytemp = mytemp - (temp *1000);
			digit = 1; 
			LCDPlaceData(36,16,(uint8_t *)FONTSMALL[temp],5);
		}	
		else
		{
			if (digit!= 0)
			{
				LCDPlaceData(36,16,(uint8_t *)FONTSMALL[0],5);
			}
		}					
		if (mytemp >99)
		{
			temp = mytemp/100;
			mytemp = mytemp - (temp *100);
			digit = 1; 
			LCDPlaceData(42,16,(uint8_t *)FONTSMALL[temp],5);
		}		
		else
		{
			if (digit!= 0)
			{
				LCDPlaceData(42,16,(uint8_t *)FONTSMALL[0],5);
			}
		}
		if (mytemp >9)
		{
			temp = mytemp/10;
			mytemp = mytemp - (temp *10);
			LCDPlaceData(48,16,(uint8_t *)FONTSMALL[temp],5);
		}
		else
		{
			if (digit!= 0)
			{
				LCDPlaceData(48,16,(uint8_t *)FONTSMALL[0],5);
			}
		}		
		LCDPlaceData(54,16,(uint8_t *)FONTSMALL[mytemp],5);		
}
			 			
uint8_t timerPressureTemp,mask,maxheight,width; 
uint8_t prevX = 0;		
void AppScreenUpdateHome(void)
{
	uint16_t x,i;
	uint8_t done,offset,temp;
	uint8_t *wordList[20];
	uint8_t j,tempData[6],*ptr;	 
	 
	done = 0; 

/*	
	//-----------------------------
	// do swap on pressure/temperature 
	// every second
	timerPressureTemp++;
	if (timerPressureTemp >20)
	{
		timerPressureTemp = 0;
		if (pressureSwap == 0)
		{
			pressureSwap = 1; 
		}
		else
		{
			pressureSwap = 0;
		}
		if (appScreen == SCREEN_COACH)
		{
			AppScreenInit(SCREEN_COACH);
		}		
		if (appScreen == SCREEN_TOWCAR)
		{
			AppScreenInit(SCREEN_TOWCAR);
		}			
	}
*/
//	if ((brakeStatus.BrakeState	 & BRAKESTATE_COMMERROR)!= 0)
//	{
//		refreshScreen(ScreenHomeCommError,1);  	
//		BacklightSetHomeColor(table0.Item.ScreenColor);		
//		appScreenHomeType = SCREEN_HOME_COMMERROR; 
//		done = 1; 
//	}		
	//-----------------------------------------
	// if breakaway has been cleared - still want 
	// background to be RED. 
	if (((brakeStatus.BrakeState & BRAKESTATE_COMMERROR)==0) && (done == 0) &&
		((brakeState == BRAKESTATE_ERROR)||(brakeState == BRAKESTATE_ERROR_FINAL)||
		(brakeState == BRAKESTATE_ERROR_VOLTAGE_ACTIVE)||
		(brakeState == BRAKESTATE_ERROR_VOLTAGE_RESET)))
	{
		BacklightSetColor(5);	
		if ((appScreen == SCREEN_HOME)&&(appScreenHomeType == SCREEN_HOME_BASE))
		{
		//----------------------------
		// send message to screen 
			offset = 40;
			tempData[5] = 0x00;
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLS];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[4] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
			wordList[5] = (uint8_t *)FONTSMALL_BLANK;
			wordList[6] = (uint8_t *)FONTSMALL[OFFSET_SMALLB];
			wordList[7] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
			wordList[8] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
			wordList[9] = (uint8_t *)FONTSMALL[OFFSET_SMALLK];
			wordList[10] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			 
			for (j=0;j<11;j++)
			{
				ptr = wordList[j];
				for (i=0;i<5;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(offset,8,tempData,6);
				offset+=6;
			}
		}
	}	
	else
	{
		if ((appScreen == SCREEN_HOME)&&(appScreenHomeType == SCREEN_HOME_BASE)&&(done == 0))
		{
			offset = 40; 
			for (j=0;j<11;j++)
			{
				ptr = FONTSMALL_BLANK;
				for (i=0;i<6;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(offset,8,tempData,6);
				offset+=6;
			}		
		}
		if ((brakeStatus.BrakeState & BRAKESTATE_BREAKAWAYTIP)!= 0)
		{
			BacklightSetColor(4);	
		}	
		else
		{
			if ((appScreenHomeType == SCREEN_HOME_MANUALBRAKE)||(brakeState == BRAKESTATE_ACTIVE_HOLD)||(brakeState == BRAKESTATE_ACTIVE_EXTEND))
			{
				if (table0.Item.BrakeBacklight == TRUE)
				{
					BacklightSetColor(7);
				}
				else
				{
					//was this BacklightSetColor(5);
					BacklightSetHomeColor(table0.Item.ScreenColor);		
				}
			}
			else
			{
				BacklightSetHomeColor(table0.Item.ScreenColor);		
			}
		}
	}
	//-----------------override the current screen 
	// * if it is a breakaway  or a manual brake event. 
	if (((brakeState == BRAKESTATE_ACTIVE_HOLD_BREAKAWAY)||(brakeState == BRAKESTATE_ACTIVE_EXTEND_BREAKAWAY)
				||(brakeState == BRAKESTATE_END_RETRACT_BREAKAWAY))
		&&(eventMessageReceived != 0)&&(done==0))
	{
		done = 1;
		eventMessageReceived = 0; 
		if (appScreen!= SCREEN_HOME)
		{
			AppScreenInit(SCREEN_HOME);
		}
		refreshScreen(ScreenHomeBreakawayError,1);
		BacklightSetColor(4);
		appScreenHomeType = SCREEN_HOME_BREAKAWAYTIP;
	}
	 
	if (((brakeState == BRAKESTATE_ACTIVE_HOLD_MANUAL)||(brakeState == BRAKESTATE_ACTIVE_EXTEND_MANUAL)
		||(brakeState == BRAKESTATE_END_RETRACT_MANUAL))
			&&(eventMessageReceived != 0) &&(done==0))
	{
		done = 1;
		eventMessageReceived = 0; 
		if (appScreen!= SCREEN_HOME)
		{
			AppScreenInit(SCREEN_HOME);
		}
		refreshScreen(ScreenHomeManualBrake,1);
		BacklightSetColor(5);
		appScreenHomeType = SCREEN_HOME_MANUALBRAKE;
	}
 
	
	if (appScreen == SCREEN_STATUS_SKELETON)
	{
		AppSkeletonBrakeState();
	}
	if ((appScreen == SCREEN_HOME)&&(eventMessageReceived != 0)&&(done==0)) 
	{
		eventMessageReceived = 0; 

		if ((brakeStatus.BrakeState	 & BRAKESTATE_COMMERROR)!= 0)
		{
			refreshScreen(ScreenHomeCommError,1);  	
			BacklightSetHomeColor(table0.Item.ScreenColor);		
			appScreenHomeType = SCREEN_HOME_COMMERROR; 
			done = 1; 
		}	
		else
		{
			if ((brakeStatus.BrakeState	 & BRAKESTATE_INPUTVOLTAGEBAD)!= 0)
			{
				refreshScreen(ScreenHomeVoltageError,1);  	
				BacklightSetHomeColor(table0.Item.ScreenColor);
				appScreenHomeType = SCREEN_HOME_INPUTVOLTAGEBAD; 
				BacklightSetColor(5);	
				done = 1; 		
			}	
			if (((brakeStatus.BrakeState & BRAKESTATE_NOTSETUP)!= 0)&&(done==0))
			{
				done = 1; 
				if (brakeState == BRAKESTATE_ERROR)
				{
					refreshScreen(ScreenHomeSetupGoneWrong,1);  							
				}
				else
				{
					refreshScreen(ScreenHomeSetupError,1);  		
					BacklightSetHomeColor(table0.Item.ScreenColor);
				}
				appScreenHomeType = SCREEN_HOME_NOTSETUP; 
			}	

			if (done ==0)
			{
				if (appScreenHomeType != SCREEN_HOME_BASE)
				{
					refreshScreen(ScreenHome,1);  	
					AppScreenPlaceForce(table0.Item.MaxForce,SCREEN_HOME);		
					BacklightSetHomeColor(table0.Item.ScreenColor);	
					if ((appScreenHomeType != SCREEN_HOME_BREAKAWAYTIP)||(brakeState == BRAKESTATE_ACTIVE))	
					{
						appScreenHomeType = SCREEN_HOME_BASE;
					}
				}
				if ((brakeStatus.BrakeState & BRAKESTATE_BREAKAWAYREADY)!= 0)
				{
					//-------------READY
					LCDPlaceData(0,32,(uint8_t *)BREAKAWAY,32);	
					LCDPlaceData(0,40,(uint8_t *)READY,32);						
				}
				else
				{
					LCDPlaceData(0,32,(uint8_t *)CLEARSECTION,32);	
					LCDPlaceData(0,40,(uint8_t *)CLEARSECTION,32);					
				}
			}								
		}	
#if BLUETOOTH_TEST		
		offset = 60;
//		temp = FWVER3 & 0x0f;
//		LCDPlaceData(offset,0,(uint8_t *)FONTSMALL[temp],5);
		temp = FWVER2  & 0x0f;
		LCDPlaceData(offset+6,0,(uint8_t *)FONTSMALL[temp],5);
		LCDPlaceData(offset+12,0,(uint8_t *)FONTSMALL_DOT,5);
		temp = FWVER1 & 0x0f;
		LCDPlaceData(offset+18,0,(uint8_t *)FONTSMALL[temp],5);
		temp = FWVER0 & 0x0f;
		LCDPlaceData(offset+24,0,(uint8_t *)FONTSMALL[temp],5);				
		offset = 90;
//		temp = brakesVersionDate[0] & 0x0f;
//		LCDPlaceData(offset,0,(uint8_t *)FONTSMALL[temp],5);
		temp = brakesVersionDate[1]  & 0x0f;
		LCDPlaceData(offset+6,0,(uint8_t *)FONTSMALL[temp],5);
		LCDPlaceData(offset+12,0,(uint8_t *)FONTSMALL_DOT,5);
		temp = brakesVersionDate[2]  & 0x0f;
		LCDPlaceData(offset+18,0,(uint8_t *)FONTSMALL[temp],5);
		temp = brakesVersionDate[3]  & 0x0f;
		LCDPlaceData(offset+24,0,(uint8_t *)FONTSMALL[temp],5);		
		if (whichRadio == WHICHRADIO_FSK)
		{
			LCDPlaceData(offset+34,0,(uint8_t *)FONTSMALL[OFFSET_SMALLF],5);		
		}
		else
		{
			LCDPlaceData(offset+34,0,(uint8_t *)FONTSMALL[OFFSET_SMALLL],5);
		}
#endif	
		x = gPrime;
		if (x > 50)
		{
			x = 50;
		}
		//--------------show ACTIVE BRAKE 
		if ((brakeState == BRAKESTATE_ACTIVE_EXTEND)||(brakeState == BRAKESTATE_ACTIVE_HOLD))
		{
		//----------------------------
		// send message to screen 
			offset = 46;
			tempData[5] = 0x00;
			wordList[0] = (uint8_t *)FONTSMALL[OFFSET_SMALLB];
			wordList[1] = (uint8_t *)FONTSMALL[OFFSET_SMALLR];
			wordList[2] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
			wordList[3] = (uint8_t *)FONTSMALL[OFFSET_SMALLK];
			wordList[4] = (uint8_t *)FONTSMALL[OFFSET_SMALLE];
			wordList[5] = (uint8_t *)FONTSMALL_BLANK;
			wordList[6] = (uint8_t *)FONTSMALL[OFFSET_SMALLA];
			wordList[7] = (uint8_t *)FONTSMALL[OFFSET_SMALLC];
			wordList[8] = (uint8_t *)FONTSMALL[OFFSET_SMALLT];
			wordList[9] = (uint8_t *)FONTSMALL[OFFSET_SMALLI];
			wordList[10] = (uint8_t *)FONTSMALL[OFFSET_SMALLV];
			wordList[11] = (uint8_t *)FONTSMALL[OFFSET_SMALLE]; 
			for (j=0;j<12;j++)
			{
				ptr = wordList[j];
				for (i=0;i<5;i++)
				{
					tempData[i] = *ptr++;
				}
				LCDPlaceData(offset,0,tempData,6);
				offset+=6;
			}
		}
		else
		{
			if ((appScreen == SCREEN_HOME)&&(appScreenHomeType == SCREEN_HOME_BASE))
			{
				offset = 46; 
				for (j=0;j<12;j++)
				{
					ptr = FONTSMALL_BLANK;
					for (i=0;i<6;i++)
					{
						tempData[i] = *ptr++;
					}
					LCDPlaceData(offset,0,tempData,6);
					offset+=6;
				}		
			}
		}		
					
 		if ((prevX != x)&&((brakeState == BRAKESTATE_ACTIVE_EXTEND)||(brakeState == BRAKESTATE_ACTIVE_HOLD)))
		{
			prevX = x; 
			for (i=0;i<4;i++)
			{
				LCDPlaceData(48,(2+i)*8,(uint8_t *)&ScreenHome[((2+i)*128)+48],80);
			}
			if (prevX >0)
			{
				maxheight = prevX/2; 
				maxheight = maxheight/8; 
				width = prevX;
				for (i=0;i<prevX;i++)
				{
					mask = 0;
					temp = ScreenHome[(5*128)+64+i];
					if ((prevX > 16)&&(i>16))
					{
						mask = 0xff; 
					}
					else
					{
						mask = GetMask(prevX/2,i);
					}
					temp |= mask; 
					LCDPlaceData(64+i,5*8,&temp,1);
				}
				if (prevX>16)
				{
					width = prevX-16;
					for (i=0;i<width;i++)
					{
						mask = 0;
						temp = ScreenHome[(4*128)+64+i+16];
						if ((width > 16)&&(i>16))
						{
							mask = 0xff; 
						}
						else
						{
							mask = GetMask(width/2,i);
						}
						temp |= mask; 
						LCDPlaceData(64+i+16,4*8,&temp,1);
					}			
				}
				if (prevX>32)
				{
					width = prevX-32;
					for (i=0;i<width;i++)
					{
						mask = 0;
						temp = ScreenHome[(3*128)+64+i+32];
						if ((width > 16)&&(i>16))
						{
							mask = 0xff; 
						}
						else
						{
							mask = GetMask(width/2,i);
						}
						temp |= mask; 
						LCDPlaceData(64+i+32,3*8,&temp,1);
					}			
				}		
				if (prevX>48)
				{
					width = prevX-48;
					for (i=0;i<width;i++)
					{
						mask = 0;
						temp = ScreenHome[(2*128)+64+i+48];
						if ((width > 16)&&(i>16))
						{
							mask = 0xff; 
						}
						else
						{
							mask = GetMask(width/2,i);
						}
						temp |= mask; 
						LCDPlaceData(64+i+48,2*8,&temp,1);
					}			
				}										
			} 
			
		}
 		if  ((brakeState != BRAKESTATE_ACTIVE_EXTEND)&&(brakeState != BRAKESTATE_ACTIVE_HOLD))
		{
			if (prevX != 0)
			{
				prevX = 0; 
				for (i=0;i<4;i++)
				{
					LCDPlaceData(48,(2+i)*8,(uint8_t *)&ScreenHome[((2+i)*128)+48],80);
				}
			}
		}
	}
} 			


uint8_t GetMask(uint8_t offset,uint8_t i)
{
	uint8_t mask; 
	mask = 0; 
						switch(offset)
						{
							case 0:
							{
								mask = 0; 
								break;
							}
							case 1:
							{
								mask = 0x01;
								break;
							}
							case 2:
							{
								mask = 0x03;
								if (i<2)
								{
									mask = 0x01; 
								}
								break;
							}		
							case 3:
							{
								mask = 0x07;
								if (i<4)
								{
									mask = 0x03; 
									if (i<2)
									{
										mask = 0x01; 
									}
								}								
								break;
							}
							case 4:
							{
								mask = 0x0f;
								if (i<6)
								{			
									mask = 0x07;					
									if (i<4)
									{
										mask = 0x03; 
										if (i<2)
										{
											mask = 0x01; 
										}
									}		
								}
								break;
							}	
							case 5:
							{
								mask = 0x1f;
								if (i<8)
								{
									mask = 0x0f; 
									if (i<6)
									{			
										mask = 0x07;					
										if (i<4)
										{
											mask = 0x03; 
											if (i<2)
											{
												mask = 0x01; 
											}
										}		
									}	
								}
								break;
							}
							case 6:
							{
								mask = 0x3f;
								if (i<10)
								{
									mask = 0x1f; 
									if (i<8)
									{
										mask = 0x0f; 
										if (i<6)
										{			
											mask = 0x07;					
											if (i<4)
											{
												mask = 0x03; 
												if (i<2)
												{
													mask = 0x01; 
												}
											}		
										}	
									}						
								}
								break;
							}		
							case 7:
							{
								mask = 0x7f;
								if (i<12)
								{
									mask = 0x3f; 
									if (i<10)
									{
										mask = 0x1f; 
										if (i<8)
										{
											mask = 0x0f; 
											if (i<6)
											{			
												mask = 0x07;					
												if (i<4)
												{
													mask = 0x03; 
													if (i<2)
													{
														mask = 0x01; 
													}
												}		
											}	
										}						
									}				
								}
								break;
							}
							default:
							{
								mask = 0xff;
								if (i<14)
								{
									mask = 0x7f; 
									if (i<12)
									{
										mask = 0x3f; 
										if (i<10)
										{
											mask = 0x1f; 
											if (i<8)
											{
												mask = 0x0f; 
												if (i<6)
												{			
													mask = 0x07;					
													if (i<4)
													{
														mask = 0x03; 
														if (i<2)
														{
															mask = 0x01; 
														}
													}		
												}	
											}						
										}				
									}					
								}
								break;
							}																							
						} 
	return mask; 
}

#endif


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ARCHIVED CODE XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#if REMOTE_TIREON
void AppScreenPlacePressure(void)
{
	uint8_t tempData[15],i,temp,*ptr,*ptr2,temp2;
	uint8_t veroffset,numDigits,digits[3],horoffset,k,j;
	uint8_t myvalue;
	
	
	if (appScreen == SCREEN_TOWCAR)
	{
		refreshScreen(TowMenu,1);
		// ---- OFFSETS 1,2,11,12   2 and 11 are front wheels
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
					numDigits = AppGetPressureConverted(1,digits,1);
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
					numDigits = AppGetPressureConverted(0,digits,1);
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
					numDigits = AppGetPressureConverted(10,digits,1);
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
					numDigits = AppGetPressureConverted(11,digits,1);
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
						ptr = (uint8_t*)FONTMID[myvalue][TOP];
						ptr2 = (uint8_t*)FONTMID[myvalue][MID];
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
						ptr = (uint8_t*)FONTMID[myvalue][MID];
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
						ptr = (uint8_t*)FONTMID[myvalue][TOP];
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
	
	if (appScreen == SCREEN_COACH)
	{
		refreshScreen(CabMenuPSI,1);
		// ---- OFFSETS 3 4 5 6   AND 7 8 9 10   6 and 7 are front wheels
		//---------------------------------
		for (k=0; k<8;k++)
		{
			switch (k)
			{
				case 0:
				{
					horoffset = 11;
					veroffset = 16;
					numDigits = AppGetPressureConverted(5,digits,1);
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
					veroffset = 40;
					numDigits = AppGetPressureConverted(2,digits,1);
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
					horoffset = 85;
					veroffset = 16;
					numDigits = AppGetPressureConverted(6,digits,1);
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
					horoffset = 85;
					veroffset = 40;
					numDigits = AppGetPressureConverted(9,digits,1);
					#if TEST_PRESSURE
					//---------testing
					numDigits = 3;
					digits[0] = 0;
					digits[1] = 0;
					digits[2] = 3;
					#endif
					break;
				}
				case 4:
				{
					horoffset = 6;
					veroffset = 32;
					numDigits = AppGetPressureConverted(3,digits,1);
					#if TEST_PRESSURE
					//---------testing
					numDigits = 3;
					digits[0] = 1;
					digits[1] = 2;
					digits[2] = 3;
					#endif
					break;
				}
				case 5:
				{
					horoffset = 30;
					veroffset = 32;
					numDigits = AppGetPressureConverted(4,digits,1);
					#if TEST_PRESSURE
					//---------testing
					numDigits = 3;
					digits[0] = 4;
					digits[1] = 5;
					digits[2] = 6;
					#endif
					break;
				}
				case 6:
				{
					horoffset = 80;
					veroffset = 32;
					numDigits = AppGetPressureConverted(7,digits,1);
					#if TEST_PRESSURE
					//---------testing
					numDigits = 3;
					digits[0] = 7;
					digits[1] = 8;
					digits[2] = 9;
					#endif
					break;
				}
				case 7:
				{
					horoffset = 104;
					veroffset = 32;
					numDigits = AppGetPressureConverted(8,digits,1);
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
			
			numDigits = 3;
			//			if ((numDigits>0)&&((k==0)||(k==2)))
			//			{
			for (j=0;j<numDigits;j++)
			{
				myvalue = digits[j];
				if (myvalue != ' ')
				{
					ptr = (uint8_t*)FONTSMALL[myvalue];
					LCDPlaceData(horoffset,veroffset,ptr,6);
				}
				else
				{
					ptr = (uint8_t*)FONTSMALL_BLANK;
					LCDPlaceData(horoffset,veroffset,ptr,6);
				}
				horoffset+=6;
			}
			//			}
			
		}
	}
}
#endif

#if REMOTE_TIREON
void AppScreenPlaceTemperature(void)
{
	uint8_t tempData[15],i,temp,*ptr,*ptr2,temp2;
	uint8_t veroffset,numDigits,digits[3],horoffset,k,j;
	uint8_t myvalue;
	
	
	if (appScreen == SCREEN_TOWCAR)
	{
		refreshScreen(TowMenuDegreeC,1);
		// ---- OFFSETS 1,2,11,12   2 and 11 are front wheels
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
					numDigits = AppGetTemperatureConverted(1,digits,1);
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
					numDigits = AppGetTemperatureConverted(0,digits,1);
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
					numDigits = AppGetTemperatureConverted(10,digits,1);
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
					numDigits = AppGetTemperatureConverted(11,digits,1);
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
						ptr = (uint8_t*)FONTMID[myvalue][TOP];
						ptr2 = (uint8_t*)FONTMID[myvalue][MID];
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
						ptr = (uint8_t*)FONTMID[myvalue][MID];
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
						ptr = (uint8_t*)FONTMID[myvalue][TOP];
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
	
	if (appScreen == SCREEN_COACH)
	{
		refreshScreen(CabMenuDegreeC,1);
		// ---- OFFSETS 3 4 5 6   AND 7 8 9 10   6 and 7 are front wheels
		//---------------------------------
		for (k=0; k<8;k++)
		{
			switch (k)
			{
				case 0:
				{
					horoffset = 11;
					veroffset = 16;
					numDigits = AppGetTemperatureConverted(5,digits,1);
					break;
				}
				case 1:
				{
					horoffset = 11;
					veroffset = 40;
					numDigits = AppGetTemperatureConverted(2,digits,1);
					break;
				}
				case 2:
				{
					horoffset = 85;
					veroffset = 16;
					numDigits = AppGetTemperatureConverted(6,digits,1);
					break;
				}
				case 3:
				{
					horoffset = 85;
					veroffset = 40;
					numDigits = AppGetTemperatureConverted(9,digits,1);
					break;
				}
				case 4:
				{
					horoffset = 6;
					veroffset = 32;
					numDigits = AppGetTemperatureConverted(3,digits,1);
					break;
				}
				case 5:
				{
					horoffset = 30;
					veroffset = 32;
					numDigits = AppGetTemperatureConverted(4,digits,1);
					break;
				}
				case 6:
				{
					horoffset = 80;
					veroffset = 32;
					numDigits = AppGetTemperatureConverted(7,digits,1);
					break;
				}
				case 7:
				{
					horoffset = 104;
					veroffset = 32;
					numDigits = AppGetTemperatureConverted(8,digits,1);
					break;
				}
			}
			
			numDigits = 3;
			//			if ((numDigits>0)&&((k==0)||(k==2)))
			//			{
			for (j=0;j<numDigits;j++)
			{
				myvalue = digits[j];
				if (myvalue != ' ')
				{
					ptr = (uint8_t*)FONTSMALL[myvalue];
					LCDPlaceData(horoffset,veroffset,ptr,6);
				}
				else
				{
					ptr = (uint8_t*)FONTSMALL_BLANK;
					LCDPlaceData(horoffset,veroffset,ptr,6);
				}
				horoffset+=6;
			}
			//			}
			
		}
	}
}
#endif 
#if REMOTE_TIREON
void AppScreenUpdateCarRadio(void)
{
	uint8_t i,temp,temp2,numDigits,digits[3],offset,k,m;
 
 
	if (appScreen == SCREEN_TIRE_REMOTE1)
	{
		refreshScreen(SystemTireRadioRemote1,1);
		k = 0;
	}
	if (appScreen == SCREEN_TIRE_REMOTE2)
	{
		k = 4; 
		refreshScreen(SystemTireRadioRemote2,1);
	}	

/* ---------------------testing 
	sensor[0].ID[0] = 0x12; 
	sensor[0].ID[1] = 0x34; 
	sensor[0].ID[2] = 0x5A; 
	sensor[0].Pressure = 123;
	sensor[0].Temperature = 46;  
	
	sensor[1].ID[0] = 0xC3;
	sensor[1].ID[1] = 0x78;
	sensor[1].ID[2] = 0x6D;
	sensor[1].Pressure = 99;
	sensor[1].Temperature = 106;
	
	sensor[2].ID[0] = 0x8B;
	sensor[2].ID[1] = 0x78;
	sensor[2].ID[2] = 0x33;
	sensor[2].Pressure = 111;
	sensor[2].Temperature = 87;	
	
	sensor[3].ID[0] = 0x22;
	sensor[3].ID[1] = 0x33;
	sensor[3].ID[2] = 0x44;
	sensor[3].Pressure = 555;
	sensor[3].Temperature = 666;	
	
-------------------------TESTING */
	
	//---------------------------
	// load the first 3 pressure sensor data on
	// screen.
	// 1. get the data
	// 2. convert it from KPA to PSI multiply by 145038 and divide by 1000000
	// 3. see how many digits.
	//---------------------------------
 
	for (m=0; m<4;m++)
	{
		if ((sensorDynamic[k].ID[0] != 0)||(sensorDynamic[k].ID[1] != 0) ||(sensorDynamic[k].ID[2]!=0))
		{
			offset = 23;
			temp = sensorDynamic[k].ID[0]; 
			temp2 = temp & 0x0f; 
			temp = temp>>4; 
			LCDPlaceData(offset,(8*m)+16,(uint8_t *)FONTSMALL[temp],5);  
			LCDPlaceData(offset+6,(8*m)+16,(uint8_t *)FONTSMALL[temp2],5);  		
			temp = sensorDynamic[k].ID[1];
			temp2 = temp & 0x0f;
			temp = temp>>4;
			LCDPlaceData(offset+12,(8*m)+16,(uint8_t *)FONTSMALL[temp],5);
			LCDPlaceData(offset+18,(8*m)+16,(uint8_t *)FONTSMALL[temp2],5);		
			temp = sensorDynamic[k].ID[2];
			temp2 = temp & 0x0f;
			temp = temp>>4;
			LCDPlaceData(offset+24,(8*m)+16,(uint8_t *)FONTSMALL[temp],5);
			LCDPlaceData(offset+30,(8*m)+16,(uint8_t *)FONTSMALL[temp2],5);
			//------------------
			// psi
			numDigits = AppGetPressureConverted(k,digits,0);	
			for (i=0;i<3;i++)
			{
				if (digits[i] != ' ')
				{
					temp = digits[i];
					LCDPlaceData((offset+42)+(i*6),(8*m)+16,(uint8_t *)FONTSMALL[temp],5);					
				}
			}	
			//------------------
			// temperature
			numDigits = AppGetTemperatureConverted(k,digits,0);
			for (i=0;i<3;i++)
			{
				if (digits[i] != ' ')
				{
					temp = digits[i];
					LCDPlaceData((offset+64)+(i*6),(8*m)+16,(uint8_t *)FONTSMALL[temp],5);
				}
			}	
			AppScreenCarRadioReadingIn(k,FALSE);
			 		
		}
		k++;
	}
}
#endif 


