//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: driverSpeaker.c
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
#if REMOTEBOARD	
#include "DriverScreenColor.h"

 

//---------------------LOCAL FUNCTION PROTOTYPES--------------------------
uint8_t currentScreenColor;  
uint8_t backlight = 0; 


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX 

void BacklightSetHomeColor(uint8_t color)
{
	if (backlight == 0)
	{
		BacklightSetColor(0);
	}
	else
	{
		BacklightSetColor(color);		
	}	  
}

 


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: BacklightToggleLight
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void BacklightToggleLight(uint8_t color)
{
	//-------------------------------
	// if the currentScreenColor is the same as color - this will 
	// the switch to NONE 
	if (backlight == 1)
	{
		backlight = 0;
		BacklightSetColor(0);	
	}
	else
	{
		backlight = 1; 
		BacklightSetColor(7);  //color);		
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void BacklightSetColor(uint8_t color)
{
	currentScreenColor = color; 
				switch (color)
				{
					case 0:
					{
						port_pin_set_output_level(LED_RED_PIN, LED_RED_INACTIVE);
						port_pin_set_output_level(LED_BLUE_PIN, LED_BLUE_INACTIVE);
						port_pin_set_output_level(LED_GREEN_PIN, LED_GREEN_INACTIVE);						
						break;
					}
					case 1:
					{
						port_pin_set_output_level(LED_RED_PIN, LED_RED_INACTIVE);
						port_pin_set_output_level(LED_BLUE_PIN, LED_BLUE_INACTIVE);
						port_pin_set_output_level(LED_GREEN_PIN, LED_GREEN_ACTIVE);
						break;
					}					
					case 2:
					{
						port_pin_set_output_level(LED_RED_PIN, LED_RED_INACTIVE);
						port_pin_set_output_level(LED_BLUE_PIN, LED_BLUE_ACTIVE);
						port_pin_set_output_level(LED_GREEN_PIN, LED_GREEN_INACTIVE);
						break;
					}					
					case 3:
					{
						port_pin_set_output_level(LED_RED_PIN, LED_RED_INACTIVE);
						port_pin_set_output_level(LED_BLUE_PIN, LED_BLUE_ACTIVE);
						port_pin_set_output_level(LED_GREEN_PIN, LED_GREEN_ACTIVE);
						break;
					}					
					case 4:
					{
						port_pin_set_output_level(LED_RED_PIN, LED_RED_ACTIVE);
						port_pin_set_output_level(LED_BLUE_PIN, LED_BLUE_INACTIVE);
						port_pin_set_output_level(LED_GREEN_PIN, LED_GREEN_INACTIVE);
						break;
					}
					case 5:
					{
						port_pin_set_output_level(LED_RED_PIN, LED_RED_ACTIVE);
						port_pin_set_output_level(LED_BLUE_PIN, LED_BLUE_INACTIVE);
						port_pin_set_output_level(LED_GREEN_PIN, LED_GREEN_ACTIVE);
						break;
					}					
					case 6:
					{
						port_pin_set_output_level(LED_RED_PIN, LED_RED_ACTIVE);
						port_pin_set_output_level(LED_BLUE_PIN, LED_BLUE_ACTIVE);
						port_pin_set_output_level(LED_GREEN_PIN, LED_GREEN_INACTIVE);
						break;
					}					
					case 7:
					{
						port_pin_set_output_level(LED_RED_PIN, LED_RED_ACTIVE);
						port_pin_set_output_level(LED_BLUE_PIN, LED_BLUE_ACTIVE);
						port_pin_set_output_level(LED_GREEN_PIN, LED_GREEN_ACTIVE);
						break;
					}											
				}
}

#endif
