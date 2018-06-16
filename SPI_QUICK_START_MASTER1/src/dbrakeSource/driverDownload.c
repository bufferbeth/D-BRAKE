//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE:  
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor:  
// TOOLS:  
// DATE:
// CONTENTS: This file contains  
//------------------------------------------------------------------------------
// HISTORY: This file  
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#include <asf.h>
#include "dbrakeDefs.h"
#include "appBluetooth.h" 
#include "driverUSART.h"
#include "driverProgramming.h"
#include "driverDownload.h"
#include "appProtocol.h"


 
//---------------------LOCAL VARIABLES------------------------------------
 
 
//---------------------LOCAL FUNCTION PROTOTYPES--------------------------   
uint8_t GetAppInfo(uint32_t startAddress, AppInfo* info);

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: GetAppInfo
//------------------------------------------------------------------------------
// This function parses app info starting at the given addr (failure returns 0)
//==============================================================================
uint8_t GetAppInfo(uint32_t startAddress, AppInfo* info)
{
	uint8_t success;
	uint8_t appInfoStarted = 0;
	
	success = 0;
	//search starting at the given address for the markers that designate the app info section
	for (uint32_t addr = startAddress; addr < startAddress + MAX_SEARCH_OFFSET; addr += 4)
	{
		if (*((uint32_t*)addr) == APP_INFO_START)
		{
			info->checksum = *(uint32_t*)(addr + 4);
			info->appLength = *(uint32_t*)(addr + 8);
			info->version = *(uint32_t*)(addr + 12);
			info->filetype1 = *(uint32_t*)(addr + 32);
			info->filetype2 = *(uint32_t*)(addr + 36);
			appInfoStarted = 1;
			addr += 16;
		}
		else if (appInfoStarted)
		{
			if (*((uint32_t*)addr) == APP_INFO_END)
			{
				//save address to start checksum at
				info->checksumStartOffset = addr + 4 - startAddress;
				
				//success
				success = 1;
			}
			//else we should have gotten an end marker by now...
		}
		//else we haven't found the app info start marker yet
	}
	
	//we didn't find both a start and end marker
	return success;
} 
#if BRAKEBOARD 

void CheckScratch(void)
{
	uint32_t ltemp,ltemp2; 
	
		if ((GetAppInfo(APP_SCRATCH_BASE, &newRemoteInfo)!= 0)&&(newRemoteInfo.filetype1==0x35))
		{
			newRemoteDownloadNeeded = TRUE; 
		}			
}


 
AppInfo newBrakeInfo;
AppInfo newRemoteInfo;
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function
//============================================================================== 
void DownloadDoneTask(void)
{
	//-------------------------------
	// check integrity of the download 
	// check if file is for Brake board or Remote board 
	//typedef struct
	//{
	//	uint32_t checksum;
	//	uint32_t appLength;
	//	uint32_t checksumStartOffset;
	//	uint32_t version;
	//}AppInfo;
	//
	// extern AppInfo brakeApp;
	// extern AppInfo remoteApp;	
	if ((GetAppInfo(APP_SCRATCH_BASE, &newBrakeInfo)!= 0)&&(newBrakeInfo.filetype1==0x34))
	{
		//---------------BRAKE DOWNLOAD .... 
		// Force a reset. 
		//----------------------------------
		/* Reset module and boot into application */
		NVIC_SystemReset();
	}	
	else
	{
		if ((GetAppInfo(APP_SCRATCH_BASE, &newRemoteInfo)!= 0)&&(newRemoteInfo.filetype1==0x35))
		{
			//---------------REMOTE DOWNLOAD .... 
			// Download to remote
			//----------------------------------
			newRemoteDownloadNeeded = TRUE; 
			//01_10
			whichRadio = WHICHRADIO_LORA;
			switchToFSK = FALSE;
			CommInit();			
		}			
	}
 
}
#endif

#if REMOTEBOARD

AppInfo newBrakeInfo;
AppInfo newRemoteInfo;
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function
//============================================================================== 
void DownloadDoneTask(void)
{
	//-------------------------------
	// check integrity of the download 
	// check if file is for Brake board or Remote board 
	//typedef struct
	//{
	//	uint32_t checksum;
	//	uint32_t appLength;
	//	uint32_t checksumStartOffset;
	//	uint32_t version;
	//}AppInfo;
	//
	// extern AppInfo brakeApp;
	// extern AppInfo remoteApp;	
	if ((GetAppInfo(APP_SCRATCH_BASE, &newRemoteInfo)!= 0)&&(newRemoteInfo.filetype1==0x35))
	{
		//---------------BRAKE DOWNLOAD .... 
		// Force a reset. 
		//----------------------------------
		/* Reset module and boot into application */
		NVIC_SystemReset();
	}	 
}
#endif 
 


