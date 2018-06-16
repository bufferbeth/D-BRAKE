//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: appEncoder.c
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
#include "appMotor.h"
#include "driverButtons.h"
#include "driverADC.h"
#include "appAccel.h"
#include "appProtocol.h"
#include "config.h"
#include "appEncoder.h" 
 
#if BRAKEBOARD

uint16_t encoderFillOffset = 0; 
uint16_t encoderCount = 0;  
uint8_t encoderFlip = 0; 
//---------------------GLOBAL VARIABLES-----------------------------------
 
BUILDTABLE buildTable;
BUILDTABLE encoderTable;
uint16_t buildTableOffset;
uint16_t encoderTableOffset; 
 
//---------------------LOCAL VARIABLES------------------------------------

	//-----------matchCurrent
	// this is the current value to look for in the encoder table to 
	// retract. 
 

	//----------------setupExtendTriggered
	// used to see if the extend is triggered during setup
	// should not happen. 
  
//---------------------LOCAL FUNCTION PROTOTYPES--------------------------  
 
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
uint16_t MotorFindEncoderMatch(uint16_t matchCurrent)
{
	uint16_t countBack,i,newOffset; 
	uint8_t done; 
	
	countBack = 0; 
	//----------------------------
	// go back from encoderTableOffset and look for first current 
	// less than matchCurrent. 
	// record the encoder count match. 
	//------------------------------
	done = 0;
	newOffset = encoderTableOffset; 
	if (newOffset >0)
	{
		newOffset--;
	}
	else
	{
		newOffset = MAX_BUILDTABLE; 
	}
	i = 0; 
	if (encoderFillOffset > MAX_BUILDTABLE)
	{
		encoderFillOffset = MAX_BUILDTABLE;
	}
	while ((done==0) && (i<encoderFillOffset))
	{
		if (encoderTable.Current[newOffset] < matchCurrent)
		{
			done = 1; 
		}
		else
		{
			i++;
			countBack++;
			if (newOffset >0)
			{
				newOffset--;
			}
			else
			{
				newOffset = MAX_BUILDTABLE; 
			}			
		}
	}
	return countBack; 
}
 
 //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 // FUNCTION:
 //------------------------------------------------------------------------------
 // This function
 //==============================================================================
 void ConfigureEncoder(void)
 {
	 struct extint_chan_conf config_extint_chan;
	 extint_chan_get_config_defaults(&config_extint_chan);
	 config_extint_chan.gpio_pin = PIN_PB11A_EIC_EXTINT11;
	 config_extint_chan.gpio_pin_mux = MUX_PB11A_EIC_EXTINT11;
	 config_extint_chan.gpio_pin_pull = EXTINT_PULL_NONE;
	 config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
	 extint_chan_set_config(11, &config_extint_chan);
 }
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void ConfigureEncoderCallbacks(void)
{
	extint_register_callback(EncoderCallback,11,EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(11,EXTINT_CALLBACK_TYPE_DETECT);
} 

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void EncoderCallback(void)
{
	encoderCount++;
	if (action == EXTENDING)
	{
		if (encoderTableOffset >= MAX_BUILDTABLE)
		{
			encoderTableOffset = 0;
			encoderFlip = 1; 
		}
		encoderTable.Current[encoderTableOffset] = ADCGetReading(ADC_INPUT_CURRENT);
		encoderTable.EncoderCount[encoderTableOffset++] = encoderCount;
	}
	if (action == RETRACTING_BY_ENCODER)
	{
		if (encoderCountBack >0)
		{
			encoderCountBack--;
			if (encoderCountBack == 0)
			{
				MotorOff();
			}
		}
	}	
}


#endif 
