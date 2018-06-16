//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: buttons.c
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
#include "driverbuttons.h"
#include "appMotor.h"
void deconfigure_wdt(void); 

//---------------------GLOBAL VARIABLES-----------------------------------
short int constantTX_pressed = 0;
short int constantRX_pressed = 0;
short int constantCW_pressed = 0;
 
#if REMOTEBOARD
short int right_pressed= 0; 
short int center_pressed = 0; 
short int left_pressed = 0; 
short int brake_pressed = 0; 
	//--------------------- 
	// breakaway_pressed - happens when right and left are pressed. 
short int breakaway_pressed = 0;
#endif
#if BRAKEBOARD
short int setup_pressed;
short int power_pressed;
short int breakawayRing_pressed; 
short int breakawayTip_pressed; 

uint8_t flimitState;
uint8_t hlimitState;
#endif 

//---------------------LOCAL VARIABLES------------------------------------
uint16_t constantTX_hist = 0x0000;
uint16_t constantRX_hist = 0x0000;
uint16_t constantCW_hist = 0x0000;
  //--------------------------
  //button check histories
#if REMOTEBOARD  
uint16_t right_hist = 0x0000;
uint16_t center_hist = 0x0000;
uint16_t left_hist = 0x0000; 
uint16_t brake_hist = 0x0000;
#endif
#if BRAKEBOARD
uint16_t setup_hist = 0x0000;
uint16_t power_hist= 0x0000;
uint16_t breakawayRing_hist = 0x0000;
uint16_t breakawayTip_hist = 0x0000;
#endif

uint8_t keyChanged; 


 
//---------------------LOCAL FUNCTION PROTOTYPES--------------------------  
 

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   ButtonSample
//------------------------------------------------------------------------------
// This function Delays then checks the buttons and updates button booleans
//==============================================================================
void FCCSample(void)
{
	 
	//------------------------
	//check pins
	constantTX_hist = (constantTX_hist << 1) + port_pin_get_input_level(CONSTANTTX);
	constantRX_hist = (constantRX_hist << 1) + port_pin_get_input_level(CONSTANTRX);
	constantCW_hist = (constantCW_hist << 1) + port_pin_get_input_level(CONSTANTCW);
	 	
	//update vars
 
	if ((constantTX_hist & 7) == 0)
	constantTX_pressed = 1;
	else
	constantTX_pressed = 0;

	 
	if ((constantRX_hist & 7) == 0)
	constantRX_pressed = 1;
	else
	constantRX_pressed = 0;
	
	 
	if ((constantCW_hist & 7) == 0)
	constantCW_pressed = 1;
	else
	constantCW_pressed = 0;	
}
 

uint8_t ButtonChanged(void)
{
	uint8_t which; 
	which = keyChanged; 
	keyChanged = 0;	
	return which;
	
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   ButtonInit
//------------------------------------------------------------------------------
// This function Initializes registers to allow button interrupts
//==============================================================================
void ButtonInit(void)
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);	
  
	/* Set buttons as inputs */
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
#if REMOTEBOARD	
	port_pin_set_config(BUTTON_SW1, &pin_conf);
	port_pin_set_config(BUTTON_SW2, &pin_conf);
	port_pin_set_config(BUTTON_SW3, &pin_conf);
	port_pin_set_config(BUTTON_SW4, &pin_conf);
#endif 
#if BRAKEBOARD
	port_pin_set_config(BUTTON_SETUP, &pin_conf);
	port_pin_set_config(BUTTON_POWER, &pin_conf);
	port_pin_set_config(INPUT_BREAKAWAY_RING, &pin_conf);	
	port_pin_set_config(INPUT_BREAKAWAY_TIP, &pin_conf);		
#endif	
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   ButtonSample
//------------------------------------------------------------------------------
// This function Delays then checks the buttons and updates button booleans
//==============================================================================
void ButtonSample(void)
{
	short int prevKey; 
	
#if REMOTEBOARD	
  //------------------------
  //check pins
  right_hist = (right_hist << 1) + port_pin_get_input_level(BUTTON_SW1);
  center_hist = (center_hist << 1) + port_pin_get_input_level(BUTTON_SW2);
  left_hist = (left_hist << 1) + port_pin_get_input_level(BUTTON_SW3);
  brake_hist = (brake_hist << 1) + port_pin_get_input_level(BUTTON_SW4);
  
  //update vars
  prevKey = right_pressed; 
  if ((right_hist & 7) == 0)  
    right_pressed = 1;
  else
    right_pressed = 0;
  if (prevKey != right_pressed)
  {
	keyChanged |= KEY_RIGHT;
	schedByte |= SCHEDBYTE_APPSCREENKEYCHANGE; 
  
  }

  prevKey = center_pressed;   
  if ((center_hist & 7) == 0)  
    center_pressed = 1;
  else
    center_pressed = 0;
  if (prevKey != center_pressed)
  {
	  keyChanged |= KEY_CENTER;
	  schedByte |= SCHEDBYTE_APPSCREENKEYCHANGE;
	  
  }

  prevKey = left_pressed;       
  if ((left_hist & 7) == 0)  
    left_pressed = 1;
  else
    left_pressed = 0;  
  if (prevKey != left_pressed)
  {
	  keyChanged |= KEY_LEFT;
	  schedByte |= SCHEDBYTE_APPSCREENKEYCHANGE;
	  
  }	
	
  prevKey = brake_pressed;  
  if ((brake_hist & 7) == 0)  
    brake_pressed = 1;
  else
    brake_pressed = 0;  
  if (prevKey != brake_pressed)
  {
	  keyChanged |= KEY_BRAKE;
	  schedByte |= SCHEDBYTE_APPSCREENKEYCHANGE;
	  
  }	
  
  if ((right_pressed != 0)&&(left_pressed != 0))
  {
	  breakaway_pressed = 1; 
  }
  else
  {
	  breakaway_pressed = 0; 
  }
  
#endif	
#if BRAKEBOARD
	//------------------------
	//check pins
	setup_hist = (setup_hist << 1) + port_pin_get_input_level(BUTTON_SETUP);
	power_hist = (power_hist << 1) + port_pin_get_input_level(BUTTON_POWER);
	breakawayRing_hist = (breakawayRing_hist << 1) + port_pin_get_input_level(INPUT_BREAKAWAY_RING);	
	breakawayTip_hist = (breakawayTip_hist << 1) + port_pin_get_input_level(INPUT_BREAKAWAY_TIP);		
	 
	//update vars
	prevKey = setup_pressed;
	if ((setup_hist & 7) == 0)
	setup_pressed = 1;
	else
	setup_pressed = 0;
	if (prevKey != setup_pressed)
	{
		keyChanged |= KEY_SETUP;
//		schedByte |= SCHEDBYTE_APPSCREENKEYCHANGE;
	
	}

	prevKey = power_pressed;
	if ((power_hist & 7) == 0)
	power_pressed = 1;
	else
	power_pressed = 0;
	if (prevKey != power_pressed)
	{
		keyChanged |= KEY_POWER;
//		schedByte |= SCHEDBYTE_APPSCREENKEYCHANGE;
	}
	
	prevKey = breakawayRing_pressed;
	if ((breakawayRing_hist & 7) == 0)
	breakawayRing_pressed = 1;
	else
	breakawayRing_pressed = 0;	
	if (prevKey != breakawayRing_pressed)
	{
		keyChanged |= KEY_BREAKAWAYRING;
//		schedByte |= SCHEDBYTE_APPSCREENKEYCHANGE;
	}	
	
	prevKey = breakawayTip_pressed;
	if ((breakawayTip_hist & 7) == 0)
	breakawayTip_pressed = 1;
	else
	breakawayTip_pressed = 0;	
	if (prevKey != breakawayTip_pressed)
	{
		keyChanged |= KEY_BREAKAWAYTIP;
//		schedByte |= SCHEDBYTE_APPSCREENKEYCHANGE;
	}	
		
	flimitState = port_pin_get_input_level(FLIMIT);	
	hlimitState = port_pin_get_input_level(HLIMIT);		
	
/*
				if (((keyChanged & KEY_POWER)!= 0)&&(power_pressed == 0))				
 				{
						if (poweredUp != 0)
						{
							poweredUp = 0;
//V01_11							brakeState = BRAKESTATE_POWERINGUP;
//V01_11							BrakeBoardStateMachineTask();	
							deconfigure_wdt();
							NVIC_SystemReset();						
						}
						else
						{
							
						//	poweredUp = 1;
						//	brakeState = BRAKESTATE_RESET;
						//	BrakeBoardStateMachineTask();									
//V01_11							deconfigure_wdt();
//V01_11							NVIC_SystemReset();					
						}		
				}	
*/				
				if (((keyChanged & KEY_POWER)!= 0)&&(power_pressed != 0))
				{
					if (poweredUp == 0)
					{
						poweredUp = 1;
						brakeState = BRAKESTATE_RESET;
						BrakeBoardStateMachineTask();	
					}
					else
					{
							poweredUp = 0;
							deconfigure_wdt();
							NVIC_SystemReset();								
					}
				}	
	
#endif		
}

