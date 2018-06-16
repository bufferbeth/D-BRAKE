//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: appMotor.c
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
#include "appBluetooth.h"
 
#define TESTWITHOUTSETUP 1
 
uint8_t brakeState;
uint8_t brakeChange;

#define BRAKESUPTIME 10
#define BRAKESUPTIME_LONG 30 
#define BRAKESUPTIME_TIMEOUT 15
#define BRAKESUPTIME_SETUPPAUSE 2
#define BRAKESUPTIMESHORT 1
 
#if BRAKEBOARD

#define MOTOR_RUN_TIME 300 //beth 200
//---------------------GLOBAL VARIABLES-----------------------------------
uint16_t voltageBadTime; 
uint16_t ditherTimer; 
//---------------------LOCAL VARIABLES------------------------------------

	//-----------matchCurrent
	// this is the current value to look for in the encoder table to 
	// retract. 
uint16_t matchCurrent; 

uint16_t instantGain;
uint16_t holdG;
uint16_t newG; 
uint16_t diffG; 
uint16_t gPrime; 

uint16_t encoderCountBack;  
uint16_t encoderCountBackTotal; 
 
uint8_t action = NONE; 
uint8_t motorOn = FALSE; 

//uint8_t brakeSetupLED;
uint8_t brakeSetupExtend;
uint16_t motorx;
uint16_t motory; 
uint16_t motorz; 
uint8_t motorAccBaseline; 
int16_t motorAccXBaseline; 
int16_t motorAccYBaseline; 
int16_t motorAccZBaseline; 

	//----------------setupExtendTriggered
	// used to see if the extend is triggered during setup
	// should not happen. 
uint8_t	setupExtendTriggered = 0;



int16_t	tempx;
int16_t tempbasex;
int16_t tempdiffx;
	#define MAX_THRESHOLD_NEEDED 3
uint8_t thresholdmet = 0;   

uint8_t hlimitCount;
uint8_t flimitCount;  

uint8_t brakeBiLED;
uint8_t brakeBlueLED;
uint8_t brakeRedLED; 



uint16_t motorRunTime;
uint16_t prevMotorRunTime; 

 
//---------------------LOCAL FUNCTION PROTOTYPES--------------------------  
void ConfigureMotorHLimitChannel(void);
void ConfigureMotorHLimitCallbacks(void);
void MotorHLimitCallback(void);
void ConfigureMotorFLimitChannel(void);
void ConfigureMotorFLimitCallbacks(void);
void MotorFLimitCallback(void);
uint8_t BrakeActuatorControl(uint8_t which);
	#define BRAKE_HOME 0
	#define BRAKE_AWAY 1
	#define BRAKE_RUN  2
	#define BRAKE_MIDDLESTOP 3
	
	#define BRAKE_ERROR 0
	#define BRAKE_GOOD 1
void BrakeLEDControl(void);	
void BrakeSupervisorytask(void);
uint16_t CurrentMotorCalculated(void);
int16_t MotorGetAcc(uint8_t activeBraking);
uint16_t CurrentMotorCalculatedBreakaway(void);
uint16_t CurrentMotorCalculatedManual(void);
void BrakeEnterIdleSleepMode(void);
uint16_t CurrentMotorCalculatedForG(uint16_t gin);
void RetractByEncoderCount(void);
void MotorExtendMore(void);
	
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void BrakeInit(void)
{
	brakeBiLED = BRAKEBILED_OFF;
	brakeBlueLED = BRAKEBLUELED_OFF;
	brakeRedLED = BRAKEREDLED_OFF; 
	brakeStatus.BrakeState |= BRAKESTATE_NOTSETUP;	
	brakeState = BRAKESTATE_POWERINGUP; 
	table0.Item.MaxForce = 5; 
	BrakeBoardStateMachineTask();	
	gPrime = 0; 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void BrakeEnterIdleSleepMode(void)
{
	brakeBiLED = BRAKEBILED_OFF;
	brakeBlueLED = BRAKEBLUELED_OFF;
	brakeRedLED = BRAKEREDLED_OFF;
	brakeState = BRAKESTATE_IDLESLEEP;
 
	
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
uint8_t BrakeActuatorControl(uint8_t which)
{
	uint8_t status; 
	status = BRAKE_GOOD; 
	switch (which)
	{
		case BRAKE_HOME:
		{
			if (hlimitState != 0)
			{	 
				if (encoderCount == 0)
				{
					brakeStatus.ActuatorStatus |= ACTUATORSTATUS_ENCODERFAIL;
					status = BRAKE_ERROR;
				}
				else
				{
					brakeStatus.ActuatorStatus |= ACTUATORSTATUS_HOMEOFFFAIL;
					brakeStatus.ActuatorStatus &= ~ACTUATORSTATUS_ENCODERFAIL;
					status = BRAKE_ERROR; 
				}
			}
			else
			{
				brakeStatus.ActuatorStatus &= ~ACTUATORSTATUS_HOMEOFFFAIL;
			}
			break;
		}
		case BRAKE_AWAY:
		{
			if (flimitState != 0)
			{
				if (encoderCount == 0)
				{
					brakeStatus.ActuatorStatus |= ACTUATORSTATUS_ENCODERFAIL;
					status = BRAKE_ERROR;
				}
				else
				{
					brakeStatus.ActuatorStatus |= ACTUATORSTATUS_EXTENDOFFFAIL;
					brakeStatus.ActuatorStatus &= ~ACTUATORSTATUS_ENCODERFAIL;
					status = BRAKE_ERROR;
				}
			}
			else
			{
				brakeStatus.ActuatorStatus &= ~ACTUATORSTATUS_EXTENDOFFFAIL;
			}
			break;
		}		
		case BRAKE_RUN:
		{
			if (encoderCount == 0)
			{
				brakeStatus.ActuatorStatus |= ACTUATORSTATUS_ENCODERFAIL;
				status = BRAKE_ERROR;
			}
			else
			{
				brakeStatus.ActuatorStatus &= ~ACTUATORSTATUS_ENCODERFAIL;
			}
			break;
		}		
		case BRAKE_MIDDLESTOP:
		{
			if (flimitState == 0)
			{
				brakeStatus.ActuatorStatus |= ACTUATORSTATUS_EXTENDONFAIL;
				status = BRAKE_ERROR;
			}
			else
			{
				brakeStatus.ActuatorStatus &= ~ACTUATORSTATUS_EXTENDONFAIL;
			}
			if (hlimitState == 0)
			{
				brakeStatus.ActuatorStatus |= ACTUATORSTATUS_HOMEONFAIL;
				status = BRAKE_ERROR;
			}
			else
			{
				brakeStatus.ActuatorStatus &= ~ACTUATORSTATUS_HOMEONFAIL;
			}			
			break;
		}		
	}
	if ((flimitState == 0)&&(hlimitState ==0))
	{
		brakeStatus.ActuatorStatus |= ACTUATORSTATUS_BOTHLIMITSACTIVE;	
		status = BRAKE_ERROR;
	}
	else
	{
		brakeStatus.ActuatorStatus &= ~ACTUATORSTATUS_BOTHLIMITSACTIVE;
	}
	return status; 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void BrakeLEDControl(void)
{
	//--------------------------------
	// what should the bi color LED be doing?
	// 1. flash red for the following errors: 
	//    a. brakeStatus.BrakeState |= BRAKESTATE_INPUTVOLTAGEBAD;	
	switch (brakeState)
	{
		case BRAKESTATE_POWERINGUP:
		case BRAKESTATE_POWEREDUP:
		case BRAKESTATE_IDLESLEEP:
		{
			//leds are off 
			brakeBiLED = BRAKEBILED_OFF;
			brakeBlueLED = BRAKEBLUELED_OFF; 
			brakeRedLED = BRAKEREDLED_OFF; 
			break; 
		}
		case BRAKESTATE_RESET:
		case BRAKESTATE_PRESETUP:
		{
			brakeBiLED = BRAKEBILED_OFF;
			brakeBlueLED = BRAKEBLUELED_ALTGREEN;
			brakeRedLED = BRAKEREDLED_OFF; 
			break;
		}		
		case BRAKESTATE_ERROR:
		{
			brakeBiLED = BRAKEBILED_REDFLASH;
			brakeBlueLED = BRAKEBLUELED_OFF;
			brakeRedLED = BRAKEREDLED_OFF; 			
			break;
		}		
		case BRAKESTATE_SETUP:
		case BRAKESTATE_SETUPACTIVE:
		{
			brakeBiLED = BRAKEBILED_OFF;
			brakeBlueLED = BRAKEBLUELED_ALTGREEN;
			brakeRedLED = BRAKEREDLED_OFF; 			
			break;
		}		
		case BRAKESTATE_ACTIVE:
		case BRAKESTATE_HOLDOFF_ACTIVE:
		{
			brakeBiLED = BRAKEBILED_GREENSOLID;
			if ((brakeStatus.BrakeState & BRAKESTATE_NOINPUTVOLTAGE)!= 0)
			{
				brakeBiLED = BRAKEBILED_YELLOWFLASH;
			}
			else
			{
				if ((brakeStatus.BrakeState & BRAKESTATE_INPUTVOLTAGEBAD)!= 0)
				{
					if ((brakeStatus.BrakeState & BRAKESTATE_COMMERROR)!= 0)
					{
						brakeBiLED = BRAKEBILED_YELLOWFLICKER;
					}
					else	
					{			
						brakeBiLED = BRAKEBILED_YELLOWSOLID;
					}
				}
				else
				{
					if ((brakeStatus.BrakeState & BRAKESTATE_COMMERROR)!= 0)
					{
						brakeBiLED = BRAKEBILED_GREENFLICKER;
					}
				}
			}
			brakeBlueLED = BRAKEBLUELED_OFF;	
			brakeRedLED = BRAKEREDLED_OFF; 					
			break;
		}
		case BRAKESTATE_ACTIVE_EXTEND:
		case BRAKESTATE_ACTIVE_RETRACT:
		case BRAKESTATE_ACTIVE_HOLD:
		case BRAKESTATE_END_RETRACT:
		{
			brakeBlueLED = BRAKEBLUELED_SOLID;
			brakeRedLED = BRAKEREDLED_OFF; 			
			break;
		}		
		case BRAKESTATE_ACTIVE_EXTEND_BREAKAWAY:
		case BRAKESTATE_ACTIVE_HOLD_BREAKAWAY:
		{
			brakeRedLED = BRAKEREDLED_SOLID;
			brakeBlueLED = BRAKEBLUELED_SOLID;
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
void BrakeSupervisorytask(void)
{
	uint16_t currentvalue,itemp; 
	//--------------------------------
	// check voltages
	currentvalue = ADCGetReading(ADC_INPUT_VOLTAGE);
	if (currentvalue< ADC_INPUTVOLTAGE_8)
	{
		if (voltageBadTime >= VOLTAGE_BAD_TIME)
		{
			if ((brakeStatus.BrakeState & BRAKESTATE_INPUTVOLTAGEBAD)== 0)
			{
				brakeStatus.BrakeState |= BRAKESTATE_INPUTVOLTAGEBAD;
			}
		}
		if (currentvalue < ADC_INPUTVOLTAGENONE)
		{
			brakeStatus.BrakeState |= BRAKESTATE_NOINPUTVOLTAGE;
		}
	}
	else
	{
		brakeStatus.BrakeState &= ~BRAKESTATE_NOINPUTVOLTAGE;
		if ((brakeStatus.BrakeState & BRAKESTATE_INPUTVOLTAGEBAD)!= 0)
		{
			if (currentvalue> ADC_INPUTVOLTAGE_8PT5)
			{
				brakeStatus.BrakeState &= ~BRAKESTATE_INPUTVOLTAGEBAD;	
			}
		}
		else
		{
			voltageBadTime = 0; 
		}
	}
	//-----------------------------------------
	// if voltage is over 10.5 volts and board is turned on - 
	// enable the super cap. 
	if ((brakeState != BRAKESTATE_IDLESLEEP)&&(brakeState != BRAKESTATE_POWERINGUP))
	{
		if (currentvalue> ADC_INPUTVOLTAGE_10PT5)
		{		
			port_pin_set_output_level(SUPERCAPEN, true);  
		}
		else
		{
			if (currentvalue< ADC_INPUTVOLTAGE_10PT2)
			{
				port_pin_set_output_level(SUPERCAPEN, false);  
			}
		}
	}
	else
	{
		port_pin_set_output_level(SUPERCAPEN, false);  
	}
}

uint16_t newCurrentThreshold; 
uint16_t currentRead;
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void BrakeBoardStateMachineTask(void)
{
	uint8_t i,done,button,itemp;
//	uint16_t tempx;
	
//	table0.Item.MaxForce = 5;  //hard coded for testing
	done = 0;
	button = ButtonChanged();
	BrakeSupervisorytask();
	if (breakawayRing_pressed == 1)
	{
		brakeStatus.BrakeState |= BRAKESTATE_BREAKAWAYREADY;
	}
	else
	{
		brakeStatus.BrakeState &= ~BRAKESTATE_BREAKAWAYREADY;
	}	
	switch(brakeState)
	{
//----------------------------
// POWERING UP STATES - that will retract arm from power-up. 
// this happens when the board has NO POWER and the processor was not 
// powered up and power is added. This is REALLY A FIRST TIME board has any 
// power on it and the supercaps are just going to get charged. 
//-------------------------------		
		case BRAKESTATE_POWERINGUP:
		{
			MotorOff();
			brakeState = BRAKESTATE_POWEREDUP;
			if (hlimitState != 0)
			{
				MotorCCW();
				brakeSupTime = 15;  //5 seconds to retract
				brakeChange &= ~BRAKECHANGE_SUPTIME;
			}
			brakeStatus.BrakeState = 0; 
			break;
		}
		case BRAKESTATE_POWEREDUP:
		{
			if (((brakeChange & BRAKECHANGE_SUPTIME)!= 0)||
			(hlimitState ==0))
			{
				MotorOff();
				brakeSupTime = 0;
				brakeChange &= ~BRAKECHANGE_SUPTIME;
				BrakeActuatorControl(BRAKE_HOME);
				brakeState = BRAKESTATE_IDLESLEEP;
				BrakeEnterIdleSleepMode();
			}
			break;
		}
		case BRAKESTATE_IDLESLEEP:
		{
			port_pin_set_output_level(SUPERCAPEN,false); 		
			break;
		}				
//------------------------------------
// POWER ON KEY HAS BEEN PRESSED. 
//------------------------------------		
		case BRAKESTATE_RESET:
		{
			poweredUp = 1;
			motorAccBaseline = 0; 
			thresholdmet = 0; 

			brakeStatus.ActuatorStatus = 0; 
			brakeState = BRAKESTATE_PRESETUP;
			brakeStatus.BrakeState = 0; 
#if TESTWITHOUTSETUP
#else		
			brakeStatus.BrakeState |= BRAKESTATE_NOTSETUP;	
#endif			
			system_interrupt_disable_global();
			//-----ADC--------------
			ADCInit();
			//---------------LORA/FSK radio
			whichRadio = WHICHRADIO_LORA; 
			bluetoothActive = FALSE; 
			CommInit();
			//-------RF433--- pressure sensor radio
			RF433Init();			
			
			system_interrupt_enable_global();
			ADCStart();
			//------------------
			// make surE the break away has been sampled.
			// ButtonSample - will give you the breakaway inputs, and
			//    the limits.
			//------------------
			for (i=0;i<8;i++)
			{
				ButtonSample();
			}
 			if (hlimitState != 0)
			{
				MotorCCW();
				brakeSupTime = BRAKESUPTIME;   
				brakeChange &= ~BRAKECHANGE_SUPTIME;
			}
			else
			{
				if (BrakeActuatorControl(BRAKE_HOME)==BRAKE_ERROR)
				{
					brakeState = BRAKESTATE_ERROR; 
				}
			}
			break;
		}
		case BRAKESTATE_PRESETUP:
		{
			if (((brakeChange & BRAKECHANGE_SUPTIME)!= 0)||
			(hlimitState ==0))
			{
				MotorOff();
				brakeSupTime = 0;
				brakeChange &= ~BRAKECHANGE_SUPTIME;
				if (BrakeActuatorControl(BRAKE_HOME)==BRAKE_ERROR)
				{
					brakeState = BRAKESTATE_ERROR;
				}
				else
				{
					if ((brakeStatus.BrakeState & BRAKESTATE_NOTSETUP)== 0)
					{
						brakeState = BRAKESTATE_ACTIVE;
						brakeStatus.BrakeState &= ~BRAKESTATE_NOTSETUP;
					}
					else
					{
						brakeState = BRAKESTATE_WAITONSETUP;
					}	 		
				}
			}
			break;
		}
		case BRAKESTATE_WAITONSETUP:
		{
			if (setup_pressed != 0)
			{
				brakeState = BRAKESTATE_SETUPACTIVE;
				brakeSetupExtend = 0;
				maxCurrentRead = 0;
				setupExtendTriggered = 0;
				if (flimitState != 0)
				{
					MotorCW();
					brakeSupTime = BRAKESUPTIME;   
					brakeChange &= ~BRAKECHANGE_SUPTIME;
				}
			}
			break;
		}
//--------------------------------
// ACTIVE SETUP STATES
// - SETUP key has been pressed and now doing the setup. The first EXTEND CW has 
// been started from the WAITONSETUP state. 		
		case BRAKESTATE_SETUPACTIVE_PAUSE_EXTEND:
		{
			if ((brakeChange & BRAKECHANGE_SUPTIME)!= 0) 
			{
				 
				brakeSupTime = 0;
				brakeChange &= ~BRAKECHANGE_SUPTIME;		
				brakeSetupExtend++;
				MotorCCW();
				brakeSupTime = BRAKESUPTIME;  	
				brakeState = BRAKESTATE_SETUPACTIVE;
			}
			break;
		}
		case BRAKESTATE_SETUPACTIVE_PAUSE_RETRACT:
		{
			if ((brakeChange & BRAKECHANGE_SUPTIME)!= 0)
			{
				
				brakeSupTime = 0;
				brakeChange &= ~BRAKECHANGE_SUPTIME;
				brakeSetupExtend++;
				MotorCW();
				brakeSupTime = BRAKESUPTIME;  
				brakeState = BRAKESTATE_SETUPACTIVE;
			}
			break;
		}		
		case BRAKESTATE_SETUPACTIVE:
		{
			done = 0; 
			//---------------???????? on below 
			if (((button & KEY_SETUP)!= 0)&&(setup_pressed != 0))
			{
				done = 1; 
				brakeState = BRAKESTATE_POWERINGUP;
			}
			if (brakeSetupExtend==8)
			{
				//building table 
				if ((brakeChange & BRAKECHANGE_TABLESAMPLE)!= 0)
				{
					brakeChange &= ~BRAKECHANGE_TABLESAMPLE; 
					if (buildTableOffset >= MAX_BUILDTABLE)
					{
						buildTableOffset = 0;
					}
					buildTable.Current[buildTableOffset] = ADCGetReading(ADC_INPUT_CURRENT); 
					buildTable.EncoderCount[buildTableOffset] = encoderCount;
					buildTableOffset++;
				}
			}
			if (done == 0)
			{
			//---------------------------------
			// brakeSetupExtend
			// * counts 0,2,4,6,8 - extend
			// * counts 1,3,5,7,9 - retract
			switch (brakeSetupExtend)
			{
				case 0:
				case 2:
				case 4:
				case 6:
				case 8:
				{
					if (flimitState == 0)
					{
						if (BrakeActuatorControl(BRAKE_AWAY)==BRAKE_ERROR)
						{
							brakeState = BRAKESTATE_ERROR_RETRACT;
							MotorCCW();
							brakeSupTime = BRAKESUPTIME;							
							brakeChange &= ~BRAKECHANGE_SUPTIME;
						}		
						else
						{	
							setupExtendTriggered = 1;		
//							brakeState = BRAKESTATE_SETUPACTIVE_PAUSE_EXTEND; 
//							brakeSupTime = BRAKESUPTIME_SETUPPAUSE;  
							brakeState = BRAKESTATE_ERROR_RETRACT;
							MotorCCW();
							brakeSupTime = BRAKESUPTIME;
							brakeChange &= ~BRAKECHANGE_SUPTIME;					
						}
						brakeChange &= ~BRAKECHANGE_SUPTIME;
					}
					else
					{
						if ((BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)&&(motorRunTime ==0))
						{
							brakeState = BRAKESTATE_ERROR_RETRACT;
							brakeSupTime = 0;
							MotorOff();
							MotorCCW();
							brakeSupTime = BRAKESUPTIME;
							brakeChange &= ~BRAKECHANGE_SUPTIME;							
						}	
						else
						{
							if ((brakeChange & BRAKECHANGE_SUPTIME)!= 0)
							{
								brakeChange &= ~BRAKECHANGE_SUPTIME;
								MotorOff();
								if (BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)
								{
									brakeState = BRAKESTATE_ERROR_RETRACT;
									MotorCCW();
									brakeSupTime = BRAKESUPTIME;
									brakeChange &= ~BRAKECHANGE_SUPTIME;
								}
								else
								{
									brakeState = BRAKESTATE_ERROR_RETRACT;
									MotorCCW();
									brakeSupTime = BRAKESUPTIME;
									brakeChange &= ~BRAKECHANGE_SUPTIME;
								}
								brakeChange &= ~BRAKECHANGE_SUPTIME;
							}			
							if (motorRunTime == 0)
							{
								if (((maxCurrentRead > CURRENT_THRESHOLD_SETUP)&&(brakeSetupExtend!= 8))||
								((maxCurrentRead > CURRENT_THRESHOLD_TABLEBUILD)&&(brakeSetupExtend== 8)))
								{
									MotorOff();
									if (BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)
									{
										brakeState = BRAKESTATE_ERROR_RETRACT;
										MotorCCW();
										brakeSupTime = BRAKESUPTIME;
										brakeChange &= ~BRAKECHANGE_SUPTIME;
									}
									else
									{								
										brakeState = BRAKESTATE_SETUPACTIVE_PAUSE_EXTEND; 										
										brakeSupTime = BRAKESUPTIME_SETUPPAUSE;  									
									}
									brakeChange &= ~BRAKECHANGE_SUPTIME;
								}
							}
						}
					}
					break;
				}
				case 1:
				case 3:
				case 5:
				case 7:
				case 9:
				{
					if (hlimitState == 0)
					{
						if (BrakeActuatorControl(BRAKE_HOME)==BRAKE_ERROR)
						{
							brakeState = BRAKESTATE_ERROR;
							brakeSupTime = 0;
						}
						else
						{		
							if (brakeSetupExtend == 9)
							{
								if (setupExtendTriggered != 0)
								{
									brakeState = BRAKESTATE_ERROR;
									MotorOff();									
								}
								else
								{
									brakeStatus.BrakeState &= ~BRAKESTATE_NOTSETUP;
									brakeState = BRAKESTATE_HOLDOFF_ACTIVE;
									MotorOff();
									brakeSupTime = BRAKESUPTIME_LONG;												
								}					
							}	
							else
							{
								brakeState = BRAKESTATE_SETUPACTIVE_PAUSE_RETRACT; 								
 								brakeSupTime = BRAKESUPTIME_SETUPPAUSE;  
							}
						}
						brakeChange &= ~BRAKECHANGE_SUPTIME;
					}
					else
					{
						if ((BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)&&(motorRunTime ==0))
						{
							brakeState = BRAKESTATE_ERROR;
							brakeSupTime = 0;
							MotorOff();
						}
						else
						{						
							if ((brakeChange & BRAKECHANGE_SUPTIME)!= 0)
							{
								brakeChange &= ~BRAKECHANGE_SUPTIME;
								//						brakeState = BRAKESTATE_IDLE;
								//						brakeBiLED = BRAKEBILED_SOLIDGREEN;
								//----------------ERROR here????
								MotorOff();
								if (BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)
								{
									brakeState = BRAKESTATE_ERROR;
									brakeSupTime = 0;
								}
								else
								{
									if (brakeSetupExtend == 9)
									{
										if (setupExtendTriggered != 0)
										{
											brakeState = BRAKESTATE_ERROR;
											MotorOff();									
										}
										else
										{							
											brakeStatus.BrakeState &= ~BRAKESTATE_NOTSETUP;
											brakeState = BRAKESTATE_HOLDOFF_ACTIVE;
											MotorOff();
											brakeSupTime = BRAKESUPTIME;			
										}
									}
									else
									{						
										brakeState = BRAKESTATE_SETUPACTIVE_PAUSE_RETRACT; 
										brakeSupTime = BRAKESUPTIME_SETUPPAUSE;
									}
								}
								brakeChange &= ~BRAKECHANGE_SUPTIME;
							}			
						}
					}
					break;
				}
			}
			}
			break;
		}
//----------------------------------
// error handling states
		case BRAKESTATE_ERROR_RETRACT:
		{
			if (((brakeChange & BRAKECHANGE_SUPTIME)!= 0)||
			(hlimitState ==0))
			{
				MotorOff();
				brakeSupTime = 0;
				brakeChange &= ~BRAKECHANGE_SUPTIME;
				BrakeActuatorControl(BRAKE_HOME);
				brakeState = BRAKESTATE_ERROR;
			}

			break;
		}
		case BRAKESTATE_ERROR:
		{
			//-------------testing BLUETOOTH 
			if (((button & KEY_SETUP)!= 0)&&(setup_pressed != 0))
			{
				if (bluetoothActive == TRUE)
				{
					bluetoothActive = FALSE; 
				}
				else
				{
					bluetoothActive = TRUE;
				}
			}						
			if (setupExtendTriggered != 0)	
			{
				brakeStatus.ActuatorStatus |= ACTUATORSTATUS_EXTENDTRIGGEREDINSETUP;
			}
			brakeStatus.BrakeState &= ~BRAKESTATE_MANUALBRAKE;
			break;
		}		
//-----------------------------------------		
		case BRAKESTATE_HOLDOFF_ACTIVE:
		{
			gPrime = 0; 
			if ((breakawayRing_pressed == 0) || 
			   ((breakawayRing_pressed ==1) && (breakawayTip_pressed ==0)))
			{
				brakeStatus.BrakeState &= ~BRAKESTATE_BREAKAWAYTIP;
			}						
			thresholdmet = 0; 	
			tempdiffx = MotorGetAcc(FALSE);
			if ((brakeChange & BRAKECHANGE_SUPTIME)!= 0)
			{
				brakeChange &= ~BRAKECHANGE_SUPTIME;
				brakeState = BRAKESTATE_ACTIVE; 
			}
			encoderCountBackTotal = 0;
			break;
		}

		case BRAKESTATE_ACTIVE:
		{
			done = 0; 
			//-------------testing BLUETOOTH 
			if (((button & KEY_SETUP)!= 0)&&(setup_pressed != 0))
			{
				if (bluetoothActive == TRUE)
				{
					bluetoothActive = FALSE; 
				}
				else
				{
					bluetoothActive = TRUE;
				}
			}			
			//---------------------check for breakaway
			if ((breakawayRing_pressed == 0) || 
			   ((breakawayRing_pressed ==1) && (breakawayTip_pressed ==0)))
			{
				brakeStatus.BrakeState &= ~BRAKESTATE_BREAKAWAYTIP;
			}								
 			if ((breakawayTip_pressed!=0)&&(breakawayRing_pressed != 0)&&
			        ((brakeStatus.BrakeState &BRAKESTATE_BREAKAWAYTIP)==0))
			{
				done = 1;
				brakeStatus.BrakeState |= BRAKESTATE_BREAKAWAYTIP;
				brakeState = BRAKESTATE_ACTIVE_EXTEND_BREAKAWAY;
				MotorCW();
				brakeSupTime = BRAKESUPTIME;
				brakeChange &= ~BRAKECHANGE_SUPTIME;
				thresholdmet = 0;
			}
			//---------------------check for manual
			if ((remoteStatus & REMOTE_MANUALBRAKE_ACTIVE) == 0)
			{
				brakeStatus.BrakeState &= ~BRAKESTATE_MANUALBRAKE;
			}			
			if (done == 0)
			{
				if (((remoteStatus & REMOTE_MANUALBRAKE_ACTIVE)!=0)&&
				       ((brakeStatus.BrakeState & BRAKESTATE_MANUALBRAKE)==0))
				{
 					done = 1;
					brakeStatus.BrakeState |= BRAKESTATE_MANUALBRAKE;
					brakeState = BRAKESTATE_ACTIVE_EXTEND_MANUAL;
					MotorCW();
					brakeSupTime = BRAKESUPTIME;
					brakeChange &= ~BRAKECHANGE_SUPTIME;
				}
			}					
			//------------------------
			// check accelerometer. 
			if (done==0)
			{
				tempdiffx = MotorGetAcc(FALSE);
//	 			if (tempdiffx >ACC_THREESIXTEENTHS_G)  //ACC_EIGHTH_G)  //WAS <
			    if (AccelProvideDecisions(ACC_SIXTEENTHS_G,DECISION_GREATER,motorAccXBaseline)!=0)
				{
					thresholdmet++; 
//					if (thresholdmet >= MAX_THRESHOLD_NEEDED)
//					{
						done = 1; 
						thresholdmet = MAX_THRESHOLD_NEEDED;
						brakeState = BRAKESTATE_ACTIVE_EXTEND;
						//----------------------------------
						// EXTEND - 
						// 1. set max time to 5 seconds
						// 2. set hold max time to 15 seconds 						
						MotorCW();
						brakeSupTime = BRAKESUPTIME;
						brakeChange &= ~BRAKECHANGE_SUPTIME;											
//					}
				}
				else
				{
					thresholdmet = 0; 
				}
			} 			
			else
			{
				thresholdmet = 0;				
			}

			break;
		}		

//------------------------- 
// ACTIVE EXTEND STATE 
//-------------------------		
		case BRAKESTATE_ACTIVE_EXTEND:
		{
			done = 0; 
			//------------------------
			// you are braking .... so 
			// 1. look for 500 counts on current to stop braking. 
			// 2. when at 1/10 g then retract 
			// 3. also maximum time of braking is 15 second. 
			// 
 			if ((flimitState == 0)&&(done == 0))
			{
				done = 1; 
				MotorOff();
				if (BrakeActuatorControl(BRAKE_AWAY)==BRAKE_ERROR)
				{
					brakeState = BRAKESTATE_ERROR;
					brakeSupTime = 0;
					brakeChange &= ~BRAKECHANGE_SUPTIME;
				}
				else
				{
					thresholdmet = 0; 
					brakeState = BRAKESTATE_ACTIVE_HOLD; 
					brakeChange &= ~BRAKECHANGE_DITHER;
					ditherTimer = 0;  
					if (encoderFlip != 0)
					{
						encoderFillOffset = MAX_BUILDTABLE;
					}
					else
					{
						encoderFillOffset = encoderTableOffset; 
					}
				}
				
			}	
			if (((brakeChange & BRAKECHANGE_SUPTIME)!= 0)&&(done == 0))
			{
				brakeChange &= ~BRAKECHANGE_SUPTIME;
				MotorOff();
				done = 1; 
				if (BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)
				{
					brakeState = BRAKESTATE_ERROR;
					brakeSupTime = 0;
				}
				else
				{
 					MotorCCW();
					brakeSupTime = BRAKESUPTIME;
					brakeState = BRAKESTATE_END_RETRACT_TIMEOUT;
				}
				brakeChange &= ~BRAKECHANGE_SUPTIME;
			}			
			if ((motorRunTime == 0)&&(done == 0))
			{
				currentRead = ADCGetReading(ADC_INPUT_CURRENT);
				newCurrentThreshold = CurrentMotorCalculated();
				if (currentRead > newCurrentThreshold)   
				{
					MotorOff();
					if (BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)
					{
						done = 1; 
						brakeState = BRAKESTATE_ERROR;
						brakeSupTime = 0;
						brakeChange &= ~BRAKECHANGE_SUPTIME;
					}
					else
					{
						done =1; 
						thresholdmet = 0;
						brakeState = BRAKESTATE_ACTIVE_HOLD;  		
						brakeChange &= ~BRAKECHANGE_DITHER;
						ditherTimer = 0;  	
						holdG = instantGain; 	
						if (encoderFlip != 0)
						{
							encoderFillOffset = MAX_BUILDTABLE;
						}
						else
						{
							encoderFillOffset = encoderTableOffset;
						}													
					}
				}
			}
#if 0			
			if (done == 0)
			{
				tempdiffx = MotorGetAcc(TRUE);				
//				if (tempdiffx!= 0)
//				{
				    if (AccelProvideDecisions(ACC_TENTH_G,DECISION_LESS,motorAccXBaseline)!=0)					
//		 			if (tempdiffx < ACC_TENTH_G)   
					{
						thresholdmet++; 
//						if (thresholdmet >= MAX_THRESHOLD_NEEDED)
//						{
							done = 1; 
							thresholdmet = 0;
							MotorCCW();
							brakeSupTime = BRAKESUPTIME;   
							brakeState = BRAKESTATE_END_RETRACT;  
							brakeChange &= ~BRAKECHANGE_SUPTIME;				
//						}
					}
					else
					{
						thresholdmet = 0; 
					}			
//				} 						
			}
#endif			
			break;
		}				
		case BRAKESTATE_ACTIVE_HOLD:
		{
			done = 0;
			//------------------------
			// you are braking .... so
			// 1. look for 500 counts on current to stop braking.
			// 2. when at 1/10 g then retract
			// 3. also maximum time of braking is 15 second.
			//
			if (((brakeChange & BRAKECHANGE_SUPTIME)!= 0)&&(done == 0))
			{
				brakeChange &= ~BRAKECHANGE_SUPTIME;
				MotorOff();
				done = 1; 
				if (BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)
				{
					brakeState = BRAKESTATE_ERROR;
					brakeSupTime = 0;
				}
				else
				{
 					MotorCCW();
					brakeSupTime = BRAKESUPTIME;
					brakeState = BRAKESTATE_END_RETRACT_TIMEOUT;
				}
				brakeChange &= ~BRAKECHANGE_SUPTIME;
			}			
			if (done == 0)
			{
				tempdiffx = MotorGetAcc(TRUE);				
//				if (tempdiffx!= 0)
//				{
						//WAS ACC_TENTH_G
				    if (AccelProvideDecisions(ACC_TWENTYITH_G,DECISION_LESS,motorAccXBaseline)!=0)					
//		 			if (tempdiffx < ACC_TENTH_G)   
					{
						thresholdmet++; 
//						if (thresholdmet >= MAX_THRESHOLD_NEEDED)
//						{
							done = 1; 
							thresholdmet = 0;
							MotorCCW();
							brakeSupTime = BRAKESUPTIME;   
							brakeState = BRAKESTATE_END_RETRACT;  
							brakeChange &= ~BRAKECHANGE_SUPTIME;				
//						}
					}
					else
					{
						thresholdmet = 0; 
					}			
//				} 			
//				else
//				{
//					thresholdmet = 0;				
//				}			
			}
#if 0  //NO DITHER			
			if ((done == 0)&&((brakeChange & BRAKECHANGE_DITHER)!=0))
			{
				brakeChange &= ~BRAKECHANGE_DITHER; 
				//--------------------------
				// at 100msec check gain value 
				newG = MotorGetAcc(TRUE);
				if (newG < holdG)
				{
					diffG = (holdG - newG); 
					if (diffG > ACC_DITHER_TRIGGER_G)
					{
						//-------------- Retract 
						thresholdmet = 0;
						brakeState = BRAKESTATE_ACTIVE_RETRACT;  		
						brakeChange &= ~BRAKECHANGE_DITHER;
						ditherTimer = 0;  	
						//---------------------
						// Calculate the current from the equation for the g 
						matchCurrent = CurrentMotorCalculatedForG(newG);
						encoderCountBack = MotorFindEncoderMatch(matchCurrent);
						if (encoderCountBack > MAX_ENCODERCOUNT_BACK)
						{
							encoderCountBack = MAX_ENCODERCOUNT_BACK; 
						}
						encoderCountBackTotal += encoderCountBack; 
						//--------------------------
						// Calculate new encoderTableOffset 
						if (encoderTableOffset < encoderCountBack)
						{
							itemp = encoderCountBack;
							itemp -= encoderTableOffset; 
							itemp = MAX_BUILDTABLE - itemp; 
							encoderTableOffset = itemp; 
						}
						else
						{
							encoderTableOffset-= encoderCountBack; 
						}
						RetractByEncoderCount();	
						holdG = newG; 	
					}
				}				
				else
				{
					diffG = (newG - holdG);
					if (diffG > ACC_DITHER_TRIGGER_G)
					{
						//---------------Extend 
						thresholdmet = 0;
						brakeState = BRAKESTATE_ACTIVE_EXTEND;  		
						brakeChange &= ~BRAKECHANGE_DITHER;
						ditherTimer = 0;  	
						MotorExtendMore(); 								
					} 
				}				
			}
#endif			
			break;
		}		
		case BRAKESTATE_ACTIVE_RETRACT:
		{
			done = 0;
 			if ((hlimitState == 0)&&(done == 0))
			{
				done = 1; 
				if (BrakeActuatorControl(BRAKE_HOME)==BRAKE_ERROR)
				{
					brakeState = BRAKESTATE_ERROR;
					brakeSupTime = 0;
				}
				else
				{
					brakeState = BRAKESTATE_HOLDOFF_ACTIVE;
					MotorOff();
					brakeSupTime = BRAKESUPTIMESHORT;
				}
			}
			if (((brakeChange & BRAKECHANGE_SUPTIME)!= 0)&&(done == 0))
			{
				done = 1; 
				brakeChange &= ~BRAKECHANGE_SUPTIME;
				MotorOff();
				if (BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)
				{
					brakeState = BRAKESTATE_ERROR;
					brakeSupTime = 0;
				}
				else
				{
					MotorCCW();
					brakeSupTime = BRAKESUPTIME;
					brakeState = BRAKESTATE_END_RETRACT_TIMEOUT;
				}
			}			
			if ((done == 0)&&(action == NONE)&&((brakeChange & BRAKECHANGE_DITHER)!=0))
			{
				brakeChange &= ~BRAKECHANGE_DITHER; 
				//--------------------------
				// at 100msec check gain value 
				newG = MotorGetAcc(TRUE);
				if (newG < holdG)
				{
					diffG = (holdG - newG); 
					if (diffG > ACC_DITHER_TRIGGER_G)
					{
						//---------------------
						// Calculate the current from the equation for the g 
						matchCurrent = CurrentMotorCalculatedForG(newG);
						itemp = MotorFindEncoderMatch(matchCurrent);
						if (itemp > MAX_ENCODERCOUNT_BACK)
						{
							itemp = MAX_ENCODERCOUNT_BACK; 
						}
						encoderCountBackTotal += itemp; 
						//--------------------------
						if (encoderCountBackTotal <1000)
						{
							encoderCountBack = itemp; 
							RetractByEncoderCount();	
							holdG = newG; 	
						}
					}
				}			
				else
				{
					diffG = (newG - holdG);
					if (diffG > ACC_DITHER_TRIGGER_G)
					{
						//---------------Extend
						thresholdmet = 0;
						brakeState = BRAKESTATE_ACTIVE_EXTEND;
						brakeChange &= ~BRAKECHANGE_DITHER;
						ditherTimer = 0;
						MotorExtendMore();
 									
					} 
				}							
			}  			
			break;
		}
//--------------------------------
// END OF A BRAKING OPERATION
// RETRACTING.
//-------------------------------		
		case BRAKESTATE_END_RETRACT_TIMEOUT:
		case BRAKESTATE_END_RETRACT:
		case BRAKESTATE_END_RETRACT_BREAKAWAY:
		case BRAKESTATE_END_RETRACT_MANUAL:
		{
 			if (hlimitState == 0)
			{
				MotorOff();
				if (BrakeActuatorControl(BRAKE_HOME)==BRAKE_ERROR)
				{
					brakeState = BRAKESTATE_ERROR;
					brakeSupTime = 0;
				}
				else
				{
					brakeSupTime = BRAKESUPTIMESHORT;
					if (brakeState == BRAKESTATE_END_RETRACT_TIMEOUT)
					{
						brakeSupTime = BRAKESUPTIME_TIMEOUT;	
					}
					brakeState = BRAKESTATE_HOLDOFF_ACTIVE;
					
					if ((remoteStatus & REMOTE_MANUALBRAKE_ACTIVE) == 0)
					{
						brakeStatus.BrakeState &= ~BRAKESTATE_MANUALBRAKE;
					}
				}
				brakeChange &= ~BRAKECHANGE_SUPTIME;
			}
			else
			{
				if ((brakeChange & BRAKECHANGE_SUPTIME)!= 0)
				{
					brakeChange &= ~BRAKECHANGE_SUPTIME;
					MotorOff();
					if (BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)
					{
						brakeState = BRAKESTATE_ERROR;
						brakeSupTime = 0;
					}
					else
					{
					brakeSupTime = BRAKESUPTIMESHORT;
						if (brakeState == BRAKESTATE_END_RETRACT_TIMEOUT)
						{
							brakeSupTime = BRAKESUPTIME_TIMEOUT;	
						}						
						brakeState = BRAKESTATE_HOLDOFF_ACTIVE;
						if ((remoteStatus & REMOTE_MANUALBRAKE_ACTIVE) == 0)
						{
							brakeStatus.BrakeState &= ~BRAKESTATE_MANUALBRAKE;
						}
					}
					brakeChange &= ~BRAKECHANGE_SUPTIME;
				}
			}
			break;
		}		
//-----------------------
// BREAKAWAY 		
		case BRAKESTATE_ACTIVE_EXTEND_BREAKAWAY:
		{	
			brakeStatus.BrakeState |= BRAKESTATE_BREAKAWAYTIP;
			if ((breakawayRing_pressed == 0) || 
			   ((breakawayRing_pressed ==1) && (breakawayTip_pressed ==0)))
			{			
					done = 1;
					MotorCCW();
					brakeSupTime = BRAKESUPTIME;
					brakeState = BRAKESTATE_END_RETRACT_BREAKAWAY;
					brakeChange &= ~BRAKECHANGE_SUPTIME;
					brakeStatus.BrakeState &= ~BRAKESTATE_BREAKAWAYTIP;
			}
			if (((brakeChange & BRAKECHANGE_SUPTIME)!= 0)&&(done == 0))
			{
				done = 1;
				brakeChange &= ~BRAKECHANGE_SUPTIME;
				MotorOff();
				if (BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)
				{
					brakeState = BRAKESTATE_ERROR;
					brakeSupTime = 0;
				}
				else
				{
					MotorCCW();
					brakeSupTime = BRAKESUPTIME;
					brakeState = BRAKESTATE_END_RETRACT_BREAKAWAY;
					brakeChange &= ~BRAKECHANGE_SUPTIME;
				}
			}						
 			if ((flimitState == 0)&&(done == 0))
			{
				done = 1; 
				if (BrakeActuatorControl(BRAKE_AWAY)==BRAKE_ERROR)
				{
					brakeState = BRAKESTATE_ERROR;
					brakeSupTime = 0;
				}
				else
				{
					MotorOff();
					brakeState = BRAKESTATE_ACTIVE_HOLD_BREAKAWAY; 
				}
				brakeChange &= ~BRAKECHANGE_SUPTIME;
			}
			if ((motorRunTime == 0)&&(done == 0))
			{
				currentRead = ADCGetReading(ADC_INPUT_CURRENT);
				newCurrentThreshold = CurrentMotorCalculatedBreakaway();
				if (currentRead > newCurrentThreshold)   
				{
					MotorOff();
					if (BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)
					{
						brakeState = BRAKESTATE_ERROR;
						brakeSupTime = 0;
					}
					else
					{
						brakeState = BRAKESTATE_ACTIVE_HOLD_BREAKAWAY;  			
					}
					brakeChange &= ~BRAKECHANGE_SUPTIME;
				}
			}			
			break;
		}
		case BRAKESTATE_ACTIVE_HOLD_BREAKAWAY:
		{
			done = 0;
			brakeStatus.BrakeState |= BRAKESTATE_BREAKAWAYTIP;
			//------------------------
			// you are braking ....  
			if ((breakawayRing_pressed == 0) || 
			   ((breakawayRing_pressed ==1) && (breakawayTip_pressed ==0)))
			{		
				//----------------------------
				// Hold until cleared by: break-away signal 
				// returning to normal, set-up button pressed 
				// on brake unit or cleared with remote.  	
				done = 1;
				MotorCCW();
				brakeSupTime = BRAKESUPTIME;
				brakeState = BRAKESTATE_END_RETRACT_BREAKAWAY;
				brakeChange &= ~BRAKECHANGE_SUPTIME;
				brakeStatus.BrakeState &= ~BRAKESTATE_BREAKAWAYTIP;
			}
			if (done == 0)
			{
				if ((button & KEY_SETUP)!=0)
				{
					if (setup_pressed == 1)
					{
						done = 1;
						brakeChange &= ~BRAKECHANGE_HOLDTIME;
						MotorCCW();
						brakeSupTime = BRAKESUPTIME;
						brakeState = BRAKESTATE_END_RETRACT_BREAKAWAY;
						brakeChange &= ~BRAKECHANGE_SUPTIME;
					}
				}	
			}		
			if (done == 0)
			{
				if ((remoteStatus & REMOTE_CLEARBREAKAWAY)!=0)
				{
						done = 1;
						brakeChange &= ~BRAKECHANGE_HOLDTIME;
						MotorCCW();
						brakeSupTime = BRAKESUPTIME;
						brakeState = BRAKESTATE_END_RETRACT_BREAKAWAY;
						brakeChange &= ~BRAKECHANGE_SUPTIME;
				}	
			}									
			break;
		}	
	//--------------------------------
	// MANUAL STATES. 
	//--------------------------------		
		case BRAKESTATE_ACTIVE_EXTEND_MANUAL:
		{
			if (((brakeChange & BRAKECHANGE_SUPTIME)!= 0)&&(done == 0))
			{
				done = 1;
				brakeChange &= ~BRAKECHANGE_SUPTIME;
				MotorOff();
				if (BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)
				{
					brakeState = BRAKESTATE_ERROR;
					brakeSupTime = 0;
				}
				else
				{
					MotorCCW();
					brakeSupTime = BRAKESUPTIME;
					brakeState = BRAKESTATE_END_RETRACT_MANUAL;
					brakeChange &= ~BRAKECHANGE_SUPTIME;
				}
			}			
			if ((flimitState == 0)&&(done == 0))
			{
				done = 1;
				if (BrakeActuatorControl(BRAKE_AWAY)==BRAKE_ERROR)
				{
					brakeState = BRAKESTATE_ERROR;
					brakeSupTime = 0;
				}
				else
				{
					MotorOff();
					brakeState = BRAKESTATE_ACTIVE_HOLD_MANUAL;
				}
				brakeChange &= ~BRAKECHANGE_SUPTIME;
			}
			if ((motorRunTime == 0)&&(done == 0))
			{
				currentRead = ADCGetReading(ADC_INPUT_CURRENT);
				newCurrentThreshold = CurrentMotorCalculatedManual();
				if (currentRead > newCurrentThreshold)
				{
					MotorOff();
					if (BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)
					{
						brakeState = BRAKESTATE_ERROR;
						brakeSupTime = 0;
						brakeChange &= ~BRAKECHANGE_SUPTIME;
					}
					else
					{
						brakeState = BRAKESTATE_ACTIVE_HOLD_MANUAL;
					}
				}
			}
			//------------------------
			// you are braking ....
			if (done == 0)
			{
				//----------------------------
				// Hold until cleared by: break-away signal
				// returning to normal, set-up button pressed
				// on brake unit or cleared with remote.
				if ((remoteStatus & REMOTE_MANUALBRAKE_ACTIVE) == 0)
				{
					done = 1;
					MotorCCW();
					brakeSupTime = BRAKESUPTIME;
					brakeState = BRAKESTATE_END_RETRACT_MANUAL;
					brakeChange &= ~BRAKECHANGE_SUPTIME;
				}
			}			
			if (done == 0)
			{
				brakeStatus.BrakeState |= BRAKESTATE_MANUALBRAKE;
			}
			break;
		}	
		case BRAKESTATE_ACTIVE_HOLD_MANUAL:
		{
			done = 0;
			if (((brakeChange & BRAKECHANGE_SUPTIME)!= 0)&&(done == 0))
			{
				done = 1;
				brakeChange &= ~BRAKECHANGE_SUPTIME;
				MotorOff();
				if (BrakeActuatorControl(BRAKE_RUN)==BRAKE_ERROR)
				{
					brakeState = BRAKESTATE_ERROR;
					brakeSupTime = 0;
				}
				else
				{
					MotorCCW();
					brakeSupTime = BRAKESUPTIME;
					brakeState = BRAKESTATE_END_RETRACT_MANUAL;
					brakeChange &= ~BRAKECHANGE_SUPTIME;
				}
			}			
			//------------------------
			// you are braking ....
			if (done == 0)
			{
				//----------------------------
				// Hold until cleared by: break-away signal
				// returning to normal, set-up button pressed
				// on brake unit or cleared with remote.
				if ((remoteStatus & REMOTE_MANUALBRAKE_ACTIVE) == 0)
				{
					done = 1;
					MotorCCW();
					brakeSupTime = BRAKESUPTIME;
					brakeState = BRAKESTATE_END_RETRACT_MANUAL;
					brakeChange &= ~BRAKECHANGE_SUPTIME;
				}
			}
			if (done == 0)
			{
				brakeStatus.BrakeState |= BRAKESTATE_MANUALBRAKE;
			}
			break;
		}	
	}
	BrakeLEDControl();
}

 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
int16_t MotorGetAcc(uint8_t activeBraking)
{
	int16_t mytempdiffx,newMotorx;
	uint8_t change; 
	int32_t ltemp;
	
	mytempdiffx = 0;
 	//------------------------
	// check accelerometer. 
	if (AccelProvideReadingChange(&motorx,&motory,&motorz,&change)!= 0)
	{
		//-------------------------
		// only update rolling average if AT HOME 
		// and the 20 sample accelertor average has been updated. 
		//------------------------------------
		if ((change != 0)&&(hlimitState == 0))
		{	
			newMotorx = (int16_t)motorx; 
			ltemp = (int32_t)motorAccXBaseline; 
			if (newMotorx >0)
			{
				ltemp = ltemp * 1000;
				ltemp += newMotorx; 
				ltemp = ltemp/1001;
			}
			else
			{
				ltemp = ltemp * 5000;
				ltemp += newMotorx; 
				ltemp = ltemp/5001;				
			}
			if (motorAccBaseline == 0)
			{
				ltemp = newMotorx; 
				motorAccBaseline = 1;			
			}
			motorAccXBaseline = (int16_t)ltemp;
			motorAccYBaseline = (int16_t)motory; 
			motorAccZBaseline = (int16_t)motorz; 
		}		
		tempx = (int16_t)motorx;
		tempbasex = (int16_t) motorAccXBaseline;
		mytempdiffx = tempx- tempbasex; 
 		
	}
	return mytempdiffx;
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
#define FM 7
#define GAIN 5 

uint32_t ltemp; 
uint8_t fm; 
uint8_t gain;
uint16_t CurrentMotorCalculated(void)
{
	uint32_t ltemp1,temp2;
	uint16_t returnCurrent;
	
	returnCurrent = 0;
	fm = table0.Item.ForceMaxSet;  
	gain = table0.Item.MaxForce;
	
	if ((fm<1)||(fm>9))
	{
		fm = 5;
	}
	if ((gain<1)||(gain>9))
	{
		gain = 5;
	}		 
	instantGain = MotorGetAcc(TRUE);
	if (instantGain > 0)
	{
//		instantGain = -1 * instantGain; 
//		instantGain =1; 
	}
	else
	{
		instantGain =0;  //1; 
//		instantGain = -1 * instantGain; 
	}
//	instantGain = ACC_POINT4_G;  //test
//	if (instantGain <= 0)
//	{
//		instantGain = -1 * instantGain; 
		//-----------------------
		// g' = g * G/5 and limite to .5g 
		// *multiply by 10 to get to whole numbers 
		temp2 = instantGain * (gain*10)/5;
		temp2 = temp2/10; 
		if (temp2 > ACC_HALF_G)
		{
			temp2 = ACC_HALF_G; 
		}
		ltemp = temp2;
		ltemp = ltemp * 100; 
		ltemp = ltemp/ACC_ONE_G;
		temp2 = ltemp; 		
		//----------------------------
		ltemp1 = temp2;
		gPrime = ltemp1; 
		ltemp1 = (ltemp1 * fm * 100);
		ltemp1 = 15000 + ltemp1;
		ltemp1 = ltemp1 * 205; 
		ltemp1 = ltemp1/100;
		ltemp1 = ltemp1/100;
		returnCurrent = ltemp1; 
//	}
//	else
//	{
//		returnCurrent = 1; 
//	}
	return returnCurrent;
}
 
uint16_t CurrentMotorCalculatedForG(uint16_t gin)
{
	
	uint32_t ltemp1,ltemp,temp2;
	uint8_t fm; 
	uint16_t returnCurrent,iGain;
	
	returnCurrent = 0;
	fm = FM;  
	fm = table0.Item.ForceMaxSet;
	gain = table0.Item.MaxForce;
	if ((fm<1)||(fm>9))
	{
		fm = 5;
	}
	if ((gain<1)||(gain>9))
	{
		gain = 5;
	}
			 
	iGain = gin;
	if (iGain != 0)
	{
		//-----------------------
		// g' = g * G/5 and limite to .5g 
		// *multiply by 10 to get to whole numbers 
		temp2 = iGain * (gain*10)/5;
		temp2 = temp2/10; 
		if (temp2 > ACC_HALF_G)
		{
			temp2 = ACC_HALF_G; 
		}
		ltemp = temp2;
		ltemp = ltemp * 100; 
		ltemp = ltemp/ACC_ONE_G;
		temp2 = ltemp; 		
		//----------------------------
		ltemp1 = temp2;
		ltemp1 = (ltemp1 * fm * 100);
		ltemp1 = 15000 + ltemp1;
		ltemp1 = ltemp1 * 205; 
		ltemp1 = ltemp1/100;
		ltemp1 = ltemp1/100;
		returnCurrent = ltemp1; 
	}
	return returnCurrent;
} 
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
uint16_t CurrentMotorCalculatedBreakaway(void)
{
	uint32_t temp1;
	uint16_t returnCurrent;
	
	returnCurrent = 0;
	fm = FM;  
	fm = table0.Item.ForceMaxSet;
	gain = table0.Item.MaxForce;
	if ((fm<1)||(fm>9))
	{
		fm = 5;
	}
	if ((gain<1)||(gain>9))
	{
		gain = 5;
	}
 	//----------------------------
	temp1 = 1500 + (fm * 375);
	temp1 = temp1 * 205;
	temp1 = temp1/1000;
	returnCurrent = temp1;
	return returnCurrent;
} 
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
uint16_t CurrentMotorCalculatedManual(void)
{
	uint32_t temp1,temp2;
	uint16_t returnCurrent;
	
	returnCurrent = 0;
	fm = table0.Item.ForceMaxSet;  
	gain = table0.Item.MaxForce;
	if ((fm<1)||(fm>9))
	{
		fm = 5;
	}
	if ((gain<1)||(gain>9))
	{
		gain = 5;
	}
	//----------------------------
	temp1 = 1500 + (fm * gain * 56);
	//---------------------
	// to make it 75% divide now by 100
	//---------------------
	temp1 = temp1 * 205;
	temp1 = temp1/1000;
	returnCurrent = temp1;
	return returnCurrent;
} 

//XXXXXXXXXXXXXXXXXXXXXXXXX MOTOR SETUP STUFF AND RUNNING XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX 

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void ConfigureMotorFLimitChannel(void)
{
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin = PIN_PA21A_EIC_EXTINT5;
	config_extint_chan.gpio_pin_mux = MUX_PA21A_EIC_EXTINT5;
	config_extint_chan.gpio_pin_pull = EXTINT_PULL_NONE;
	config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
	extint_chan_set_config(5, &config_extint_chan);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void ConfigureMotorFLimitCallbacks(void)
{
	extint_register_callback(MotorFLimitCallback,5,EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(5,EXTINT_CALLBACK_TYPE_DETECT);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void MotorFLimitCallback(void)
{
	schedByte |= SCHEDBYTE_MOTORFLIMIT;
	flimitState = port_pin_get_input_level(FLIMIT);		
	if (flimitState == 0)
	{
		MotorOff();
	}
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void ConfigureMotorHLimitChannel(void)
{
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin = PIN_PA20A_EIC_EXTINT4;   
	config_extint_chan.gpio_pin_mux = MUX_PA20A_EIC_EXTINT4;   
	config_extint_chan.gpio_pin_pull = EXTINT_PULL_NONE;
	config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH; 
	extint_chan_set_config(4, &config_extint_chan);   
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void ConfigureMotorHLimitCallbacks(void)
{
	extint_register_callback(MotorHLimitCallback,4,EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(4,EXTINT_CALLBACK_TYPE_DETECT);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void MotorHLimitCallback(void)
{
	schedByte |= SCHEDBYTE_MOTORHLIMIT;
	hlimitState = port_pin_get_input_level(HLIMIT);		
	if (hlimitState == 0)
	{
		MotorOff();
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void MotorHLimitTask(void)
{
	hlimitCount++;	
	hlimitState = port_pin_get_input_level(HLIMIT);	
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void MotorFLimitTask(void)
{
	flimitCount++;
	flimitState = port_pin_get_input_level(FLIMIT);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void MotorInit(void)
{
	hlimitCount = 0;
	flimitCount = 0; 
	
	ConfigureMotorHLimitChannel();
	ConfigureMotorHLimitCallbacks();
	
	ConfigureMotorFLimitChannel();
	ConfigureMotorFLimitCallbacks();	
	
	flimitState = port_pin_get_input_level(FLIMIT);
	hlimitState = port_pin_get_input_level(HLIMIT);	
	
	ConfigureEncoder();
	ConfigureEncoderCallbacks();
	encoderCount = 0; 
	buildTableOffset = 0;	
}
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   MotorOff
//------------------------------------------------------------------------------
// This function Initializes registers to allow button interrupts
//==============================================================================
void MotorOff(void)
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);	
  
	motorOn = FALSE; 
	motorRunTime = 0;	
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(ENa, &pin_conf);
	port_pin_set_output_level(ENa, false);
	port_pin_set_config(ENb, &pin_conf);
	port_pin_set_output_level(ENb, false);
	port_pin_set_config(INa, &pin_conf);
	port_pin_set_output_level(INa, false);
	port_pin_set_config(INb, &pin_conf);
	port_pin_set_output_level(INb, false);
	action = NONE; 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   MotorCW
//------------------------------------------------------------------------------
// This function
//==============================================================================
void RetractByEncoderCount(void)
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);
	
	motorOn = TRUE; 
	port_pin_set_output_level(INa, false);
	port_pin_set_output_level(INb, true);
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(ENa, &pin_conf);
	port_pin_set_config(ENb, &pin_conf);
	action = RETRACTING_BY_ENCODER; 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   MotorCW
//------------------------------------------------------------------------------
// This function
//==============================================================================
void MotorCCW(void)
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);
	
	motorOn = TRUE; 
	motorRunTime = MOTOR_RUN_TIME;	
	maxCurrentRead = 0; 
	encoderCount = 0;
	buildTableOffset = 0;	
	port_pin_set_output_level(INa, false);
	port_pin_set_output_level(INb, true);
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(ENa, &pin_conf);
	port_pin_set_config(ENb, &pin_conf);
	action = RETRACTING; 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   MotorCW
//------------------------------------------------------------------------------
// This function  
//==============================================================================
void MotorCW(void)
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);	

	motorOn = TRUE; 
	motorRunTime = MOTOR_RUN_TIME;	
	maxCurrentRead = 0;
	encoderCount = 0;	
	encoderFlip = 0; 
	buildTableOffset = 0;
	encoderTableOffset = 0; 
	port_pin_set_output_level(INa, true);
	port_pin_set_output_level(INb, false);
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(ENa, &pin_conf);
	port_pin_set_config(ENb, &pin_conf);
	action = EXTENDING; 
	
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   MotorCW
//------------------------------------------------------------------------------
// This function
//==============================================================================
void MotorExtendMore(void)
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	motorOn = TRUE;
	motorRunTime = MOTOR_RUN_TIME;
	maxCurrentRead = 0;
//	encoderCount = 0;
//	encoderFlip = 0;
	buildTableOffset = 0;
//	encoderTableOffset = 0;
	port_pin_set_output_level(INa, true);
	port_pin_set_output_level(INb, false);
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(ENa, &pin_conf);
	port_pin_set_config(ENb, &pin_conf);
	action = EXTENDING;
	
}


#endif
