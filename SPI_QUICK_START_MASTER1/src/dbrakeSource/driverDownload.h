//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: DRIVERDOWNLOAD.H
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#ifndef __DRIVERDOWNLOAD_H__
#define __DRIVERDOWNLOAD_H__

#include "driverUSART.h"
 
//#if BRAKEBOARD
#define APP_INFO_START				0x89ABCDEF
#define APP_INFO_END				0xFEDCBA98
#define MAX_SEARCH_OFFSET			512
 
//---------------------GLOBAL VARIABLES--------------------------
extern AppInfo newBrakeInfo;
extern AppInfo newRemoteInfo;
//---------------------GLOBAL PROTOTYPES--------------------------
void DownloadDoneTask(void);
void CheckScratch(void); 
//#endif
#endif
