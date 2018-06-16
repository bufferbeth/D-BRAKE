//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: mainc
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor: STM32F103R
// TOOLS: IAR Workbench
// DATE:
// CONTENTS: This file contains
//------------------------------------------------------------------------------
// HISTORY: This file
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//
//******************************************************************************

#include <asf.h>
#include "dbrakeDefs.h"
#include "driverXSPI.h"
#include "driverASPI.h"
#include "driverTSPI.h"
#include "sx1272.h"
#include "driverLCD.h"
#include "driverbuttons.h"
#include "appLCD.h"
#include "sx1272-fsk.h"
#include "sx1272-lora.h"
#include "radio.h"
#include "config.h"
#include "appAccel.h"
#include "appMotor.h"
#include "driverADC.h"
#include "driverSpeaker.h"
#include "appProtocol.h"
#include "driverScreenColor.h"
#include "bod_feature.h"
#include "appbluetooth.h"
#include "driverProgramming.h"
#include "nvm.h"
#include "driverDownload.h"
#include "driverUSART.h" 

int SPImain(void);
int I2Cmain(void); 

//---------------------GLOBAL VARIABLES-----------------------------------
	//----------------FCC TESTING
uint8_t setTXContinuous;
uint8_t setCW;
uint8_t setRXContinuous;
 
	//----------------
	//which radio type 
uint8_t switchOnTransmit = 0; 
uint8_t whichRadio; 

  //--------------------------------------
  // scheduling of tasks
unsigned char schedCount;
unsigned char schedDone;
unsigned int schedByte; 

unsigned char programming = 0;
unsigned char mainLineTask = 0; 

uint16_t blockingTime;
int wdogTimer;
uint16_t commSupTimer;
uint8_t commErrorCount;
uint8_t commFailureCount; 
uint16_t blinkTimer; 
	#define BLINKTIME 250   //.25 sec
uint16_t flickTimer; 
	#define FLICKERTIME 1000   //1 sec total with 900 on/ 100 off
	#define FLICKOFF 900
uint16_t strobeTimer;
	#define STROBETIME 2000   //2 sec total with 100 on/ 1900 off
	#define STROBEOFF 100

#if BRAKEBOARD
uint8_t	ledBlue;
uint8_t ledRed;
uint8_t	ledBiGreen; 
uint8_t ledBiRed;
uint8_t supercapState; 
uint8_t poweredUp;
#endif 
	//--------------statusData
	// keeps track of interface status 
STATUSDATA statusData; 
STATUSDATA statusBrake;
	//------------on the brake this is the current status 
	//            on the remote ... this is received from the brake
BRAKEDATA  brakeStatus;  

uint8_t eventMessageReceived; 

//---------------------LOCAL VARIABLES------------------------------------
//--------------------------
uint16_t timerSecond; 
uint32_t TickCounter = 0;
uint16_t timerAccelerometer = 0;
uint16_t timerRF433 = 0; 
uint16_t wdog;
uint8_t hundredMSec; 
uint8_t brakeSupTime; 
uint8_t brakeHoldTime; 
uint8_t twentyfiveMSec; 
uint8_t downloadTimer = 0;
uint8_t bluetoothHoldTimer100msec = 0;

#if REMOTEBOARD
uint16_t timerBrakeComm;
#endif

//---------------------LOCAL FUNCTION PROTOTYPES--------------------------
void configure_tc(void);
void configure_tc_callbacks(void);
void tc_callback_to_toggle_led(struct tc_module *const module_inst);
void NEXTMSG(void);
void AppStatusInitialization(void);
bool pinState; 
void configure_wdt(void);
void UsartMain(void);
void CommSupTask(void);
void EmptyTask(void);


void EmptyTask(void)
{
	
}

typedef void swTask(void);
swTask *const SwTaskList[16] =
{
	AccelProcess,	

#if REMOTEBOARD
	AppScreenUpdateHome,
	EmptyTask, 
#else
	MotorHLimitTask,
	MotorFLimitTask,
#endif	  
	ButtonSample,
	RF433Task,
	ADCTask,
#if REMOTEBOARD
EmptyTask,
#else
BrakeBoardStateMachineTask,
#endif
#if REMOTEBOARD	
	AppScreenProcessKeyChange,
#else
	EmptyTask,
#endif	
	PressureUpdateTask,
	AppFskTask,
	AppLoraTask,	
#if REMOTEBOARD
	BrakeCommTask,
#else	
	EmptyTask,
#endif	
	EmptyTask,  //ButtonSample,
	CommSupTask,
	DownloadDoneTask,
 
	EmptyTask,
};



//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
static void configure_bod33(void)
{
	struct bod_config config_bod33;
	bod_get_config_defaults(&config_bod33);
	bod_set_config(BOD_BOD33, &config_bod33);
	bod_enable(BOD_BOD33);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//	enum system_reset_cause reset_cause = system_get_reset_cause();
//	if (reset_cause == SYSTEM_RESET_CAUSE_WDT) {
//		port_pin_set_output_level(LED_0_PIN, LED_0_INACTIVE);
//	}
//	else {
//		port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
//	}
//==============================================================================
void configure_wdt(void)
{
	//------------------------------------
	// Create a new configuration structure 
	// for the Watchdog settings and fill
	// with the default module settings. 
	//----------------------------------- 
	struct wdt_conf config_wdt;
	 
	wdt_get_config_defaults(&config_wdt);
	 
	config_wdt.always_on      = false;
#if !(SAML21)
	config_wdt.clock_source   = GCLK_GENERATOR_4;
#endif
	config_wdt.timeout_period = WDT_PERIOD_16384CLK;  //WDT_PERIOD_2048CLK;
 
	wdt_set_config(&config_wdt);
 
}
 

void deconfigure_wdt(void)
{
	//------------------------------------
	// Create a new configuration structure 
	// for the Watchdog settings and fill
	// with the default module settings. 
	//----------------------------------- 
	struct wdt_conf config_wdt;
	 
	wdt_get_config_defaults(&config_wdt);
	config_wdt.enable               = false;
	config_wdt.always_on      = false;
#if !(SAML21)
	config_wdt.clock_source   = GCLK_GENERATOR_4;
#endif
	config_wdt.timeout_period = WDT_PERIOD_16384CLK;  //WDT_PERIOD_2048CLK;
 
	wdt_set_config(&config_wdt);
 
}


uint8_t backlightSelect; 
uint8_t newSW3;
uint8_t prevSW3;
uint8_t demoSelect; 
uint8_t newSW4;
uint8_t prevSW4;
uint8_t motorTest; 	
uint8_t message;

uint8_t testI2C;
void HardDelay(void)
{
	uint32_t j; 
	for (j=0;j<0x0080;j++)
	{
		testI2C++;
	}
	
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
int main(void)
{
	uint8_t button,i; 
	struct nvm_config config;
	enum status_code error_code; 		
	struct port_config pin_conf;
	
	system_interrupt_disable_global();
 
	port_get_config_defaults(&pin_conf);
	//-------------------------------
	// Initialize the SAM system 
    //-------------------------------
    system_init();	

	configure_bod33();	
	//-------------------------------
	// initialize the status data before 
	// initializing any of the interfaces. 
	//------------------------------
	AppStatusInitialization(); 
	//-------------------------------
	// different board hardware configuration
	
#if BRAKEBOARD
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA12, &pin_conf);
	port_pin_set_output_level(PIN_PA12, FALSE);
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA13, &pin_conf);
	port_pin_set_output_level(PIN_PA13, FALSE);
		
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA13, &pin_conf);
	port_pin_set_output_level(PIN_PA13, TRUE);		
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA12, &pin_conf);
	port_pin_set_output_level(PIN_PA12, TRUE);		
	
	for (i=0;i<16;i++)
	{
		HardDelay();
		pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
		port_pin_set_config(PIN_PA13, &pin_conf);
		port_pin_set_output_level(PIN_PA13, FALSE);		
		HardDelay();
		pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
		port_pin_set_config(PIN_PA13, &pin_conf);
		port_pin_set_output_level(PIN_PA13, TRUE);		
	}


	//-------LEDS off
	//  
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_BIGREEN_PIN, &pin_conf);
	port_pin_set_output_level(LED_BIGREEN_PIN, true);
	port_pin_set_config(LED_BIRED_PIN, &pin_conf);
	port_pin_set_output_level(LED_BIRED_PIN, true);	
	ledBiGreen = 1;
	ledBiRed = 1; 
	port_pin_set_config(LED_BLUE_PIN, &pin_conf);
	port_pin_set_output_level(LED_BLUE_PIN, true);
	port_pin_set_config(LED_RED_PIN, &pin_conf);
	port_pin_set_output_level(LED_RED_PIN, true);	
	 
	ledRed = 2; 

	//----------------------------
	// motor pin initialization - outputs
	// PA16 PWMIN
	// PA18 EDa
	// PA19 INa
	// PB24 EDb
	// PB25 INb 
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	//---------- EDb
	port_pin_set_config(ENb, &pin_conf);
	port_pin_set_output_level(ENb, false);
	//--------- INb
	port_pin_set_config(INb, &pin_conf);
	port_pin_set_output_level(INb, false);	
	//----------EDa
	port_pin_set_config(ENa, &pin_conf);
	port_pin_set_output_level(ENa, false);
	//---------INa
	port_pin_set_config(INa, &pin_conf);
	port_pin_set_output_level(INa, false);
	//-------- PWmin
	port_pin_set_config(PIN_PA16, &pin_conf);
	port_pin_set_output_level(PIN_PA16, true); //false);	
	//-----------------------------
	// MOTOR PIN INITIALIZATION - INPUTS
	// PA3 CS - CURRENT SENSE
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(PIN_PA03, &pin_conf);
 
	
	//-------MOTOR OFF 
	// ENa,ENb, INa, INb all 0 
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(ENa, &pin_conf);	
	port_pin_set_output_level(ENa, false);
	port_pin_set_config(ENb, &pin_conf);	
	port_pin_set_output_level(ENb, false);
	port_pin_set_config(INa, &pin_conf);		
	port_pin_set_output_level(INa, false);
	port_pin_set_config(INb, &pin_conf);		
	port_pin_set_output_level(INb, false);	
 
	//-----------motor inputs
	// FLIMIT - 
	// HLIMIT
	// ENCODER 
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(IRLEDEN, &pin_conf);	
	port_pin_set_output_level(IRLEDEN, true);	
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(FLIMIT, &pin_conf);
	port_pin_set_config(HLIMIT, &pin_conf);
	port_pin_set_config(ENCODER, &pin_conf);		 
 

 
	MotorInit(); 
 
	//-------SUPER CAP enable
	// 1 is on
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(SUPERCAPEN, &pin_conf);	
	port_pin_set_output_level(SUPERCAPEN, false);  
	supercapState = true; 
	
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(PIN_PA02, &pin_conf);
	port_pin_set_config(PIN_PA28, &pin_conf);
	port_pin_set_config(INPUT_BREAKAWAY_TIP, &pin_conf);
	port_pin_set_config(INPUT_BREAKAWAY_RING, &pin_conf);	
	
	//--------------------------
	// BUTTON INPUTS
	//--------------------------
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(BUTTON_SETUP, &pin_conf);
	port_pin_set_config(BUTTON_POWER, &pin_conf);
	//-------------------------
	// ADC
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(PIN_PA02, &pin_conf);
	port_pin_set_config(PIN_PA03, &pin_conf);
	port_pin_set_config(PIN_PB04, &pin_conf);
	port_pin_set_config(PIN_PB06, &pin_conf); 
	port_pin_set_config(PIN_PB07, &pin_conf); 	
	
	//--------------------------
	// BLUETOOTH INPUTS
	//--------------------------
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(BLUETOOTH_PROG, &pin_conf);
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
//	port_pin_set_config(BLUETOOTH_RX, &pin_conf);	
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(BLUETOOTH_RESET, &pin_conf);
	port_pin_set_output_level(BLUETOOTH_RESET, true);
//	port_pin_set_config(BLUETOOTH_TX, &pin_conf);
		

	 
#endif	

#if REMOTEBOARD  
	//--------------------------------
	// Set all LED pins as outputs
	// PB13,PB14,PB15
	// ** turn them off as initial setting
	//--------------------------------
  //----------PB13 RED LED
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_RED_PIN, &pin_conf);
	port_pin_set_output_level(LED_RED_PIN, LED_RED_INACTIVE);
	port_pin_set_output_level(LED_RED_PIN, LED_RED_ACTIVE);
	port_pin_set_output_level(LED_RED_PIN, LED_RED_INACTIVE);		
  //----------PB14 BLUE LED
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_BLUE_PIN, &pin_conf);
	port_pin_set_output_level(LED_BLUE_PIN, LED_BLUE_INACTIVE);	
  //----------PB15 RED LED
    pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(LED_GREEN_PIN, &pin_conf);
    port_pin_set_output_level(LED_GREEN_PIN, LED_GREEN_INACTIVE);
    //----------PA03 BACKLIGHT
   pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
   port_pin_set_config(BL_KEY_PIN, &pin_conf);
   port_pin_set_output_level(BL_KEY_PIN, BL_KEY_INACTIVE); 
//BETH   port_pin_set_output_level(BL_KEY_PIN, BL_KEY_ACTIVE); 
//   port_pin_set_output_level(BL_KEY_PIN, BL_KEY_INACTIVE); 
//   port_pin_set_output_level(BL_KEY_PIN, BL_KEY_ACTIVE);         
//   port_pin_set_output_level(BL_KEY_PIN, BL_KEY_INACTIVE); 
	//--------------------------
	// BUTTON INPUTS 
	//--------------------------
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(BUTTON_SW1, &pin_conf);	
	port_pin_set_config(BUTTON_SW2, &pin_conf);
	port_pin_set_config(BUTTON_SW3, &pin_conf);
	port_pin_set_config(BUTTON_SW4, &pin_conf);
	
	
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_NONE;
	port_pin_set_config(PIN_PA02, &pin_conf);
	port_pin_set_config(PIN_PB11, &pin_conf);
	port_pin_set_config(PIN_PA28, &pin_conf);
	port_pin_set_config(PIN_PB22, &pin_conf); 
	port_pin_set_config(PIN_PB23, &pin_conf); 	
#endif	

	//----------------------------
	// FCC PINS 
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(CONSTANTTX, &pin_conf);
	port_pin_set_config(CONSTANTRX, &pin_conf);	 
	port_pin_set_config(CONSTANTCW, &pin_conf);
	//---------------------
	// SET FCC stuff up
	setTXContinuous = 0;
	setCW = 0;
	setRXContinuous = 0;	
	for (i=0;i<10;i++)
	{
		FCCSample();
	}
	if ((constantTX_pressed!=0)&&(constantRX_pressed==0)&&(constantCW_pressed==0))
	{
		setTXContinuous = 1; 
	}
	if ((constantTX_pressed==0)&&(constantRX_pressed!=0)&&(constantCW_pressed==0))
	{
		setRXContinuous = 1; 
	}
	if ((constantTX_pressed==0)&&(constantRX_pressed==0)&&(constantCW_pressed!=0))
	{
		setCW = 1; 
	}		
	//--------------------------------
	// speaker PB16 and PB17
	//--------------------------------
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PB16, &pin_conf);
	port_pin_set_output_level(PIN_PB16, false);
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PB17, &pin_conf);
	port_pin_set_output_level(PIN_PB17, false);
				
	//---------------------
	// i2c setup for EEPROM - COMMON
	//---------------------
	I2Cmain();
	ConfigInit(); 
//ConfigSensorTableClear();	
	//-----------Accelerometer 
	// Brakeboard - is i2c
	// remoteboard is spi
#if BRAKEBOARD	
	AccelInit(); 
#endif

#if REMOTEBOARD
	SPIAMain();
	AccelInit(); 
#endif	
	//-------RF433--- pressure sensor radio 
	// initialization. 
	// 1. Does the SPI and all radio
	//	initialization required. 
	// 2. enables the external interrupt for processing events.
	//------------------------------------ 
//V00_80 removing TIRE PRESSURE	RF433Init();
	//----------------------
	// LCD setup
	// Initialize RST, A0, RW, E as output
	// Initialize ST7567
	//----------------------
#if REMOTEBOARD
	lcdPinInit();                             
    lcdInit();                                
	AppScreenInit(SCREEN_HOME);
#endif
#if BRAKEBOARD
 
#endif
	//----------Timer Initialization
	//		
	configure_tc();
	configure_tc_callbacks();

	//-----ADC--------------
	ADCInit();
	
	//---------------LORA/FSK radio 
	whichRadio = WHICHRADIO_LORA; 
	CommInit();

//	Speakermain();

#if REMOTEBOARD
	BacklightSet(table0.Item.BackLightOn);	
	BacklightSetHomeColor(table0.Item.ScreenColor);
#endif

	system_interrupt_enable_global();
	Enable_global_interrupt();
	
	ADCStart(); 

#if BRAKEBOARD
	BrakeInit();
	UsartMain();
	USARTDisable();
	BluetoothSleep();
//	BluetoothMicrochipConfig();
//	BluetoothWakeUp();	
	 
	nvm_get_config_defaults(&config);
	nvm_set_config(&config);
#if 0 
	 for (i=0;i<200;i++)
	 {
		 general_buffer[i] = 0x23;
	 }
	 nvm_execute_command(NVM_COMMAND_UNLOCK_REGION,
	 APP_SCRATCH_BASE, 0);
	 do
	 {
		 error_code = nvm_erase_row(
		 //				100 * NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE);
		 APP_SCRATCH_BASE);
	 } while (error_code == STATUS_BUSY);
	 do
	 {
		 error_code = nvm_write_buffer(
		 //				(100 * NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE)+ NVMCTRL_PAGE_SIZE,
		 (APP_SCRATCH_BASE),
		 general_buffer, NVMCTRL_PAGE_SIZE);
	 } while (error_code == STATUS_BUSY);
	 do
	 {
		 error_code = nvm_read_buffer(
		 //				100 * NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE,
		 APP_SCRATCH_BASE,
		 general_buffer, NVMCTRL_PAGE_SIZE);
	 } while (error_code == STATUS_BUSY);
#endif	
#endif		

	button = ButtonChanged(); 
	configure_wdt();
    while (1) 
    {
//		wdt_reset_count();

		schedDone = 0;
		schedCount = 0;
		while ((schedCount <16) && (schedDone ==0))
		{
			mainLineTask = 0; 
			if ((schedByte & (1<<schedCount))!= 0)
			{
				//-------------------
				// disable interrupt
				//-------------------
				Disable_global_interrupt();			
				schedByte &= (~(1<<schedCount));			
				Enable_global_interrupt();
#if REMOTEBOARD					
				SwTaskList[schedCount]();
/*				
				button = ButtonChanged();
					if (((button & KEY_RIGHT)!= 0)&&(right_pressed != 0))
					{
						 SpeakerOn();
					}				
*/				
#endif			
#if BRAKEBOARD				
				if (poweredUp != 0)
				{
					SwTaskList[schedCount]();
				}
				else
				{
					if ((schedCount ==14))
					{
						DownloadDoneTask();
					}
					if ((schedCount == 13)||(schedCount==10))
					{
					//v 01_10
						if (newRemoteDownloadNeeded == TRUE)
						{
							SwTaskList[schedCount]();	
						}
					}
					if (schedCount ==6)
					{
						BrakeBoardStateMachineTask();
					}					
					if (schedCount ==3)
					{
						ButtonSample();
					}
					//------------------------
					// if not powered up and 
					// setup is pressed, will RESET the 
					// bluetooth to wake it up.
					//------------------------
					button = ButtonChanged();
					if (((button & KEY_SETUP)!= 0)&&(setup_pressed != 0))
					{
						bluetoothHoldTimer100msec = 30; 
					}
					if (setup_pressed != 0)
					{
						if (bluetoothHoldTimer100msec == 1)
						{
							bluetoothHoldTimer100msec = 0;
							if(bluetoothAwake != 0)
							{
								USARTDisable();
								BluetoothSleep();
							}
							else
							{
								USARTEnable();
								BluetoothWakeUp();
							}
						}
					}
					if (setup_pressed ==0)
					{
						bluetoothHoldTimer100msec = 0;
					}

				}
/*				
				if (((button & KEY_POWER)!= 0)&&(power_pressed != 0))				
 				{
						if (poweredUp != 0)
						{
							poweredUp = 0;
							brakeState = BRAKESTATE_POWERINGUP;
							BrakeBoardStateMachineTask();							
						}
						else
						{
							deconfigure_wdt();
							NVIC_SystemReset();
//							poweredUp = 1; 
//							brakeState = BRAKESTATE_RESET;
//							BrakeBoardStateMachineTask();							
						}
				
				}
				prevSW4 = newSW4;
*/				
#endif	
			}
			schedCount++;
		}
 
#if REMOTEBOARD		

		
#endif 
 
		
    }
	return 0;
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//whichRadio = WHICHRADIO_LORA;  hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh will need to add back if comm loss
//==============================================================================
void CommSupTask(void)
{
 	
	if ((setCW == 0)&&(setTXContinuous==0)&&(setRXContinuous==0))
	{
		commErrorCount++;
		if (commErrorCount > 20)
		{
			commErrorCount = 0; 
			brakeStatus.BrakeState |= BRAKESTATE_COMMERROR; 
	#if REMOTEBOARD		
			eventMessageReceived = 1; 
			AppScreenUpdateHome();	
	#endif	
			CommInit();	
			SX1272Init(whichRadio);
	#if BRAKEBOARD	
		
			if (whichRadio == WHICHRADIO_LORA)
			{					 
				AppLoraReceiveStart();
			}
			else
			{
				AppFskReceiveStart();
			}
			commSupTimer = COMM_SUP_TIME; 
	#endif
			commFailureCount++;
			if (commFailureCount>30)
			{
				commFailureCount =  0;
	#if REMOTEBOARD		
				if (whichRadio == WHICHRADIO_LORA)
				{
					AppLoraReceiveStart();
				}
				else
				{
					AppFskReceiveStart();
				}
	#endif			 
			}	
		}	
		else
		{
	#if BRAKEBOARD	
			commSupTimer = COMM_SUP_TIME; 
	#endif		
		}
	}
}

#if REMOTEBOARD
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
uint8_t switchRadioOver = FALSE; 
uint8_t switchTo; 
void AppHandleRadioSwitch(uint8_t btactive)
{
	//---------------------
	// brake board is indicating bluetooth session
	// 1. make sure not in a braking condition. 
	// 2. if not, set up to move to FSK. 
	//---------------------	
	if (btactive == 0x55)
	{
		if (whichRadio != WHICHRADIO_FSK)
		{
			switchRadioOver = TRUE; 
			switchTo = WHICHRADIO_FSK;
		}
	}
	else
	{
		if (whichRadio != WHICHRADIO_LORA)
		{
			switchRadioOver = TRUE; 
			switchTo = WHICHRADIO_LORA;
		}
	}	
	
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
uint8_t SwitchRadio(uint8_t *whichWay)
{
	uint8_t status; 
	status = FALSE; 
	if (switchRadioOver == TRUE)
	{
		status = TRUE; 
		*whichWay = switchTo;
	}
	switchRadioOver = FALSE; 
	return status; 
}
#endif 

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void CommInit(void)
{
	struct port_config pin_conf;
	if ((setCW != 0))
	{
		whichRadio = WHICHRADIO_FSK;
	}
		
//	whichRadio = WHICHRADIO_FSK;  //TESTTINTGGGGGT
	port_get_config_defaults(&pin_conf);
	//----------------------
	// LoRA radio
	//-----------------------
	//--------------------------------
	// X-RESET = output = PA3
	// X-NSSS = output = PA6
	// X-MOSI = output = PA4
	// X-MISO = input = PA7
	// X-SCK = output = PA5
	// X-DIO0 = input ? = PB9
	//--------------------------------
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LORA_XRESET, &pin_conf);
	port_pin_set_output_level(LORA_XRESET, 0);

	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	port_pin_set_config(LORA_XDIO0, &pin_conf);
	port_pin_set_config(LORA_XDIO1, &pin_conf);
	SPIXConfigure();
	SPIXMain();

	SX1272Init(whichRadio); 
	if ((setCW == 0))	
	{
		if (whichRadio == WHICHRADIO_LORA)
		{
			AppLoraReceiveStart();
		}
		else
		{
			AppFskReceiveStart();
		}
	}
	if (setTXContinuous!=0)
	{	
		SendOneMessage();
	}
#if REMOTEBOARD
	commSupTimer = 0; 
	commErrorCount = 2; 	
#endif
#if BRAKEBOARD
	commSupTimer = COMM_SUP_TIME; 
	commErrorCount = 0; 	
#endif	

}





//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppStatusInitialization(void)
{
	//-----------------------
	// Tire radio status 
	// bit 0 = init of the tire radio has been done
	// bit 1 = successful comm with the RF433 chip 
	// bit 2 = received a packet via the RF433 chip 
	statusData.TireRadio = 0; 
	//----------------------
	statusData.Accelerometer = 0;
	statusData.EEPROM = 0;
	statusData.ExtRadio = 0;

	statusBrake.TireRadio = 0;
	//----------------------
	statusBrake.Accelerometer = 0;
	statusBrake.EEPROM = 0;
	statusBrake.ExtRadio = 0;	
	//----------------brake status 
	brakeStatus.VoltageInput = 0;
	brakeStatus.AccelerometerStatus = 0;
	brakeStatus.ActuatorStatus = 0; 
	brakeStatus.BrakeState = 0;
	brakeStatus.VoltageSupercap = 0; 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void AppStatusUpdate(uint8_t whichInterface,uint8_t statusUpdate,uint8_t good)
{
	switch (whichInterface)
	{
		case INTERFACE_TIRERADIO:
		{
			//-----------------------
			// Tire radio status
			// bit 0 = init of the tire radio has been done
			// bit 1 = successful comm with the RF433 chip
			// bit 2 = received a packet via the RF433 chip
			if (good != 0)
			{
				statusData.TireRadio |= statusUpdate; 
			}		
			else
			{
				statusData.TireRadio &= ~statusUpdate; 
			}
			break;
		}
		case INTERFACE_EEPROM:
		{
			//-----------------------
			// Tire radio status
			// bit 0 = init of the tire radio has been done
			// bit 1 = successful comm with the RF433 chip
			// bit 2 = received a packet via the RF433 chip
			if (good != 0)
			{
				statusData.EEPROM |= statusUpdate; 
			}		
			else
			{
				statusData.EEPROM &= ~statusUpdate; 
			}
			break;
		}	
		case INTERFACE_ACCELEROMETER:
		{
			//-----------------------
			// Tire radio status
			// bit 0 = init of the tire radio has been done
			// bit 1 = successful comm with the RF433 chip
			// bit 2 = received a packet via the RF433 chip
			if (good != 0)
			{
				statusData.Accelerometer |= statusUpdate; 
			}		
			else
			{
				statusData.Accelerometer &= ~statusUpdate; 
			}
			break;
		}					
	}
}

struct tc_module tc_instance;

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
uint8_t minute=0;
uint8_t toggle; 
void tc_callback_to_toggle_led(struct tc_module *const module_inst)
{
	wdt_reset_count();
//--------------------------
// 1 MSEC tasks.
//--------------------------
	// * accelerometer. 
#if REMOTEBOARD
	timerAccelerometer++;
	if (timerAccelerometer >= 1000)
	{
		timerAccelerometer = 0;
		schedByte |= SCHEDBYTE_ACCELEROMETER;
	}	
#else
	schedByte |= SCHEDBYTE_ACCELEROMETER;
#endif
#if BRAKEBOARD 
	if (blockingTime >0)
	{
		blockingTime--;
	}
#endif
//-------------------
// 25 msec tasks
//-------------------	
	twentyfiveMSec++;
	if (twentyfiveMSec >= 25)
	{
#if BRAKEBOARD		
		schedByte|= SCHEDBYTE_BRAKETASK; 
		brakeChange |= BRAKECHANGE_TABLESAMPLE; 
		
		if (breakawayHoldTimer > 0)
		{
			breakawayHoldTimer++;
		}
#endif		

		twentyfiveMSec = 0;
	}
#if BRAKEBOARD			 
	if (motorOn == TRUE)
	{
		schedByte |= SCHEDBYTE_BRAKETASK;
	}
#endif		
#if BRAKEBOARD	
	ditherTimer++;
	if (ditherTimer >= DITHER_TIME)
	{	
		ditherTimer = 0;		
		schedByte|= SCHEDBYTE_BRAKETASK; 
		brakeChange |= BRAKECHANGE_DITHER; 
	}
#endif 
	
	hundredMSec++;
	if (hundredMSec >= 100)
	{
#if BRAKEBOARD 
		if ((brakeState != BRAKESTATE_RESET)&&(programming ==0))
		{	
			mainLineTask++;
		}
#else
		if ((programming ==0))
		{
			mainLineTask++;
		}
#endif 		
		if (mainLineTask>10)
		{
			while (1)
			{
				
			}
		}
#if REMOTEBOARD
		schedByte |= SCHEDBYTE_SCREENREFRESH;
		downloadTimer++;
		if (downloadTimer >10)
		{
			downloadTimer = 0;
			downloadTime = TRUE; 
		}
		
#endif
		hundredMSec = 0;
#if BRAKEBOARD	
		if (bluetoothHoldTimer100msec > 1)
		{
			bluetoothHoldTimer100msec--;
		}			
		if (voltageBadTime < VOLTAGE_BAD_TIME)
		{
			voltageBadTime++;
		}
		if (needNewBaselineTimer >0)
		{
			needNewBaselineTimer--;
			needNewBaseline = 1; 
		}
		else
		{
			needNewBaseline = 0;
		}
#endif		
	}
	timerSecond++;
	if (timerSecond >= 1000)
	{
		timerSecond = 0;
		minute++;
		if (minute >= 60)
		{
			minute = 0;
		}
//		port_pin_toggle_output_level(BL_KEY_PIN);
#if BRAKEBOARD
		if (brakeSupTime >0)
		{
			brakeSupTime--;
			if (brakeSupTime ==0)
			{
				brakeChange |= BRAKECHANGE_SUPTIME; 
			}
		}

#endif


		//-----------------one second timer 
		timerRF433++;
		if (timerRF433>= 30)
		{
			timerRF433 = 0;
			schedByte |= SCHEDBYTE_RF433;
		}

	}
	TickCounter++;
	if (TickCounter > 20)
	{
		TickCounter = 0;
		schedByte |= SCHEDBYTE_BUTTON;
	}
	if (wdog >0)
	{
		wdog--;
	}


#if REMOTEBOARD	
	timerBrakeComm++;
	if (timerBrakeComm >= 200)
	{
		timerBrakeComm = 0;
		schedByte |= SCHEDBYTE_COMMTOBRAKE;
	}	
#endif	

#if BRAKEBOARD
	if (brakeBiLED == BRAKEBILED_GREENFLICKER)
	{
		flickTimer++;
		if (flickTimer >= FLICKERTIME)
		{
			flickTimer = 0;
			port_pin_set_output_level(LED_BIGREEN_PIN, false);
			port_pin_set_output_level(LED_BIRED_PIN, true);			
		}
		else
		{
			if (flickTimer >= FLICKOFF)
			{
				port_pin_set_output_level(LED_BIGREEN_PIN, true);
				port_pin_set_output_level(LED_BIRED_PIN, true);			
			}
		}
	}	
	if (brakeBiLED == BRAKEBILED_YELLOWFLICKER)
	{
		flickTimer++;
		if (flickTimer >= FLICKERTIME)
		{
			flickTimer = 0;
			port_pin_set_output_level(LED_BIGREEN_PIN, false);
			port_pin_set_output_level(LED_BIRED_PIN, false);			
		}
		else
		{
			if (flickTimer >= FLICKOFF)
			{
				port_pin_set_output_level(LED_BIGREEN_PIN, true);
				port_pin_set_output_level(LED_BIRED_PIN, true);			
			}
		}
	}		
#endif	
#if BRAKEBOARD
	if (brakeBiLED == BRAKEBILED_GREENSTROBE)
	{
		strobeTimer++;
		if (strobeTimer >= STROBETIME)
		{
			strobeTimer = 0;
			port_pin_set_output_level(LED_BIGREEN_PIN, false);
			port_pin_set_output_level(LED_BIRED_PIN, true);
		}
		else
		{
			if (strobeTimer >= STROBEOFF)
			{
				port_pin_set_output_level(LED_BIGREEN_PIN, true);
				port_pin_set_output_level(LED_BIRED_PIN, true);
			}
		}
	}
#endif
	blinkTimer++;
	if (blinkTimer >= BLINKTIME)
	{
		blinkTimer = 0;
#if BRAKEBOARD
		if ((brakeBlueLED == BRAKEBLUELED_ALTGREEN)||(brakeBlueLED == BRAKEBLUELED_ALTYELLOW))
		{
			if (ledBlue == 0)
			{
				ledBlue = 1; 
				port_pin_set_output_level(LED_BLUE_PIN, true);
				if (brakeBlueLED == BRAKEBLUELED_ALTGREEN)
				{
					port_pin_set_output_level(LED_BIGREEN_PIN, false);
				}
				else
				{
					port_pin_set_output_level(LED_BIGREEN_PIN, false);
					port_pin_set_output_level(LED_BIRED_PIN, false);
				}
			}	
			else
			{
				ledBlue = 0;
				port_pin_set_output_level(LED_BLUE_PIN, false);
				port_pin_set_output_level(LED_BIGREEN_PIN, true);
				port_pin_set_output_level(LED_BIRED_PIN, true);
			}
		}
		else
		{
			switch (brakeBiLED)
			{
				case BRAKEBILED_REDFLASH:
				{
					port_pin_set_output_level(LED_BIGREEN_PIN, true);
					if (ledBiRed != 0)
					{
						port_pin_set_output_level(LED_BIRED_PIN, false);
						ledBiRed = 0;
					}
					else
					{
						port_pin_set_output_level(LED_BIRED_PIN, true);
						ledBiRed = 1;
					}	
					break;
				}
				case BRAKEBILED_YELLOWSOLID:
				{
		 			port_pin_set_output_level(LED_BIRED_PIN, false);
					port_pin_set_output_level(LED_BIGREEN_PIN, false);
					ledBiRed = 0;
					ledBiGreen = 0;
					break;
				}	
				case BRAKEBILED_YELLOWFLASH:
				{
					if (ledBiRed != 0)
					{
						port_pin_set_output_level(LED_BIRED_PIN, false);
						port_pin_set_output_level(LED_BIGREEN_PIN, false);
						ledBiRed = 0;
						ledBiGreen = 0;
					}
					else
					{
						port_pin_set_output_level(LED_BIRED_PIN, true);
						port_pin_set_output_level(LED_BIGREEN_PIN, true);
						ledBiRed = 1;
						ledBiGreen = 1;
					}
					break;
				}													
				case BRAKEBILED_OFF:
				{
					port_pin_set_output_level(LED_BIGREEN_PIN, true);
					port_pin_set_output_level(LED_BIRED_PIN, true);
					ledBiGreen = 1;
					ledBiRed = 1;
					break;
				}		
				case BRAKEBILED_GREENSOLID:
				{
					port_pin_set_output_level(LED_BIGREEN_PIN, false);
					port_pin_set_output_level(LED_BIRED_PIN, true);
					ledBiGreen = 0;
					ledBiRed = 1;
					break;
				}				
			}
			switch (brakeBlueLED)
			{
				case BRAKEBLUELED_OFF:
				{
					port_pin_set_output_level(LED_BLUE_PIN, true);
					break;
				}
				case BRAKEBLUELED_SOLID:
				{
					port_pin_set_output_level(LED_BLUE_PIN, false);
					break;
				}
			}	
			switch (brakeRedLED)
			{
				case BRAKEREDLED_OFF:
				{
					port_pin_set_output_level(LED_RED_PIN, true);
					break;
				}
				case BRAKEREDLED_SOLID:
				{
					port_pin_set_output_level(LED_RED_PIN, false);
					break;
				}
			}			
		}
#endif	
	}
	if (adcTimer >0)
	{
		adcTimer--;
		if (adcTimer == 0)
		{
			adcTimeout = 1; 
			schedByte |= SCHEDBYTE_ADC; 
		}
	}
		if (commSupTimer>0)
		{
			commSupTimer--;
			if (commSupTimer == 0)
			{
				schedByte |= SCHEDBYTE_COMMSUP; 	
			}
		}	
#if BRAKEBOARD		
	if (motorRunTime >0)
	{
		motorRunTime--;
	}		
#endif		
}


 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void configure_tc(void)
{
	struct tc_config config_tc;
 
	tc_get_config_defaults(&config_tc);
 
	config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc.clock_source = GCLK_GENERATOR_1;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;
	config_tc.counter_8_bit.period = 8;  //9;
//	config_tc.counter_16_bit.value = 200;
//	config_tc.counter_8_bit.compare_capture_channel[0] = 50;
//	config_tc.counter_8_bit.compare_capture_channel[1] = 54;
	 
	tc_init(&tc_instance, TC3, &config_tc);
	tc_enable(&tc_instance);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void configure_tc_callbacks(void)
{
	tc_register_callback(&tc_instance, tc_callback_to_toggle_led,
	TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//============================================================================== 
#if REMOTEBOARD
 void BacklightSet(uint8_t state)
{
	if (state == TRUE)
	{
		port_pin_set_output_level(BL_KEY_PIN, BL_KEY_ACTIVE); 		
	}	
	else
	{
		port_pin_set_output_level(BL_KEY_PIN, BL_KEY_INACTIVE); 		
	}
}
#endif

 