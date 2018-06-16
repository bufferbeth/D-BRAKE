/**
 * \file
 *
 * \brief SAM ADC Quick Start
 *
 * Copyright (C) 2013-2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "dBrakeDefs.h"
#include <asf.h>
#include "driverADC.h"
#include "appMotor.h"
 
void adc_complete_callback(
		const struct adc_module *const module);

uint8_t adcFirstPassDone = 0; 
	//-------------adcOffset 
#if BRAKEBOARD
#define MAX_ADC_CHANNELS  2
#endif
#if REMOTEBOARD
#define MAX_ADC_CHANNELS  1
#endif
uint8_t adcOffset; 
 
uint16_t adcTimer; 
uint8_t adcTimeout;
//#define ADCTIME 3
 
	//-----------adc_result_buffer------------
	// temporary adc buffer
#define ADC_SAMPLES 4
uint16_t adc_result_buffer[ADC_SAMPLES];
 
	//-----------adcReadings 
uint16_t adcReadings[MAX_ADC_CHANNELS][ADC_SAMPLES];
uint16_t adcAverageReadings[MAX_ADC_CHANNELS];
uint16_t maxCurrentRead = 0;

struct adc_module adc_instance;
 
volatile bool adc_read_done = false;



//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
 uint16_t ADCGetReading(uint8_t which)
 {
	 uint16_t valueRead; 
	 valueRead = 0; 
	 switch(which)
	 {
		 case ADC_INPUT_VOLTAGE:
		 {
			 valueRead = adcAverageReadings[0]; 
			 break;
		 }
		 case ADC_INPUT_CURRENT:
		 {
			 valueRead = adcAverageReadings[1]; 
			 break;
		 }
	 }
	 return valueRead; 
 }
 
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void adc_complete_callback(const struct adc_module *const module)
{
	uint8_t i;
	uint16_t averageRead;	
	adc_read_done = true;

#if BRAKEBOARD	
	if (motorOn == TRUE)
	{
/*		
		//-------------------
		// load in the readings 
		for (i=0;i<(ADC_SAMPLES-1);i++)
		{
			if (adcOffset < MAX_ADC_CHANNELS)
			{
				adcReadings[adcOffset][i] = adc_result_buffer[i+1];
			}
		}
		averageRead = 0;
		for (i=1;i<(ADC_SAMPLES-1);i++)
		{
			averageRead += adc_result_buffer[i+1];
		}
		averageRead = averageRead/(ADC_SAMPLES-1-1);
		adcAverageReadings[adcOffset] = averageRead;	
*/		
		adcAverageReadings[adcOffset] = adc_result_buffer[1];
		adcOffset=1;
		adc_set_positive_input(&adc_instance,ADC_POSITIVE_INPUT_PIN1);	
		adc_read_buffer_job(&adc_instance, adc_result_buffer, ADC_SAMPLES);		
		if (motorRunTime > 0)
		{
			maxCurrentRead = 0;
		}	
		else
		{
			if (maxCurrentRead < adcAverageReadings[1])
			{
				maxCurrentRead = adcAverageReadings[1]; 
			}
		}
	}
	else
	{
		schedByte |= SCHEDBYTE_ADC;
	}
#else
	schedByte |= SCHEDBYTE_ADC;	
#endif
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================  
void configure_adc(uint8_t which)
{
	struct adc_config config_adc;
	
 	adc_get_config_defaults(&config_adc);
 
	config_adc.gain_factor     = ADC_GAIN_FACTOR_1X;  //ADC_GAIN_FACTOR_DIV2;
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV8;  //8;
	config_adc.reference       = ADC_REFERENCE_INT1V; //ADC_REFERENCE_INTVCC0;  //ADC_REFERENCE_INTVCC1;
#if REMOTEBOARD
	config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN0;
#else
	config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN1;
#endif	
	config_adc.resolution      = ADC_RESOLUTION_12BIT;
//    config_adc.correction.correction_enable = true;
//    config_adc.correction.offset_correction = 512;
//	config_adc.correction.gain_correction = 0x0800;
	config_adc.negative_input     =ADC_NEGATIVE_INPUT_GND;
	config_adc.sample_length                 = 1;
	config_adc.resolution         = ADC_RESOLUTION_CUSTOM;
	config_adc.divide_result = ADC_DIVIDE_RESULT_16;
	config_adc.accumulate_samples = ADC_ACCUMULATE_SAMPLES_16;

#if BRAKEBOARD	
	switch (which)
	{
		case 0:
		{
			config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN0;			
			break;
		}	
		case 1:
		{
			config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN1;			
			break;
		}
		case 2:
		{
			config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN12;			
			break;
		}	
		case 3:
		{
			config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN14;			
			break;
		}
		case 4:
		{
			config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN15;			
			break;
		}				
	}
#endif 

	adc_init(&adc_instance, ADC, &config_adc);
	adc_enable(&adc_instance);
 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void configure_adc_callbacks(void)
{
 
	adc_register_callback(&adc_instance,
			adc_complete_callback, ADC_CALLBACK_READ_BUFFER);
 
	adc_enable_callback(&adc_instance, ADC_CALLBACK_READ_BUFFER);
 
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
void ADCTask(void)
{
	uint8_t i; 
	uint16_t averageRead; 
	//----------------------
	// load in the readings for the current channel 
	// start the next reading of the next channel
	//-----------------------
	if (adc_read_done != FALSE)
	{
		adc_read_done = 0; 
		//-------------------
		// load in the readings 
		for (i=0;i<(ADC_SAMPLES-1);i++)
		{
			if (adcOffset < MAX_ADC_CHANNELS)
			{
				adcReadings[adcOffset][i] = adc_result_buffer[i+1];
			}
		}
		averageRead = 0;
		for (i=1;i<(ADC_SAMPLES-1);i++)
		{
			averageRead += adc_result_buffer[i+1];
		}
		averageRead = averageRead/(ADC_SAMPLES-1-1);
		adcAverageReadings[adcOffset] = averageRead;		
		//----------------------
		// testing grab maximum current sense 
		//----------------------
		if (adcOffset == 1)
		{
			
			//----------------------
			// Get an average. 
			if (averageRead>maxCurrentRead)
			{
				maxCurrentRead = averageRead;
			}
		}		
		adcOffset++;
		if (adcOffset >= MAX_ADC_CHANNELS)
		{
			adcOffset = 0;
			adcFirstPassDone = 1;  
#if BRAKEBOARD			
			brakeChange |= BRAKECHANGE_ADCDONE; 
			schedByte|= SCHEDBYTE_BRAKETASK; 
#endif			
			//-----------------------
			// set a timer to do the ADC stuff
			
		}
#if BRAKEBOARD		
		if (motorOn == TRUE)
		{
			adcOffset = 1; 
		}
		adcTimeout = 1; //04-11-16
#endif
 		adcTimer = ADCTIME; 
		adcTimeout = 0; 
	}
	if (adcTimeout != 0)
	{
		adcTimeout = 0;
		adcTimer = 0; 
		switch (adcOffset)
		{
			case 0:
			{
				adc_set_positive_input(&adc_instance,ADC_POSITIVE_INPUT_PIN0);
	//			config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN0;			
				break;
			}	
			case 1:
			{
				adc_set_positive_input(&adc_instance,ADC_POSITIVE_INPUT_PIN1);			
	//			config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN1;			
				break;
			}
			case 2:
			{
				adc_set_positive_input(&adc_instance,ADC_POSITIVE_INPUT_PIN12);
	//			config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN2;			
				break;
			}	
			case 3:
			{
				adc_set_positive_input(&adc_instance,ADC_POSITIVE_INPUT_PIN14);			
	//			config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN14;			
				break;
			}
			case 4:
			{
				adc_set_positive_input(&adc_instance,ADC_POSITIVE_INPUT_PIN15);			
	//			config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN15;			
				break;
			}				
		}
		adc_read_buffer_job(&adc_instance, adc_result_buffer, ADC_SAMPLES);		
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void ADCInit(void)
{
	uint8_t i,j; 
	
	adcOffset = 0; 
	adcFirstPassDone = 0; 
	 
	configure_adc(adcOffset);	
	configure_adc_callbacks();	
	
	for (i=0;i<MAX_ADC_CHANNELS;i++)
	{
		for (j=0;j<ADC_SAMPLES;j++)
		{
			adcReadings[i][j]=0;
		}
		adcAverageReadings[i]=0;
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void ADCStart(void)
{
	uint8_t i; 
	
	for (i=0;i<MAX_ADC_CHANNELS;i++)
	{
		adc_read_buffer_job(&adc_instance, adc_result_buffer, ADC_SAMPLES);
		while ((schedByte&SCHEDBYTE_ADC)==0);
		schedByte &= ~SCHEDBYTE_ADC; 
		ADCTask();
		adcTimeout = 1; 
		ADCTask();
	}
}

