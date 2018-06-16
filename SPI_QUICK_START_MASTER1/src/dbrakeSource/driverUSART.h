//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: DRIVERUSART.H
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#ifndef __DRIVERUSART_H__
#define __DRIVERUSART_H__

 


typedef struct 
{
	uint32_t checksum;
	uint32_t appLength; 
	uint32_t checksumStartOffset; 
	uint32_t version;	
	uint32_t filetype1;
	uint32_t filetype2; 
}AppInfo;
#if BRAKEBOARD 
//---------------------GLOBAL VARIABLES--------------------------
extern AppInfo brakeApp;
extern AppInfo remoteApp; 
//---------------------GLOBAL PROTOTYPES--------------------------
void USARTDisable(void);
void USARTEnable(void);
 
#endif
#endif
