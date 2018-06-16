/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FILE:    arrayfile.c                                                      */
/*                                                                           */
/* PURPOSE: This example illustrates how to use the Formatting and IO        */
/*          functions ArrayToFile and fileToArray to write/read a data file. */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Include files                                                             */
/*---------------------------------------------------------------------------*/
#include "toolbox.h"
#include <ansi_c.h>
#include <cvirte.h>
#include <userint.h>
#include <formatio.h>																					  
#include <string.h>
#include <stdlib.h>
#include "arrayfile.h"
#include "micro tester menu.h"  
#include "micro tester main.h"  
/*---------------------------------------------------------------------------*/
/* Defines                                                                   */
/*---------------------------------------------------------------------------*/
#define COUNT3B 12*1024
#define COUNT  32*1024
#define APPCOUNT 96 * 1024
#define COUNT2 12*1024
#define APPCOUNT2 52 *1024
#define COUNT3 250*1024     
#define APPCOUNT3 96 *1024
#define COUNT4 39*1024

/*---------------------------------------------------------------------------*/
/* Module-globals                                                            */
/*---------------------------------------------------------------------------*/
//#define MAX_PATHNAME_LEN  100
static char proj_dir[MAX_PATHNAME_LEN];
static char file_name[MAX_PATHNAME_LEN];
static char prevFileName[MAX_PATHNAME_LEN]; 
static char appfile_name[MAX_PATHNAME_LEN];


static char extraboot[39*1024];
	//--------------------------
	// brake file stuff 
	//--------------------------
static char wave[256*1024];
static char headerBrakeFile[40];

static char appwave[96*1024];

static int handle;
static int err;
static char temp[40];

#define FILENAME_VERSTART 22 
#define FILENAME_NAMESTART 16
char baseFileName[] = "c:\\CreedMonarch\\sw033-00_00.bin"; 
char appbaseFileName[] = "c:\\CreedMonarch\\sw034-00_00.bin";  
 


char lotCode[6]; 
char rev[2];
int model,month,day,year; 
char newLotCode,newRev,newModel,fileGood;
int appfoundOne,foundOne;    

#define APP_INFO_START				0x89ABCDEF
#define APP_INFO_END				0xFEDCBA98
#define MAX_SEARCH_OFFSET			512

 

//---------------------GLOBAL VARIABLES-----------------------------------
AppInfo				brake;
AppInfo				remote;


void FindFile(void); 


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: GetAppInfo
//------------------------------------------------------------------------------
// This function parses app info starting at the given addr (failure returns 0)
//==============================================================================
static uint8_t GetAppInfo(uint32_t startAddress, AppInfo* info)
{
	uint8_t appInfoStarted = 0;
	
	//search starting at the given address for the markers that designate the app info section
	for (uint32_t addr = startAddress; addr < startAddress + MAX_SEARCH_OFFSET; addr += 4)
	{
		if (*((uint32_t*)addr) == APP_INFO_START)
		{
			info->checksum = *(uint32_t*)(addr + 4);
			info->appLength = *(uint32_t*)(addr + 8);
 			info->version = *(uint32_t*)(addr + 12);                       
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
				return 1;
			}
			//else we should have gotten an end marker by now...
		}
		//else we haven't found the app info start marker yet
	}
	
	//we didn't find both a start and end marker
	return 0;
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   
//------------------------------------------------------------------------------
// This function 
//==============================================================================
//---------------------------------------------------------------------
// Description:  
//---------------------------------------------------------------------
int CVICALLBACK GetFileVersionsCallback (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
	int buttonValue,i,j,status,temp; 
	unsigned __int64 ltemp;
	
    switch (event)
    {
		case EVENT_COMMIT:
            GetCtrlVal ( panelHandle  , PANEL_GETFILEVERSIONS, &buttonValue);
			if (buttonValue != 0)
			{
			  	SetCtrlVal (panelHandle, PANEL_GETFILEVERSIONS, 1);   
			    FindFile();   
			}
			else
			{
 			    SetCtrlVal (panelHandle, PANEL_GETFILEVERSIONS, 0);
			   
			}
            break;
            
        }
    return 0;

}

/*---------------------------------------------------------------------------*/
/* This is the application's entry-point.                                    */
/*---------------------------------------------------------------------------*/
void DownloadMain (void)
{
	int i,done,j; 
	int a,b,c,d;
	ssize_t size; 
	char tempName[4]; 
	
    
    GetProjectDir (proj_dir);	
//	FindFile();
	
	newRev = 0;
	newLotCode = 0;
	newModel = 0; 
	fileGood = 0;
}

/*---------------------------------------------------------------------------*/
/*                                                     */
/*---------------------------------------------------------------------------*/
void FindFile(void)
{
	int i,done,j; 
	int a,b,c,d;
	ssize_t size; 
	char tempName[4]; 	
  	
	memset(wave, 0, sizeof(wave));
	//DO THE BRAKE FILE 
	//---------------------------------------
	// check if the bootfile exists. 
	// format is: sw033-xx_xxx.bin"
	//-------------------------
	for(i=0;i<31;i++)
	{
		file_name[i] = baseFileName[i];
		prevFileName[i] = baseFileName[i];
	}	
	// LOOK AT MSB 
	//----------------------
	done = FALSE; 
	foundOne = FALSE; 
	i = 0; 	
	for (a=0;a<10;a++)
	{
		for (b=0;b<10;b++)
		{
			for (c=0;c<10;c++)
			{
				for (d=0;d<10;d++)
				{				   
					file_name[FILENAME_VERSTART] = a | 0x30; 
					file_name[FILENAME_VERSTART+1] = b|0x30;
					file_name[FILENAME_VERSTART+3] = c | 0x30;
					file_name[FILENAME_VERSTART+4] = d | 0x30; 
					if (FileExists(file_name,&size)==1)
					{
						foundOne = TRUE; 
						if (prevFileName[FILENAME_VERSTART] > file_name[FILENAME_VERSTART])
						{
						
						}
						else if(prevFileName[FILENAME_VERSTART] == file_name[FILENAME_VERSTART])
						{
							if (prevFileName[FILENAME_VERSTART+1] > file_name[FILENAME_VERSTART+1])
							{
						
							}
							else if(prevFileName[FILENAME_VERSTART+1] == file_name[FILENAME_VERSTART+1])
							{
								if (prevFileName[FILENAME_VERSTART+3] > file_name[FILENAME_VERSTART+3])
								{
						
								}
								else if(prevFileName[FILENAME_VERSTART+3] == file_name[FILENAME_VERSTART+3])
								{
									if (prevFileName[FILENAME_VERSTART+4] > file_name[FILENAME_VERSTART+4])
									{
						
									}
									else
									{
										prevFileName[FILENAME_VERSTART] = file_name[FILENAME_VERSTART]; 
										prevFileName[FILENAME_VERSTART+1] = file_name[FILENAME_VERSTART+1];      
										prevFileName[FILENAME_VERSTART+3] = file_name[FILENAME_VERSTART+3];      
										prevFileName[FILENAME_VERSTART+4] = file_name[FILENAME_VERSTART+4];      
			 					 	}							
								}
								else
								{
									prevFileName[FILENAME_VERSTART] = file_name[FILENAME_VERSTART]; 
									prevFileName[FILENAME_VERSTART+1] = file_name[FILENAME_VERSTART+1];      
									prevFileName[FILENAME_VERSTART+3] = file_name[FILENAME_VERSTART+3];      
									prevFileName[FILENAME_VERSTART+4] = file_name[FILENAME_VERSTART+4];      
		 					 	}							
							}
							else
							{
								prevFileName[FILENAME_VERSTART] = file_name[FILENAME_VERSTART]; 
								prevFileName[FILENAME_VERSTART+1] = file_name[FILENAME_VERSTART+1];      
								prevFileName[FILENAME_VERSTART+3] = file_name[FILENAME_VERSTART+3];      
								prevFileName[FILENAME_VERSTART+4] = file_name[FILENAME_VERSTART+4];      
	 					 	}							
						}
						else
						{
							prevFileName[FILENAME_VERSTART] = file_name[FILENAME_VERSTART]; 
							prevFileName[FILENAME_VERSTART+1] = file_name[FILENAME_VERSTART+1];      
							prevFileName[FILENAME_VERSTART+3] = file_name[FILENAME_VERSTART+3];      
							prevFileName[FILENAME_VERSTART+4] = file_name[FILENAME_VERSTART+4];      
 					 	}
					}
				}
			}
		}
	}
	file_name[FILENAME_VERSTART] = prevFileName[FILENAME_VERSTART]; 
	file_name[FILENAME_VERSTART+1]=prevFileName[FILENAME_VERSTART+1];      
	file_name[FILENAME_VERSTART+3]=prevFileName[FILENAME_VERSTART+3];      
	file_name[FILENAME_VERSTART+4]=prevFileName[FILENAME_VERSTART+4];       
	SetCtrlVal (panelHandle, PANEL_FILE_BRAKE, file_name);     
	if (foundOne != TRUE)
	{
		MessagePopup ("File Error", "Did not find a Brake file. Check directory"); 	
	}		
 
	//DO THE app FILE 
	//---------------------------------------
	// check if the bootfile exists. 
	// format is: sw025-xx_xxx.bin"
	for(i=0;i<31;i++)
	{
		appfile_name[i] = appbaseFileName[i];
		prevFileName[i] = appbaseFileName[i];
	}
	//----------------------
	done = FALSE; 
	appfoundOne = FALSE; 
	i = 0; 	
	for (a=0;a<10;a++)
	{
		for (b=0;b<10;b++)
		{
			for (c=0;c<10;c++)
			{
				for (d=0;d<10;d++)
				{				   
					appfile_name[FILENAME_VERSTART] = a | 0x30; 
					appfile_name[FILENAME_VERSTART+1] = b|0x30;
					appfile_name[FILENAME_VERSTART+3] = c | 0x30;
					appfile_name[FILENAME_VERSTART+4] = d | 0x30; 
					if (FileExists(appfile_name,&size)==1)
					{
						appfoundOne = TRUE; 
						if (prevFileName[FILENAME_VERSTART] > appfile_name[FILENAME_VERSTART])
						{
						
						}
						else if(prevFileName[FILENAME_VERSTART] == appfile_name[FILENAME_VERSTART])
						{
							if (prevFileName[FILENAME_VERSTART+1] > appfile_name[FILENAME_VERSTART+1])
							{
						
							}
							else if(prevFileName[FILENAME_VERSTART+1] == appfile_name[FILENAME_VERSTART+1])
							{
								if (prevFileName[FILENAME_VERSTART+3] > appfile_name[FILENAME_VERSTART+3])
								{
						
								}
								else if(prevFileName[FILENAME_VERSTART+3] == appfile_name[FILENAME_VERSTART+3])
								{
									if (prevFileName[FILENAME_VERSTART+4] > appfile_name[FILENAME_VERSTART+4])
									{
						
									}
									else
									{
										prevFileName[FILENAME_VERSTART] = appfile_name[FILENAME_VERSTART]; 
										prevFileName[FILENAME_VERSTART+1] = appfile_name[FILENAME_VERSTART+1];      
										prevFileName[FILENAME_VERSTART+3] = appfile_name[FILENAME_VERSTART+3];      
										prevFileName[FILENAME_VERSTART+4] = appfile_name[FILENAME_VERSTART+4];      
			 					 	}							
								}
								else
								{
									prevFileName[FILENAME_VERSTART] = appfile_name[FILENAME_VERSTART]; 
									prevFileName[FILENAME_VERSTART+1] = appfile_name[FILENAME_VERSTART+1];      
									prevFileName[FILENAME_VERSTART+3] = appfile_name[FILENAME_VERSTART+3];      
									prevFileName[FILENAME_VERSTART+4] = appfile_name[FILENAME_VERSTART+4];      
		 					 	}							
							}
							else
							{
								prevFileName[FILENAME_VERSTART] = appfile_name[FILENAME_VERSTART]; 
								prevFileName[FILENAME_VERSTART+1] = appfile_name[FILENAME_VERSTART+1];      
								prevFileName[FILENAME_VERSTART+3] = appfile_name[FILENAME_VERSTART+3];      
								prevFileName[FILENAME_VERSTART+4] = appfile_name[FILENAME_VERSTART+4];      
	 					 	}							
						}
						else
						{
							prevFileName[FILENAME_VERSTART] = appfile_name[FILENAME_VERSTART]; 
							prevFileName[FILENAME_VERSTART+1] = appfile_name[FILENAME_VERSTART+1];      
							prevFileName[FILENAME_VERSTART+3] = appfile_name[FILENAME_VERSTART+3];      
							prevFileName[FILENAME_VERSTART+4] = appfile_name[FILENAME_VERSTART+4];      
 					 	}
					}
				}
			}
		}
	}
	appfile_name[FILENAME_VERSTART] = prevFileName[FILENAME_VERSTART]; 
	appfile_name[FILENAME_VERSTART+1]=prevFileName[FILENAME_VERSTART+1];      
	appfile_name[FILENAME_VERSTART+3]=prevFileName[FILENAME_VERSTART+3];      
	appfile_name[FILENAME_VERSTART+4]=prevFileName[FILENAME_VERSTART+4];      
	//-----------------------------------------------
	// show the resultant bootloader version on the display
	SetCtrlVal (panelHandle, PANEL_FILE_REMOTE, appfile_name);     
	if (appfoundOne != TRUE)
	{
		MessagePopup ("File Error", "Did not find a REMOTE file. Check directory"); 	
	}
//===============================================================================================		
 
	if ((appfoundOne==TRUE)&&(foundOne==TRUE))
	{
		
	
	    if (FileToArray (file_name, wave, VAL_CHAR, COUNT, 1,
                  VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_BINARY) == 0)
		{	
		    if (FileToArray (appfile_name, appwave, VAL_CHAR, APPCOUNT, 1,
		                  VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_BINARY) == 0)
			{	
				
				GetAppInfo((uint32_t)wave, &brake);    
				GetAppInfo((uint32_t)appwave, &remote);
/*
				fileGood = 1; 
				tempName[0] = appfile_name[FILENAME_VERSTART];     			
				tempName[1] = appfile_name[FILENAME_VERSTART+1];				
				tempName[2] = appfile_name[FILENAME_VERSTART+3];     			
				tempName[3] = appfile_name[FILENAME_VERSTART+4];	
				appfile_name[FILENAME_VERSTART] = 'X';     			
				appfile_name[FILENAME_VERSTART+1] = 'X';				
				appfile_name[FILENAME_VERSTART+3] = 'X';     			
				appfile_name[FILENAME_VERSTART+4] = 'X';
	            ArrayToFile (appfile_name, appwave, VAL_CHAR, APPCOUNT, 1,
	                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
	                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
				appfile_name[FILENAME_VERSTART] = tempName[0];     			
				appfile_name[FILENAME_VERSTART+1] = tempName[1];				
				appfile_name[FILENAME_VERSTART+3] = tempName[2];     			
				appfile_name[FILENAME_VERSTART+4] = tempName[3];				
*/
			}	
			else
			{
				MessagePopup ("File Error", "ISSUE reading in the application"); 	   
			}				
	   }	
		else
		{
			MessagePopup ("File Error", "ISSUE reading in the bootloader"); 	   
		}	
	}
}



#if 0
/*---------------------------------------------------------------------------*/
/* Plot some data to the Graph control.                                      */
/*---------------------------------------------------------------------------*/
int CVICALLBACK Program (int panel, int control, int event, void *callbackData,
                      int eventData1, int eventData2)
{
    int i;

    if (event == EVENT_COMMIT)
    {
 		if ((appfoundOne == TRUE)&&(foundOne==TRUE))
		{
		   switch (model)
		   {
				case 0x08:
				{
			    	   if (LaunchExecutable("c:\\c-monster\\microfiles\\Bootloader.bat") != 0)
					   {
						 MessagePopup ("Program Error", "Programming batch file missing!"); 	  
					   }   
					   break;
				}
		   		case 0x01:
				case 0x02:
		   		case 0x03:
				case 0x04:
		   		case 0x05:
				case 0x06:	
			    case 0x07:
			   {
		    	   if (LaunchExecutable("c:\\c-monster\\hydrofiles\\Bootloader.bat") != 0)
				   {
					 MessagePopup ("Program Error", "Programming batch file missing!"); 	  
				   }   	
				   break;
			   }  
		   		case 0x09:
				case 0x0a:
		   		case 0x0b:
				case 0x0c:
		   		case 0x0d:
				case 0x0e:	
			    case 0x0f:
			   {
		    	   if (LaunchExecutable("c:\\c-monster\\hydbifiles\\Bootloader.bat") != 0)
				   {
					 MessagePopup ("Program Error", "Programming batch file missing!"); 	  
				   }   	
				   break;
			   }  		
		   		case 16:
				case 17:
		   		case 18:
				case 19:
		   		case 20:
				case 21:	
			    case 22:
			   {
		    	   if (LaunchExecutable("c:\\c-monster\\hydCMfiles\\Bootloader.bat") != 0)
				   {
					 MessagePopup ("Program Error", "Programming batch file missing!"); 	  
				   }   	
				   break;
			   }  			   			   
		   }  
        }
		else
		{
			MessagePopup ("Program Error", "MISSING FILES"); 	
		}	
	}	
    return 0;
}

/*---------------------------------------------------------------------------*/
/* This function brings up a File Selection dialog and allows you to enter a */
/* file name with a .dat extension.  After the file name is entered or       */
/* selected, that file is opened and assigned a file handle.                 */
/* This file handle is used anytime action is performed on the file.  The    */
/* file is written using the ArrayToFile function, then closed.              */
/*---------------------------------------------------------------------------*/
int CVICALLBACK Save (int panel, int control, int event, void *callbackData,
                      int eventData1, int eventData2)
{

    int i;
    int fileType;

	int offset,itemp,itemp2; 
	
    if (event == EVENT_COMMIT)
    {
		if (newLotCode == 0)
		{
			MessagePopup ("Save Error", "Enter Lot Code"); 	
		}
		else if (newRev == 0)
		{
			MessagePopup ("Save Error", "Enter Rev"); 	       
		}
		else if (newModel == 0)
		{
			MessagePopup ("Save Error", "Enter Model"); 	       	
		}
		else
		{
					GetCtrlVal (handle,Examp1_STRING_LOTCODE, &lotCode[0]);      
					GetCtrlVal (handle,Examp1_STRING_REV, &rev[0]);    
					GetCtrlVal (handle,Examp1_LISTBOX,&model);			
			switch (model)
			{
				case 0x08:
				{

					offset = 0x8000 - 15-11-11 - 9;
			
					wave[offset++] = 'M';
					wave[offset++] = 'O';
					wave[offset++] = 'D';
					wave[offset++] = 'E';
					wave[offset++] = 'L';
					wave[offset++] = ' ';	
					i = model >>8;
					wave[offset++] = i;
					i = model & 0xff; 
					wave[offset++] = i;
					wave[offset++] = ' ';			
					//----------------------app version 
					//
					for (i=0;i<11;i++)
					{
						wave[offset++] = appfile_name[FILENAME_NAMESTART+i];
					}
					for (i=0;i<11;i++)
					{
						wave[offset++] = file_name[FILENAME_NAMESTART+i];
					}			
			
					wave[offset++] = model; 
					wave[offset++]= rev[0]; 
				    wave[offset++] = lotCode[4];
				    wave[offset++]= lotCode[3];      
				    wave[offset++]= lotCode[2];      
				    wave[offset++] = lotCode[1];      
				    wave[offset++] = lotCode[0];     
					GetSystemDate(&month,&day,&year);
					wave[offset++] = month;
					wave[offset++]= day;
					itemp = year;
					itemp2 = itemp>>8;
					wave[offset++] = itemp2;
					itemp = itemp &0x00ff;
					wave[offset++] = itemp;
				    wave[offset++] = 'B';      
				    wave[offset++] = 'o';      
				    wave[offset++] = 'o';      
				    wave[offset++] = 't';      
		
 
					file_name[FILENAME_VERSTART] = 'X';     			
					file_name[FILENAME_VERSTART+1] = 'X';			
					file_name[FILENAME_VERSTART+3] = 'X';     			
					file_name[FILENAME_VERSTART+4] = 'X';
		            ArrayToFile (file_name, wave, VAL_CHAR, COUNT, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
		            SetCtrlAttribute (handle, Examp1_Save, ATTR_DIMMED, 1);
				    SetCtrlAttribute (handle, Examp1_Program, ATTR_DIMMED, 0); 
					break;
				}
				case 0x07:		 
				{
					
					offset = 0x3000 - 12;
			
					
//					GetSystemDate(&month,&day,&year);
//					wave[offset++] = month;
//					wave[offset++]= day;
//					itemp = year;
//					itemp2 = itemp>>8;
//					wave[offset++] = itemp2;
//					itemp = itemp &0x00ff;
//					wave[offset++] = itemp;					
					
					
					wave[offset++] = 0x14;
					wave[offset++] = 0x44;
					//option byte
					wave[offset++] = 0x01;
					//------------psi
					wave[offset++] = 0x41;
					wave[offset++] = 0x32;
					
					wave[offset++] = model; 
					wave[offset++]= rev[0]; 
				    wave[offset++] = lotCode[0];
				    wave[offset++]= lotCode[1];      
				    wave[offset++]= lotCode[2];      
				    wave[offset++] = lotCode[3];      
				    wave[offset++] = lotCode[4];     

 
					file_name[FILENAME_VERSTART] = 'X';     			
					file_name[FILENAME_VERSTART+1] = 'X';			
					file_name[FILENAME_VERSTART+3] = 'X';     			
					file_name[FILENAME_VERSTART+4] = 'X';
		            ArrayToFile (file_name, wave, VAL_CHAR, COUNT2, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
		            SetCtrlAttribute (handle, Examp1_Save, ATTR_DIMMED, 1);
				    SetCtrlAttribute (handle, Examp1_Program, ATTR_DIMMED, 0); 
					break;					
				}
				case 0x06:
				case 0x05:	
				{
					
					offset = 0x3000 - 12;
			
					
//					GetSystemDate(&month,&day,&year);
//					wave[offset++] = month;
//					wave[offset++]= day;
//					itemp = year;
//					itemp2 = itemp>>8;
//					wave[offset++] = itemp2;
//					itemp = itemp &0x00ff;
//					wave[offset++] = itemp;					
					
					
					wave[offset++] = 0x14;
					wave[offset++] = 0x54;
					//option byte
					wave[offset++] = 0x01;
					//------------psi
					wave[offset++] = 0x41;
					wave[offset++] = 0x32;
					
					wave[offset++] = model; 
					wave[offset++]= rev[0]; 
				    wave[offset++] = lotCode[0];
				    wave[offset++]= lotCode[1];      
				    wave[offset++]= lotCode[2];      
				    wave[offset++] = lotCode[3];      
				    wave[offset++] = lotCode[4];     

 
					file_name[FILENAME_VERSTART] = 'X';     			
					file_name[FILENAME_VERSTART+1] = 'X';			
					file_name[FILENAME_VERSTART+3] = 'X';     			
					file_name[FILENAME_VERSTART+4] = 'X';
		            ArrayToFile (file_name, wave, VAL_CHAR, COUNT2, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
		            SetCtrlAttribute (handle, Examp1_Save, ATTR_DIMMED, 1);
				    SetCtrlAttribute (handle, Examp1_Program, ATTR_DIMMED, 0); 
					break;					
				}	
				case 0x04:
				{
					
					offset = 0x3000 - 12;
		
					
					
//					GetSystemDate(&month,&day,&year);
//					wave[offset++] = month;
//					wave[offset++]= day;
//					itemp = year;
//					itemp2 = itemp>>8;
//					wave[offset++] = itemp2;
//					itemp = itemp &0x00ff;
//					wave[offset++] = itemp;					
					
					
					wave[offset++] = 0x14;
					wave[offset++] = 0x64;
					//option byte
					wave[offset++] = 0x01;
					//------------psi
					wave[offset++] = 0x3c;
					wave[offset++] = 0x28;
					
					wave[offset++] = model; 
					wave[offset++]= rev[0]; 
				    wave[offset++] = lotCode[0];
				    wave[offset++]= lotCode[1];      
				    wave[offset++]= lotCode[2];      
				    wave[offset++] = lotCode[3];      
				    wave[offset++] = lotCode[4];     

 
					file_name[FILENAME_VERSTART] = 'X';     			
					file_name[FILENAME_VERSTART+1] = 'X';			
					file_name[FILENAME_VERSTART+3] = 'X';     			
					file_name[FILENAME_VERSTART+4] = 'X';
		            ArrayToFile (file_name, wave, VAL_CHAR, COUNT2, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
		            SetCtrlAttribute (handle, Examp1_Save, ATTR_DIMMED, 1);
				    SetCtrlAttribute (handle, Examp1_Program, ATTR_DIMMED, 0); 
					break;					
				}	
				case 0x03:
				{
					
					offset = 0x3000 - 12;
			
					
					wave[offset++] = 0x14;
					wave[offset++] = 0x64;
					//option byte
					wave[offset++] = 0x01;
					//------------psi
					wave[offset++] = 0x41;
					wave[offset++] = 0x37;
					
					wave[offset++] = model; 
					wave[offset++]= rev[0]; 
				    wave[offset++] = lotCode[0];
				    wave[offset++]= lotCode[1];      
				    wave[offset++]= lotCode[2];      
				    wave[offset++] = lotCode[3];      
				    wave[offset++] = lotCode[4];     

 
					file_name[FILENAME_VERSTART] = 'X';     			
					file_name[FILENAME_VERSTART+1] = 'X';			
					file_name[FILENAME_VERSTART+3] = 'X';     			
					file_name[FILENAME_VERSTART+4] = 'X';
		            ArrayToFile (file_name, wave, VAL_CHAR, COUNT2, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
		            SetCtrlAttribute (handle, Examp1_Save, ATTR_DIMMED, 1);
				    SetCtrlAttribute (handle, Examp1_Program, ATTR_DIMMED, 0); 
					break;					
				}
				case 0x02:
				{
					
					offset = 0x3000 - 12;
			
					
//					GetSystemDate(&month,&day,&year);
//					wave[offset++] = month;
//					wave[offset++]= day;
//					itemp = year;
//					itemp2 = itemp>>8;
//					wave[offset++] = itemp2;
//					itemp = itemp &0x00ff;
//					wave[offset++] = itemp;					
					
					
					wave[offset++] = 0x14;
					wave[offset++] = 0x64;
					//option byte
					wave[offset++] = 0x01;
					//------------psi
					wave[offset++] = 0x46;
					wave[offset++] = 0x37;
					
					wave[offset++] = model; 
					wave[offset++]= rev[0]; 
				    wave[offset++] = lotCode[0];
				    wave[offset++]= lotCode[1];      
				    wave[offset++]= lotCode[2];      
				    wave[offset++] = lotCode[3];      
				    wave[offset++] = lotCode[4];     

 
					file_name[FILENAME_VERSTART] = 'X';     			
					file_name[FILENAME_VERSTART+1] = 'X';			
					file_name[FILENAME_VERSTART+3] = 'X';     			
					file_name[FILENAME_VERSTART+4] = 'X';
		            ArrayToFile (file_name, wave, VAL_CHAR, COUNT2, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
		            SetCtrlAttribute (handle, Examp1_Save, ATTR_DIMMED, 1);
				    SetCtrlAttribute (handle, Examp1_Program, ATTR_DIMMED, 0); 
					break;					
				}	
				case 0x01:
				{
					
					offset = 0x3000 - 12;
			
					
//					GetSystemDate(&month,&day,&year);
//					wave[offset++] = month;
//					wave[offset++]= day;
//					itemp = year;
//					itemp2 = itemp>>8;
//					wave[offset++] = itemp2;
//					itemp = itemp &0x00ff;
//					wave[offset++] = itemp;					
					
					
					wave[offset++] = 0x14;
					wave[offset++] = 0x64;
					//option byte
					wave[offset++] = 0x01;
					//------------psi
					wave[offset++] = 0x78;
					wave[offset++] = 0x50;
					
					wave[offset++] = model; 
					wave[offset++]= rev[0]; 
				    wave[offset++] = lotCode[0];
				    wave[offset++]= lotCode[1];      
				    wave[offset++]= lotCode[2];      
				    wave[offset++] = lotCode[3];      
				    wave[offset++] = lotCode[4];     

 
					file_name[FILENAME_VERSTART] = 'X';     			
					file_name[FILENAME_VERSTART+1] = 'X';			
					file_name[FILENAME_VERSTART+3] = 'X';     			
					file_name[FILENAME_VERSTART+4] = 'X';
		            ArrayToFile (file_name, wave, VAL_CHAR, COUNT2, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
		            SetCtrlAttribute (handle, Examp1_Save, ATTR_DIMMED, 1);
				    SetCtrlAttribute (handle, Examp1_Program, ATTR_DIMMED, 0); 
					break;					
				}	
//------------------------------------------------------------------------------------------------				
				case 0x0f:
				case 22:
				{
					
					offset = 0x3000 - 12;
					wave[offset++] = 0x14;
					wave[offset++] = 0x44;
					//option byte
					wave[offset++] = 0x01;
					//------------psi
					wave[offset++] = 0x41;
					wave[offset++] = 0x32;
					
					wave[offset++] = model; 
					wave[offset++]= rev[0]; 
				    wave[offset++] = lotCode[0];
				    wave[offset++]= lotCode[1];      
				    wave[offset++]= lotCode[2];      
				    wave[offset++] = lotCode[3];      
				    wave[offset++] = lotCode[4];     

 
					file_name[FILENAME_VERSTART] = 'X';     			
					file_name[FILENAME_VERSTART+1] = 'X';			
					file_name[FILENAME_VERSTART+3] = 'X';     			
					file_name[FILENAME_VERSTART+4] = 'X';
					//newboot
					for (i=0;i<COUNT4;i++)
					{
						extraboot[i] = wave[i+0x036000];
					}
					//------------------------------------------
					ArrayToFile (file_name, wave, VAL_CHAR, COUNT3B, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
					//newboot
					file_name[FILENAME_VERSTART] = 'B';     			
					file_name[FILENAME_VERSTART+1] = 'B';			
					file_name[FILENAME_VERSTART+3] = 'B';     			
					file_name[FILENAME_VERSTART+4] = 'B';					
					ArrayToFile (file_name, extraboot, VAL_CHAR, COUNT4, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
					//------------------------------------------------------
		            SetCtrlAttribute (handle, Examp1_Save, ATTR_DIMMED, 1);
				    SetCtrlAttribute (handle, Examp1_Program, ATTR_DIMMED, 0); 
					break;					
				}
				case 0x0e:
				case 0x0d:	
				case 20:
				case 21:
				{
					
					offset = 0x3000 - 12;
					wave[offset++] = 0x14;
					wave[offset++] = 0x54;
					//option byte
					wave[offset++] = 0x01;
					//------------psi
					wave[offset++] = 0x41;
					wave[offset++] = 0x32;
					
					wave[offset++] = model; 
					wave[offset++]= rev[0]; 
				    wave[offset++] = lotCode[0];
				    wave[offset++]= lotCode[1];      
				    wave[offset++]= lotCode[2];      
				    wave[offset++] = lotCode[3];      
				    wave[offset++] = lotCode[4];     

 
					file_name[FILENAME_VERSTART] = 'X';     			
					file_name[FILENAME_VERSTART+1] = 'X';			
					file_name[FILENAME_VERSTART+3] = 'X';     			
					file_name[FILENAME_VERSTART+4] = 'X';
					//newboot
					for (i=0;i<COUNT4;i++)
					{
						extraboot[i] = wave[i+0x036000];
					}
					//------------------------------------------
					ArrayToFile (file_name, wave, VAL_CHAR, COUNT3B, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
					//newboot
					file_name[FILENAME_VERSTART] = 'B';     			
					file_name[FILENAME_VERSTART+1] = 'B';			
					file_name[FILENAME_VERSTART+3] = 'B';     			
					file_name[FILENAME_VERSTART+4] = 'B';					
					ArrayToFile (file_name, extraboot, VAL_CHAR, COUNT4, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
					//------------------------------------------------------
		            SetCtrlAttribute (handle, Examp1_Save, ATTR_DIMMED, 1);
				    SetCtrlAttribute (handle, Examp1_Program, ATTR_DIMMED, 0); 
					break;					
				}	
				case 0x0c:
				case 19:
				{
					
					offset = 0x3000 - 12;
					wave[offset++] = 0x14;
					wave[offset++] = 0x64;
					//option byte
					wave[offset++] = 0x01;
					//------------psi
					wave[offset++] = 0x3c;
					wave[offset++] = 0x28;
					
					wave[offset++] = model; 
					wave[offset++]= rev[0]; 
				    wave[offset++] = lotCode[0];
				    wave[offset++]= lotCode[1];      
				    wave[offset++]= lotCode[2];      
				    wave[offset++] = lotCode[3];      
				    wave[offset++] = lotCode[4];     

 
					file_name[FILENAME_VERSTART] = 'X';     			
					file_name[FILENAME_VERSTART+1] = 'X';			
					file_name[FILENAME_VERSTART+3] = 'X';     			
					file_name[FILENAME_VERSTART+4] = 'X';
					//newboot
					for (i=0;i<COUNT4;i++)
					{
						extraboot[i] = wave[i+0x036000];
					}
					//------------------------------------------
					ArrayToFile (file_name, wave, VAL_CHAR, COUNT3B, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
					//newboot
					file_name[FILENAME_VERSTART] = 'B';     			
					file_name[FILENAME_VERSTART+1] = 'B';			
					file_name[FILENAME_VERSTART+3] = 'B';     			
					file_name[FILENAME_VERSTART+4] = 'B';					
					ArrayToFile (file_name, extraboot, VAL_CHAR, COUNT4, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
					//------------------------------------------------------
		            SetCtrlAttribute (handle, Examp1_Save, ATTR_DIMMED, 1);
				    SetCtrlAttribute (handle, Examp1_Program, ATTR_DIMMED, 0); 
					break;					
				}	
				case 0x0b:
				case 18:	
				{
					
					offset = 0x3000 - 12;
			
					
					wave[offset++] = 0x14;
					wave[offset++] = 0x64;
					//option byte
					wave[offset++] = 0x01;
					//------------psi
					wave[offset++] = 0x41;
					wave[offset++] = 0x37;
					
					wave[offset++] = model; 
					wave[offset++]= rev[0]; 
				    wave[offset++] = lotCode[0];
				    wave[offset++]= lotCode[1];      
				    wave[offset++]= lotCode[2];      
				    wave[offset++] = lotCode[3];      
				    wave[offset++] = lotCode[4];     

 
					file_name[FILENAME_VERSTART] = 'X';     			
					file_name[FILENAME_VERSTART+1] = 'X';			
					file_name[FILENAME_VERSTART+3] = 'X';     			
					file_name[FILENAME_VERSTART+4] = 'X';
					//newboot
					for (i=0;i<COUNT4;i++)
					{
						extraboot[i] = wave[i+0x036000];
					}
					//------------------------------------------
					ArrayToFile (file_name, wave, VAL_CHAR, COUNT3B, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
					//newboot
					file_name[FILENAME_VERSTART] = 'B';     			
					file_name[FILENAME_VERSTART+1] = 'B';			
					file_name[FILENAME_VERSTART+3] = 'B';     			
					file_name[FILENAME_VERSTART+4] = 'B';					
					ArrayToFile (file_name, extraboot, VAL_CHAR, COUNT4, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
					//------------------------------------------------------
		            SetCtrlAttribute (handle, Examp1_Save, ATTR_DIMMED, 1);
				    SetCtrlAttribute (handle, Examp1_Program, ATTR_DIMMED, 0); 
					break;					
				}
				case 0x0a:
				case 17:
				{
					
					offset = 0x3000 - 12;
			
					
//					GetSystemDate(&month,&day,&year);
//					wave[offset++] = month;
//					wave[offset++]= day;
//					itemp = year;
//					itemp2 = itemp>>8;
//					wave[offset++] = itemp2;
//					itemp = itemp &0x00ff;
//					wave[offset++] = itemp;					
					
					
					wave[offset++] = 0x14;
					wave[offset++] = 0x64;
					//option byte
					wave[offset++] = 0x01;
					//------------psi
					wave[offset++] = 0x46;
					wave[offset++] = 0x37;
					
					wave[offset++] = model; 
					wave[offset++]= rev[0]; 
				    wave[offset++] = lotCode[0];
				    wave[offset++]= lotCode[1];      
				    wave[offset++]= lotCode[2];      
				    wave[offset++] = lotCode[3];      
				    wave[offset++] = lotCode[4];     

 
					file_name[FILENAME_VERSTART] = 'X';     			
					file_name[FILENAME_VERSTART+1] = 'X';			
					file_name[FILENAME_VERSTART+3] = 'X';     			
					file_name[FILENAME_VERSTART+4] = 'X';
					//newboot
					for (i=0;i<COUNT4;i++)
					{
						extraboot[i] = wave[i+0x036000];
					}
					//------------------------------------------
					ArrayToFile (file_name, wave, VAL_CHAR, COUNT3B, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
					//newboot
					file_name[FILENAME_VERSTART] = 'B';     			
					file_name[FILENAME_VERSTART+1] = 'B';			
					file_name[FILENAME_VERSTART+3] = 'B';     			
					file_name[FILENAME_VERSTART+4] = 'B';					
					ArrayToFile (file_name, extraboot, VAL_CHAR, COUNT4, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
					//------------------------------------------------------
		            SetCtrlAttribute (handle, Examp1_Save, ATTR_DIMMED, 1);
				    SetCtrlAttribute (handle, Examp1_Program, ATTR_DIMMED, 0); 
					break;					
				}	
				case 0x09:
				case 16:	
				{
					
					offset = 0x3000 - 12;
			
					
//					GetSystemDate(&month,&day,&year);
//					wave[offset++] = month;
//					wave[offset++]= day;
//					itemp = year;
//					itemp2 = itemp>>8;
//					wave[offset++] = itemp2;
//					itemp = itemp &0x00ff;
//					wave[offset++] = itemp;					
					
					
					wave[offset++] = 0x14;
					wave[offset++] = 0x64;
					//option byte
					wave[offset++] = 0x01;
					//------------psi
					wave[offset++] = 0x78;
					wave[offset++] = 0x50;
					
					wave[offset++] = model; 
					wave[offset++]= rev[0]; 
				    wave[offset++] = lotCode[0];
				    wave[offset++]= lotCode[1];      
				    wave[offset++]= lotCode[2];      
				    wave[offset++] = lotCode[3];      
				    wave[offset++] = lotCode[4];     

 
					file_name[FILENAME_VERSTART] = 'X';     			
					file_name[FILENAME_VERSTART+1] = 'X';			
					file_name[FILENAME_VERSTART+3] = 'X';     			
					file_name[FILENAME_VERSTART+4] = 'X';

					//newboot
					for (i=0;i<COUNT4;i++)
					{
						extraboot[i] = wave[i+0x036000];
					}
					//------------------------------------------
					ArrayToFile (file_name, wave, VAL_CHAR, COUNT3B, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
					//newboot
					file_name[FILENAME_VERSTART] = 'B';     			
					file_name[FILENAME_VERSTART+1] = 'B';			
					file_name[FILENAME_VERSTART+3] = 'B';     			
					file_name[FILENAME_VERSTART+4] = 'B';					
					ArrayToFile (file_name, extraboot, VAL_CHAR, COUNT4, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
					//------------------------------------------------------
					
		            SetCtrlAttribute (handle, Examp1_Save, ATTR_DIMMED, 1);
				    SetCtrlAttribute (handle, Examp1_Program, ATTR_DIMMED, 0); 
					break;					
				}						
				
				
//------------------------------------------------------------------------------------------------				
				default:
				{
					
					offset = 0x3000 - 12;
			
					
//					GetSystemDate(&month,&day,&year);
//					wave[offset++] = month;
//					wave[offset++]= day;
//					itemp = year;
//					itemp2 = itemp>>8;
//					wave[offset++] = itemp2;
//					itemp = itemp &0x00ff;
//					wave[offset++] = itemp;					
					
					
					wave[offset++] = 0x14;
					wave[offset++] = 0x44;
					//option byte
					wave[offset++] = 0x01;
					//------------psi
					wave[offset++] = 0x41;
					wave[offset++] = 0x32;
					
					wave[offset++] = model; 
					wave[offset++]= rev[0]; 
				    wave[offset++] = lotCode[0];
				    wave[offset++]= lotCode[1];      
				    wave[offset++]= lotCode[2];      
				    wave[offset++] = lotCode[3];      
				    wave[offset++] = lotCode[4];     

 
					file_name[FILENAME_VERSTART] = 'X';     			
					file_name[FILENAME_VERSTART+1] = 'X';			
					file_name[FILENAME_VERSTART+3] = 'X';     			
					file_name[FILENAME_VERSTART+4] = 'X';
		            ArrayToFile (file_name, wave, VAL_CHAR, COUNT2, 1,
		                         VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS,
		                         VAL_CONST_WIDTH, 10, VAL_BINARY, VAL_TRUNCATE);
		            SetCtrlAttribute (handle, Examp1_Save, ATTR_DIMMED, 1);
				    SetCtrlAttribute (handle, Examp1_Program, ATTR_DIMMED, 0); 
					break;					
				}				
			}	
		}	
    }
    return 0;
}

 

/*---------------------------------------------------------------------------*/
/*                                                     */
/*---------------------------------------------------------------------------*/
int CVICALLBACK cbLotCode (int panel, int control, int event, void *callbackData,
                      int eventData1, int eventData2)
{
    if (event == EVENT_COMMIT)
	{
		newLotCode = 1;
    }
    return 0;
}
/*---------------------------------------------------------------------------*/
/*                                                     */
/*---------------------------------------------------------------------------*/
int CVICALLBACK cbRev (int panel, int control, int event, void *callbackData,
                      int eventData1, int eventData2)
{
    if (event == EVENT_COMMIT)
	{
		newRev = 1;
    }
    return 0;
	
}


/*---------------------------------------------------------------------------*/
/*                                                     */
/*---------------------------------------------------------------------------*/
int CVICALLBACK cbFileName (int panel, int control, int event, void *callbackData,
                      int eventData1, int eventData2)
{
    if (event == EVENT_COMMIT)
	{
//		GetCtrlVal(handle,Examp1_STRING,file_name);   
	    if (FileToArray (file_name, wave, VAL_CHAR, COUNT, 1,
	                  VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_BINARY) == 0)
		{	
			fileGood = 1; 
	    }	
		else
		{
			MessagePopup ("File Error", "Enter Correct File name"); 	   
		}	
    }
    return 0;
}

#endif 
 

