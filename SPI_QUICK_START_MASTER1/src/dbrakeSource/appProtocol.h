//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: appProtocol.H
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#ifndef __APPPROTOCOL_H__
#define __APPPROTOCOL_H__

#include "driverTSPI.h"
#include "driverUSART.h"
//---------------------GLOBAL DEFINITIONS--------------------------
//#define MSG_SR	0x534D  //was SR now SM 
#define MSG_PR	0X504D	//was PR now PM
#define MSG_FV	0x464D	//was FV now FM 
#define MSG_STATUS	0x424D	//was BS now BM
#define MSG_SW	0x5357  //SW 

//-------------------------
// BYTE 5 of the remote 
// gives some key/commands from the remote to the brake 
#define REMOTE_MANUALBRAKE_ACTIVE	0x01
#define REMOTE_CLEARBREAKAWAY		0x02
extern uint8_t remoteStatus; 
extern uint8_t remoteForce; 
#define REMOTE_ACTIVEBRAKEENABLE			0x01
#define REMOTE_TPMSENABLE					0x02
extern uint8_t remoteSettings; 
extern uint16_t gPrime;

//---------------------GLOBAL VARIABLES--------------------------
extern uint8_t brakesEEPROMData[6];
extern uint16_t brakesAccelData[3];
extern uint8_t brakesTireData[MAXSENSORS][8]; 
extern uint8_t brakesVersionDate[10]; 
extern uint8_t remoteVersionToReport[4];
extern uint8_t switchToFSK;
extern uint8_t downloadTime;
#if REMOTEBOARD
AppInfo remoteDownloadApp;
#endif
#if BRAKEBOARD
extern uint8_t newRemoteDownloadNeeded;
#endif
//---------------------GLOBAL PROTOTYPES--------------------------
void AppProtocolBrake(uint8_t *buffers);
void AppProtocolRemote(uint8_t *buffer); 
void SendOneMessage(void);	 
#if REMOTEBOARD
void BrakeCommTask(void);
void DownloadStart(void);
#endif
#endif
