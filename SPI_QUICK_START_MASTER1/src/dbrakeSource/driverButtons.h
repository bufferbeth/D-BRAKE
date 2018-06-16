//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: BUTTONS.H
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#ifndef __BUTTONS_H__
#define __BUTTONS_H__

 

//---------------------GLOBAL DEFINITIONS--------------------------
#if REMOTEBOARD
 #define KEY_RIGHT 0x01
 #define KEY_LEFT  0x02
 #define KEY_CENTER 0x04
 #define KEY_BRAKE  0x08
#endif
#if BRAKEBOARD
	#define KEY_SETUP 0x01
	#define KEY_POWER 0x02
	#define KEY_BREAKAWAYRING 0x04
	#define KEY_BREAKAWAYTIP	0x08
#endif
//---------------------GLOBAL VARIABLES--------------------------

extern short int constantTX_pressed;
extern short int constantRX_pressed;
extern short int constantCW_pressed;
 
#if REMOTEBOARD 
extern short int right_pressed; 
extern short int center_pressed; 
extern short int left_pressed; 
extern short int brake_pressed; 
extern short int breakaway_pressed; 
#endif
#if BRAKEBOARD
extern short int setup_pressed;
extern short int power_pressed;
extern short int breakawayRing_pressed; 
extern short int breakawayTip_pressed; 

extern uint8_t flimitState;
extern uint8_t hlimitState;
#endif  
//---------------------GLOBAL PROTOTYPES--------------------------
void ButtonInit(void);
void ButtonSample(void);
uint8_t ButtonChanged(void);
void FCCSample(void);
#endif
