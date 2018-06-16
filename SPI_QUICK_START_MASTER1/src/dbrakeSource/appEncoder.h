//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: APPENCODER.H
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#ifndef __APPENCODER_H__
#define __APPENCODER_H__

 
#if BRAKEBOARD
 
//---------------------GLOBAL VARIABLES--------------------------
#define MAX_BUILDTABLE 1024
typedef struct
{
	uint16_t Current[MAX_BUILDTABLE];
	uint16_t EncoderCount[MAX_BUILDTABLE];
}BUILDTABLE;
extern BUILDTABLE buildTable;
extern BUILDTABLE encoderTable;
extern uint16_t buildTableOffset;
extern uint16_t encoderTableOffset;

extern uint16_t encoderCount;
extern uint8_t encoderFlip; 
extern uint16_t encoderFillOffset;
 
//---------------------GLOBAL PROTOTYPES--------------------------
uint16_t MotorFindEncoderMatch(uint16_t matchCurrent);
void ConfigureEncoder(void);
void ConfigureEncoderCallbacks(void);
void EncoderCallback(void); 
 
#endif
#endif
