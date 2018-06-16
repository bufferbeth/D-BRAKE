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
#include "driverSpeaker.h"

#define SPEAKER_MAXSTEP 1  //7
 
uint8_t testStep; 
struct tc_module speakertc_instance;

//---------------------LOCAL FUNCTION PROTOTYPES--------------------------
void SpeakerTurnOn(uint8_t step);
void SpeakerConfigureCallbacks(void);
void SpeakerCallback(struct tc_module *const module_inst);
void SpeakerTurnOff(void);

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX 
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void SpeakerOff(void)
{
	tc_reset(&speakertc_instance);
} 
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void SpeakerOn(void)
{
	tc_reset(&speakertc_instance);
	SpeakerTurnOn(0);
	SpeakerConfigureCallbacks();
} 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
void SpeakerCallback(struct tc_module *const module_inst)
{
	static uint16_t i = 0;
//	i += 512;
	i += 256;
//	i += 128;
	tc_set_compare_value(module_inst, TC_COMPARE_CAPTURE_CHANNEL_0, i + 1);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void SpeakerTurnOff(void)
{
	tc_disable(&speakertc_instance);		
	tc_reset(&speakertc_instance);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
// 8 steps 
void SpeakerTurnOn(uint8_t step)
{
	 
	struct tc_config config_tc;
	 
	tc_get_config_defaults(&config_tc);
	 
	config_tc.counter_size    = TC_COUNTER_SIZE_16BIT;
	config_tc.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM;
	config_tc.counter_16_bit.compare_capture_channel[0] = 0xFFFF;
	
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;	
	switch (step)
	{
		case 0:
		{
			config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;	
			break;
		}
		case 1:
		{
			config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV2;	
			break;
		}	
		case 2:
		{
			config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV4;	
			break;
		}
		case 3:
		{
			config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV8;	
			break;
		}				
		case 4:
		{
			config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV16;	
			break;
		}
		case 5:
		{
			config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV64;	
			break;
		}	
		case 6:
		{
			config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV256;	
			break;
		}
		case 7:
		{
			config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;	
			break;
		}				
		default:
		{
			config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;	
			break;
		}						
	}

	config_tc.waveform_invert_output = TC_WAVEFORM_INVERT_OUTPUT_CHANNEL_1;
	 
	config_tc.pwm_channel[0].enabled = true;
	config_tc.pwm_channel[0].pin_out = PIN_PB16E_TC6_WO0;
	config_tc.pwm_channel[0].pin_mux = MUX_PB16E_TC6_WO0;

	config_tc.pwm_channel[1].enabled = true;
	config_tc.pwm_channel[1].pin_out = PIN_PB17E_TC6_WO1;
	config_tc.pwm_channel[1].pin_mux = MUX_PB17E_TC6_WO1;
		 
	tc_init(&speakertc_instance, TC6, &config_tc);
	 
	tc_enable(&speakertc_instance);
	 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void SpeakerConfigureCallbacks(void)
{
	//! [setup_register_callback]
	tc_register_callback(&speakertc_instance,SpeakerCallback,TC_CALLBACK_CC_CHANNEL0); 
	tc_enable_callback(&speakertc_instance, TC_CALLBACK_CC_CHANNEL0);
	 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
void SpeakerNextStep(void) 
{
	tc_reset(&speakertc_instance);
	testStep++;
	if (testStep > SPEAKER_MAXSTEP)
	{
		testStep = 0; 
	}
	if (testStep == 0)
	{
		SpeakerTurnOn(testStep);	
 		SpeakerConfigureCallbacks();
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
void Speakermain(void)
{
	testStep = 0; 
	SpeakerTurnOn(0);
	SpeakerConfigureCallbacks();
 
}


