//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: BUTTONS.H
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#ifndef __APPMOTOR_H__
#define __APPMOTOR_H__

extern uint8_t brakeState;
#define BRAKESTATE_RESET		0
#define BRAKESTATE_IDLESLEEP	1
#define BRAKESTATE_POWERINGUP	2
#define BRAKESTATE_PRESETUP		3
#define BRAKESTATE_WAITONSETUP	4
#define BRAKESTATE_SETUPACTIVE	5
#define BRAKESTATE_POWEREDUP	6
#define BRAKESTATE_ERROR_RETRACT 7
#define BRAKESTATE_ERROR		8
#define BRAKESTATE_ACTIVE		9
#define BRAKESTATE_ACTIVE_EXTEND		10
#define BRAKESTATE_HOLDOFF_ACTIVE		11
#define BRAKESTATE_ACTIVE_RETRACT		12
#define BRAKESTATE_ACTIVE_HOLD			13
#define BRAKESTATE_SETUPACTIVE_PAUSE_EXTEND 14
#define BRAKESTATE_SETUPACTIVE_PAUSE_RETRACT 15
#define BRAKESTATE_END_RETRACT				16
#define BRAKESTATE_ACTIVE_EXTEND_BREAKAWAY	17
#define BRAKESTATE_ACTIVE_HOLD_BREAKAWAY	18
#define BRAKESTATE_END_RETRACT_BREAKAWAY	19
#define BRAKESTATE_ACTIVE_EXTEND_MANUAL		20
#define BRAKESTATE_ACTIVE_HOLD_MANUAL		21
#define BRAKESTATE_END_RETRACT_MANUAL		22
#define BRAKESTATE_END_RETRACT_TIMEOUT	23
#define BRAKESTATE_ERROR_FINAL			24
#define BRAKESTATE_ERROR_VOLTAGE_ACTIVE 25
#define BRAKESTATE_ERROR_VOLTAGE_RESET	26


#if BRAKEBOARD 

#define MAX_ENCODERCOUNT_BACK 300

#define TIME_TOTAL_BRAKING_EVENT 100
//Voltage readings: 155 counts/volt
//Motor current reading: 205 counts/amp

	//----------------accelerometer definitions
#define ACC_ONE_G ((int16_t) 0x4000)
#define ACC_FOURTH_G ((int16_t)(ACC_ONE_G/4)) 
#define ACC_THREESIXTEENTHS_G ((int16_t)((ACC_ONE_G *3)/16))
#define ACC_EIGHTH_G ((int16_t)(ACC_ONE_G/8)) //2048d = 0x800h 834)   // was -834 ... making it postivie
#define ACC_SIXTEENTHS_G ((int16_t)(ACC_ONE_G/16))
#define ACC_TENTH_G ((int16_t)(ACC_ONE_G/10))
#define ACC_TWENTYITH_G ((int16_t)(ACC_ONE_G/20))
#define ACC_THRESHOLD_MULTIPLIER  ((int16_t)(ACC_ONE_G/200)) 
#define ACC_TWENTYFIFTH_G ((int16_t)(ACC_ONE_G/25))
#define NEG_ACC_THREESIXTEENTHS_G ((int16_t)(-1*ACC_THREESIXTEENTHS_G))
#define NEG_ACC_TENTH_G ((int16_t)(-1*ACC_TENTH_G))
#define ACC_HALF_G ((int16_t) (ACC_ONE_G/2))
#define ACC_POINT4_G ((int16_t)((ACC_ONE_G * 4)/10))
#define ACC_DITHER_TRIGGER_G ((int16_t)(ACC_ONE_G/10))  //1/40 = .025
 

	//-----------current threshold definitionS
#define AMPS_4POINT5	923
#define AMPS_3POINT75	769	
#define AMPS_2			410
#define AMPS_2POINT5	513
#define AMPS_2POINT75	564
#define AMPS_3			(3*205)
#define AMPS_4POINT082  837
#define AMPS_1			205

	#define CURRENT_THRESHOLD_SETUP		AMPS_4POINT5  //AMPS_3POINT75
	#define CURRENT_THRESHOLD_TABLEBUILD	AMPS_4POINT082  //AMPS_3
	#define CURRENT_THRESHOLD_EXTEND	AMPS_1   //AMPS_2POINT75
	#define CURRENT_THRESHOLD_RETRACT AMPS_2 
	#define CURRENT_FLUFF  20

//---------------------GLOBAL DEFINITIONS--------------------------
 extern uint8_t brakeBlueLED;
 #define BRAKEBLUELED_ALTYELLOW		0
 #define BRAKEBLUELED_SOLID			1
 #define BRAKEBLUELED_OFF			2
 #define BRAKEBLUELED_ALTGREEN		3
 
extern uint8_t brakeBiLED;
#define BRAKEBILED_GREENSOLID	0
#define BRAKEBILED_GREENFLICKER 1
#define BRAKEBILED_OFF			2
#define BRAKEBILED_REDFLASH		3
#define BRAKEBILED_YELLOWFLASH	4
#define BRAKEBILED_GREENSTROBE	5
#define BRAKEBILED_YELLOWSOLID	6
#define BRAKEBILED_YELLOWFLICKER 7

extern uint8_t  brakeRedLED;
#define BRAKEREDLED_OFF			1
#define BRAKEREDLED_SOLID		2
 
 
#define BRAKESTATE_RESET		0
#define BRAKESTATE_IDLE			1
#define BRAKESTATE_PRESLEEP		2
#define BRAKESTATE_PRESETUP		3
#define BRAKESTATE_SETUP		4
#define BRAKESTATE_SETUPACTIVE	5
#define BRAKESTATE_SLEEP		6
#define BRAKESTATE_SLEEPING		7 
//---------------------GLOBAL VARIABLES--------------------------
uint8_t needNewBaseline;
uint16_t needNewBaselineTimer;

extern uint16_t motorRunTime;
extern uint8_t motorOn; 
extern uint16_t motorx;
extern int16_t motorAccXBaseline;

extern uint16_t breakawayHoldTimer; 

	//---------------------------
	// based on a 100msec 
	//---------------------------
extern uint16_t voltageBadTime;
#define VOLTAGE_BAD_TIME	30 
 
#define DITHER_TIME 1000
extern uint16_t ditherTimer; 

extern uint16_t encoderCountBack;  

#define NONE 0
#define EXTENDING 1
#define RETRACTING 2
#define RETRACTING_BY_ENCODER	3
extern uint8_t action;
//---------------------GLOBAL PROTOTYPES--------------------------
void MotorOff(void);
void MotorCW(void);
void MotorCCW(void);
void MotorHLimitTask(void); 
void MotorFLimitTask(void);
void MotorInit(void);
void BrakeBoardStateMachineTask(void);
void BrakeInit(void);
void MotorBuildGetAcc(void);
#endif
#endif
