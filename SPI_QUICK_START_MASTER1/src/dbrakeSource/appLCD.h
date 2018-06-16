//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: appLCD.h
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor: STM32F103R
// TOOLS: IAR Workbench
// DATE:
// CONTENTS: This file contains
//------------------------------------------------------------------------------
// HISTORY: This file
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#ifndef APP_LCD_H
#define APP_LCD_H
 
//---------------------GLOBAL DEFINITIONS-----------------------------------

	//---------------appScreen
#define SCREEN_IDLE		0
#define SCREEN_HOME		1
#define SCREEN_MAIN		2
#define SCREEN_SYSTEM	3
#define SCREEN_FORCE	4
#define SCREEN_COACH	5
#define SCREEN_TOWCAR	6
#define SCREEN_STATUS_ROOT 7
#define SCREEN_TIRE_REMOTE1 8
#define SCREEN_TIRE_REMOTE2 9
#define SCREEN_BRAKE_STATUS 10
#define SCREEN_TIRE_BRAKE1 11
#define SCREEN_TIRE_BRAKE2 12
#define SCREEN_CONFIG		13
#define SCREEN_SENSOR		14
#define SCREEN_MISC			15
#define SCREEN_STATUS_SKELETON		16
#define SCREEN_SETUPNEW		17
#define SCREEN_SETUPPAIR	18
#define	SCREEN_SETUPHYBRID 19
#define SCREEN_SETUPMAXFORCE	20
#define SCREEN_SETUPRESET		21
#define SCREEN_SETUPBRAKEBACKLIGHT  22
#define SCREEN_DOWNLOADING		23
#define SCREEN_DOWNLOADREQUEST  24
#define SCREEN_SETUPSENSITIVITY		25
#define SCREEN_INFO				26

	//------------------appScreenHomeType---------------
#define SCREEN_HOME_BASE			0
#define SCREEN_HOME_COMMERROR		1
#define SCREEN_HOME_INPUTVOLTAGEBAD	2
#define SCREEN_HOME_NOTSETUP		3
#define SCREEN_HOME_BREAKAWAYTIP	4
#define SCREEN_HOME_MANUALBRAKE		5
#define SCREEN_HOME_BREAKAWAYTIP_KEYSCLEAR	6

//---------------------GLOBAL VARIABLES-----------------------------------
extern uint8_t appScreen;
extern uint8_t appScreenHomeType;  

extern const uint8_t ScreenDownloadRequest[1024];
extern const uint8_t ScreenDownloading[1024];
extern const uint8_t ScreenSetupMaxForce[1024];
extern const uint8_t ScreenSetupHybrid[1024];
extern const uint8_t ScreenSetupPair[1024];
extern const uint8_t ScreenSetupResetSettings[1024];
extern const uint8_t ScreenSetupSensitivitySettings[1024];
extern const uint8_t ScreenInfoNew[1024];
//---------------------GLOBAL FUNCTION PROTOTYPES--------------------------
extern void AppScreenInit(uint8_t which); 
extern void AppScreenProcessKeyChange(void); 
extern void AppScreenCarRadioReadingIn(uint8_t which,uint8_t increment);
extern void AppScreenFSKReadingIn(uint8_t *buffer,uint8_t myoffset);
extern void AppScreenUpdateHome(void);
extern void AppScreenPlacePressure(void);
extern void PlaceDownloadPacket(uint16_t packetnumber);
#endif