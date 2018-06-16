//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: appProtocol
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor: STM32F103R
// TOOLS: IAR Workbench 
// DATE:
// CONTENTS: This file contains  
//------------------------------------------------------------------------------
// HISTORY: This file  
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#include <asf.h>
#include "dbrakeDefs.h"
#include "appProtocol.h"
#include "sx1272-fsk.h"
#include "config.h"
#include "appAccel.h"
#include "driverTSPI.h"
#include "radio.h"
#include "sx1272-LoRa.h"
#include "appLCD.h"
#include "appLCDConfigMisc.h"
#include "driverI2C.h"
#include "driverButtons.h"
#include "appmotor.h"
#include "driverSpeaker.h"
#include "appBluetooth.h"
#include "driverUSART.h"
#include "driverDownload.h"
#include "driverProgramming.h"

//---------------------GLOBAL VARIABLES-----------------------------------
uint8_t buffer[20];
uint8_t switchToFSK =FALSE; 

#if REMOTEBOARD
uint16_t gPrime; 
#define MAX_BRAKE_MESSAGE 2
uint8_t brakeMessageOffset = 0;
#define MAX_BRAKE_SYSTEM_MESSAGE 3
uint8_t brakeMessageSystemOffset = 0; 
void BrakeCommTask(void);
#endif 

uint8_t remoteSettings; 
uint8_t remoteStatus;
uint8_t remoteForce; 
uint8_t remoteVersionToReport[4];

//---------------------LOCAL VARIABLES------------------------------------
  //--------------------------
  //button check histories
#define MAX_PROTOCOL_BUFFER 150
uint8_t protocolBuffer[MAX_PROTOCOL_BUFFER]; 

	//--------------remote downloads
//uint16_t receiveLength;  
uint16_t remoteDownloadPacketNumber; 
uint16_t remoteDownloadPacketCount; 
uint16_t remoteDownloadNextPacketNumber;
uint16_t remoteDownloadLastPacketNumber; 
uint32_t remoteDownloadLength;  
//---------------------LOCAL FUNCTION PROTOTYPES--------------------------  
 void AppProtocolClearCommErrors(void);

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   
//------------------------------------------------------------------------------
// This function  
//==============================================================================
uint8_t AppProtocolChecksum(uint8_t *buffers,uint16_t length)
{
	uint8_t checksum,*ptr; 
	uint16_t i; 
	
	checksum = 0; 
	ptr = buffers; 
	
	for (i=0;i<length;i++)
	{
		checksum += *ptr++;	
	}
	return checksum; 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   
//------------------------------------------------------------------------------
// This function  
//==============================================================================
void AppProtocolClearCommErrors(void)
{
		commSupTimer = COMM_SUP_TIME; 
		commFailureCount = 0; 
		commErrorCount = 0; 
		brakeStatus.BrakeState &= ~BRAKESTATE_COMMERROR; 
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX BRAKE ONLY FUNCTIONS XXXXXXXXXXXXXXXXXXXXXX
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

#if BRAKEBOARD		

uint8_t newRemoteDownloadNeeded = FALSE; 
uint8_t NewRemoteDownload(void)
{
	//----------------- if there is a remote version in SCRATCH 
	//                    RETURN the information and allow the remote 
	//                    to decide if it wants a download. 
	//----------------------------------------
	CheckScratch();
	return newRemoteDownloadNeeded; 
}

extern uint16_t gPrime; 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   
//------------------------------------------------------------------------------
// This function  
//==============================================================================
void AppProtocolBrake(uint8_t *buffers)
{

	uint16_t command,itemp,itemp2,doffset,ltemp;
	uint8_t length,i,match,temp,offset,goodmsg,checksum;  
	uint16_t x,y,z;
	uint32_t ltemp1,ltemp2;
	uint8_t *lptr; 

	if ((buffers[1] == '#')&&(buffers[0]>3))
	{
		//--------------------------------
		// Qualify the received message - match is TRUE 
		// if the address is paired with the board. 
		// goodMsg = true if checksum/header/etc are good 
		//-----------------------------------
		match = FALSE; 
		goodmsg = FALSE;
		if((buffers[2] == table0.Item.EepromManDevSerial[4])&&
			(buffers[3] == table0.Item.EepromManDevSerial[5]))
		{
			match = TRUE; 
		}		
		//----------------
		// build the command
		itemp = buffers[4];
		itemp2 = buffers[5];
		itemp = itemp<<8; 
		itemp |= itemp2; 
		command = itemp;
		//----------------
		// get the length
		length = buffers[0];
	
		goodmsg = AppProtocolChecksum(&buffers[1],length-2);	
		if (goodmsg == buffers[length-1])
		{
			goodmsg = TRUE; 
		}
		else
		{
			goodmsg = FALSE; 
		}
								
		if ((((command == MSG_PR)&&(buffers[2] == 0xff)&&(buffers[3]==0xff)) ||
			((command != MSG_PR)&&(match == TRUE)))&&(goodmsg == TRUE))
		{
	
			if ((buffers[1] == '#')&&(length>3))
			{
				commSupTimer = COMM_SUP_TIME; 
				commFailureCount = 0; 
				commErrorCount = 0; 	
				brakeStatus.BrakeState &= ~BRAKESTATE_COMMERROR; 
					
				if ((command == MSG_STATUS)||(command == MSG_FV)||(command==0x5245)||(command==0x5241))		
				{
						remoteStatus = buffers[7];
						remoteForce = buffers[8]; 
						remoteSettings = buffers[9];
						//-----------------------force setting
						if ((remoteForce & 0x0f) != (table0.Item.MaxForce))	
						{
							table0.Item.MaxForce = remoteForce & 0x0f; 
							ConfigUpdate(table0.Item.MaxForce,MaxForce_Setting);
						}
						//--------------------- max force set 
						temp = remoteForce >>4; 
						temp &= 0x0f; 
						if ((temp) != (table0.Item.ForceMaxSet))	
						{
							table0.Item.ForceMaxSet = temp & 0x0f; 
							ConfigUpdate(table0.Item.ForceMaxSet,ForceMaxSetting);
						}					
						//-------------------handle active brake enable setting 	
						//--------------------- sensitivity set
						temp = remoteSettings >>4;
						temp &= 0x0f;
						if ((temp) != (table0.Item.SensitivitySet))
						{
							table0.Item.SensitivitySet= temp & 0x0f;
							ConfigUpdate(table0.Item.SensitivitySet,SensitivitySetting);
						}					
						temp = 0; 
						if ((remoteSettings & REMOTE_ACTIVEBRAKEENABLE)!= FALSE)
						{
							temp = TRUE; 
						}
						if (temp != table0.Item.ActiveBrakeEnable)	
						{
							table0.Item.ActiveBrakeEnable = temp;
							ConfigUpdate(table0.Item.ActiveBrakeEnable,ActiveBrakeEnableSetting);
						}			
						//-------------------handle TPMS Enable setting 	
						temp = 0; 
						if ((remoteSettings & REMOTE_TPMSENABLE)!= FALSE)
						{
							temp = TRUE; 
						}
						if (temp != table0.Item.TPMSEnable)	
						{
							table0.Item.TPMSEnable = temp;
							ConfigUpdate(table0.Item.TPMSEnable,TPMSEnableSetting);
						}																		
				}
				switch (command)
				{
					case 0x4452:
					{
						//----------------------------------
						// check the offset and see if download mode. 
						//----------------------------------
						protocolBuffer[0] = '#';
						protocolBuffer[1] = table0.Item.EepromManDevSerial[4];
						protocolBuffer[2] = table0.Item.EepromManDevSerial[5];
						protocolBuffer[3] = 'D';
						protocolBuffer[4] = 'R';
						protocolBuffer[5] = 0; //length 
						if ((NewRemoteDownload()==TRUE)&&(bluetoothAwake!=0))
						{
							//----------------
							// build the offset
							itemp = buffers[7];
							itemp2 = buffers[8];
							itemp = itemp<<8; 
							itemp |= itemp2; 
							doffset = itemp;						
							if (doffset == 0)
							{
								protocolBuffer[5] = 28; 
								//----------offset 
								protocolBuffer[6] = 0x00;
								protocolBuffer[7] = 0x00;
							//----------length 							
								//-------file length, 4 bytes
								ltemp1 = newRemoteInfo.appLength >>24;
								protocolBuffer[8] = ltemp1; 
								ltemp1 = newRemoteInfo.appLength >>16; 
								ltemp1 &= 0x00ff; 
								protocolBuffer[9] = ltemp1; 
								ltemp1 = newRemoteInfo.appLength >>8; 
								ltemp1 &= 0x00ff; 
								protocolBuffer[10] = ltemp1; 
								ltemp1 = newRemoteInfo.appLength; 
								ltemp1 &= 0x00ff; 
								protocolBuffer[11] = ltemp1; 	
								//-------file checksum, 4 bytes
								ltemp1 = newRemoteInfo.checksum >>24;
								protocolBuffer[12] = ltemp1; 
								ltemp1 = newRemoteInfo.checksum >>16; 
								ltemp1 &= 0x00ff; 
								protocolBuffer[13] = ltemp1; 
								ltemp1 = newRemoteInfo.checksum >>8; 
								ltemp1 &= 0x00ff; 
								protocolBuffer[14] = ltemp1; 
								ltemp1 = newRemoteInfo.checksum; 
								ltemp1 &= 0x00ff; 
								protocolBuffer[15] = ltemp1; 		
								//-------file checksum start, 4 bytes
								ltemp1 = newRemoteInfo.checksumStartOffset >>24;
								protocolBuffer[16] = ltemp1; 
								ltemp1 = newRemoteInfo.checksumStartOffset >>16; 
								ltemp1 &= 0x00ff; 
								protocolBuffer[17] = ltemp1; 
								ltemp1 = newRemoteInfo.checksumStartOffset >>8; 
								ltemp1 &= 0x00ff; 
								protocolBuffer[18] = ltemp1; 
								ltemp1 = newRemoteInfo.checksumStartOffset; 
								ltemp1 &= 0x00ff; 
								protocolBuffer[19] = ltemp1; 			
								//-------file version, 4 bytes 
								ltemp1 = newRemoteInfo.version >>24;
								protocolBuffer[20] = ltemp1; 
								ltemp1 = newRemoteInfo.version >>16; 
								ltemp1 &= 0x00ff; 
								protocolBuffer[21] = ltemp1; 
								ltemp1 = newRemoteInfo.version >>8; 
								ltemp1 &= 0x00ff; 
								protocolBuffer[22] = ltemp1; 
								ltemp1 = newRemoteInfo.version; 
								ltemp1 &= 0x00ff; 
								protocolBuffer[23] = ltemp1; 			
								//-------number of packets, 1 byte
								ltemp1 = newRemoteInfo.appLength;
								ltemp1 = ltemp1/128; 
								ltemp1++;
								remoteDownloadPacketCount = ltemp1; 
								protocolBuffer[24] = ltemp1>>8;
								ltemp1 &= 0xff; 
								protocolBuffer[25] = ltemp1;
							
								checksum = AppProtocolChecksum(protocolBuffer,26);
								protocolBuffer[26] = checksum;
								protocolBuffer[27] = 0x04;
								if (whichRadio == WHICHRADIO_LORA)
								{
									SX1272LoraTransmit(protocolBuffer,28);
								}
								else
								{
									SX1272FskTransmit(protocolBuffer,28);
								}							 
							}
							else
							{
								if (doffset == 0xffff)
								{
									newRemoteDownloadNeeded = FALSE; 
									protocolBuffer[5] = 14;
									//----------offset 
									protocolBuffer[6] = 0x00;
									protocolBuffer[7] = 0x00;
									//----------length 
									protocolBuffer[8] = 0x00; 
									protocolBuffer[9] = 0x00;
									protocolBuffer[10] = 0x00;
									protocolBuffer[11] = 0x00;
									checksum = AppProtocolChecksum(protocolBuffer,12);
									protocolBuffer[12] = checksum;
									protocolBuffer[13] = 0x04;
									if (whichRadio == WHICHRADIO_LORA)
									{
										SX1272LoraTransmit(protocolBuffer,14);
									}
									else
									{
										SX1272FskTransmit(protocolBuffer,14);
									}									
								
								}
								else
								{
							
									//-----------------offset greater than 0x00 
									protocolBuffer[5] = 138; 
									//----------offset 
									itemp = doffset >>8;
									itemp2 = doffset & 0xff; 
									protocolBuffer[6] = itemp;
									protocolBuffer[7] = itemp2;
									//---------------------------
									// one less than what you are on 
									ltemp1 = (doffset-1) *128; 
									ltemp2 = APP_SCRATCH_BASE +ltemp1; 
									lptr = ltemp2; 				 
			 						for (x=0;x<128;x++)
									{
										protocolBuffer[x+8] = *lptr++;	
									}
									checksum = AppProtocolChecksum(protocolBuffer,136);
									protocolBuffer[136] = checksum;
									protocolBuffer[137] = 0x04;
									if (whichRadio == WHICHRADIO_LORA)
									{
										SX1272LoraTransmit(protocolBuffer,138);
									}
									else
									{
										SX1272FskTransmit(protocolBuffer,138);
									}		 		
								}
							}
						}
						else
						{				
							remoteDownloadPacketNumber=0; 
							remoteDownloadPacketCount=0; 
							remoteDownloadNextPacketNumber=0;
							remoteDownloadLastPacketNumber=0; 
							remoteDownloadLength=0; 						
						 					 
							protocolBuffer[5] = 14;
							//----------offset 
							protocolBuffer[6] = 0x00;
							protocolBuffer[7] = 0x00;
							//----------length 
							protocolBuffer[8] = 0x00; 
							protocolBuffer[9] = 0x00;
							protocolBuffer[10] = 0x00;
							protocolBuffer[11] = 0x00;
							checksum = AppProtocolChecksum(protocolBuffer,12);
							protocolBuffer[12] = checksum;
							protocolBuffer[13] = 0x04;
							if (whichRadio == WHICHRADIO_LORA)
							{
								SX1272LoraTransmit(protocolBuffer,14);
							}
							else
							{
								SX1272FskTransmit(protocolBuffer,14);
							}	
						}
						break;
					}
					case MSG_SW:
					{
						protocolBuffer[0] = '#';
						protocolBuffer[1] = table0.Item.EepromManDevSerial[4];
						protocolBuffer[2] = table0.Item.EepromManDevSerial[5];
						protocolBuffer[3] = 'S';
						protocolBuffer[4] = 'W';					 
						protocolBuffer[5] = 0x09;
						protocolBuffer[6] = buffers[7];  //yes switching
						switchOnTransmit = buffers[7]; 
						checksum = AppProtocolChecksum(protocolBuffer,7);
						protocolBuffer[7] = checksum;
						protocolBuffer[8] = 0x04;
						if (whichRadio == WHICHRADIO_LORA)
						{
							SX1272LoraTransmit(protocolBuffer,9);
						}
						else
						{
							SX1272FskTransmit(protocolBuffer,9);
						}
					
						break;
					}				
					case MSG_PR:
					{			
						protocolBuffer[0] = '#';
						protocolBuffer[1] = table0.Item.EepromManDevSerial[4];
						protocolBuffer[2] = table0.Item.EepromManDevSerial[5];
						protocolBuffer[3] = 'P';
						protocolBuffer[4] = 'M';	
						if (setup_pressed == 0)		
						{
							protocolBuffer[4] = 'L';		
						}				
						protocolBuffer[5] = 0x08; 	
						checksum = AppProtocolChecksum(protocolBuffer,6);	
						protocolBuffer[6] = checksum; 			
						protocolBuffer[7] = 0x04; 	
						if (setup_pressed != 0)
						{						
							if (whichRadio == WHICHRADIO_LORA)
							{
								SX1272LoraTransmit(protocolBuffer,8);
							}
							else
							{
								SX1272FskTransmit(protocolBuffer,8);
							}
						}
						break;
					}
					case MSG_FV:
					{
 							//---------FV
						protocolBuffer[0] = '#';
						protocolBuffer[1] = table0.Item.EepromManDevSerial[4];
						protocolBuffer[2] = table0.Item.EepromManDevSerial[5];
						protocolBuffer[3] = 'F';
						protocolBuffer[4] = 'M';
						protocolBuffer[5] = 18;
						protocolBuffer[6] = FWVER3;
						protocolBuffer[7] = FWVER2;
						protocolBuffer[8] = FWVER1;
						protocolBuffer[9] =	FWVER0;			
						protocolBuffer[10]= MONTHMSB;
						protocolBuffer[11] = MONTHLSB;
						protocolBuffer[12] = DAYMSB;
						protocolBuffer[13] = DAYLSB;						
						protocolBuffer[14] = YEARMSB;
						protocolBuffer[15] = YEARLSB;	
						checksum = AppProtocolChecksum(protocolBuffer,16);	
						protocolBuffer[16] = checksum; 					
						protocolBuffer[17] = 0x04; 	
						if (whichRadio == WHICHRADIO_LORA)
						{
							SX1272LoraTransmit(protocolBuffer,18);
						}
						else
						{
							SX1272FskTransmit(protocolBuffer,18);
						}					
						break;
					}									
					case MSG_STATUS:
					{
 							//---------BS
						remoteVersionToReport[0] = buffers[10];
						remoteVersionToReport[1] = buffers[11];
						remoteVersionToReport[2] = buffers[12];
						remoteVersionToReport[3] = buffers[13];
					
						//--------------------------------------
						protocolBuffer[0] = '#';
						protocolBuffer[1] = table0.Item.EepromManDevSerial[4];
						protocolBuffer[2] = table0.Item.EepromManDevSerial[5];
						protocolBuffer[3] = 'B';
						protocolBuffer[4] = 'M';
						protocolBuffer[5] = 22;
						protocolBuffer[6] = statusData.TireRadio;
						protocolBuffer[7] = statusData.ExtRadio;
						protocolBuffer[8] = statusData.EEPROM;;
						protocolBuffer[9] =	statusData.Accelerometer;		
						protocolBuffer[10] = brakeStatus.VoltageInput;
						protocolBuffer[11] = brakeStatus.AccelerometerStatus;
						protocolBuffer[12] = brakeStatus.ActuatorStatus;;
						protocolBuffer[13] = brakeStatus.BrakeState;	
						protocolBuffer[14] = brakeStatus.VoltageSupercap; 	
						protocolBuffer[15] = brakeState; 		
						protocolBuffer[16] = 0x00; 	
						if (switchToFSK ==TRUE)
						{
							protocolBuffer[16] = 0x55; 
						}
						ltemp = gPrime;
						ltemp = ltemp>>8;
						protocolBuffer[17] = ltemp; 
						ltemp = gPrime & 0x00ff;	
						protocolBuffer[18] = ltemp; 	
						protocolBuffer[19] = 0x00; 	
 					
						checksum = AppProtocolChecksum(protocolBuffer,20);	
						protocolBuffer[20] = checksum; 																			
						protocolBuffer[21] = 0x04; 	
						if (whichRadio == WHICHRADIO_LORA)
						{
							SX1272LoraTransmit(protocolBuffer,22);
						}
						else
						{
							SX1272FskTransmit(protocolBuffer,22);
						}		
						break;
					}						
					case 0x5245:
					{
						//---------RE
						protocolBuffer[0] = '#';
						protocolBuffer[1] = table0.Item.EepromManDevSerial[4];
						protocolBuffer[2] = table0.Item.EepromManDevSerial[5];
						protocolBuffer[3] = 'R';
						protocolBuffer[4] = 'E';
						protocolBuffer[5] = 14;
						protocolBuffer[6] = table0.Item.EepromManDevSerial[0];
						protocolBuffer[7] = table0.Item.EepromManDevSerial[1];
						protocolBuffer[8] = table0.Item.EepromManDevSerial[2];;
						protocolBuffer[9] =	table0.Item.EepromManDevSerial[3];;				
						protocolBuffer[10] = table0.Item.EepromManDevSerial[4];
						protocolBuffer[11] = table0.Item.EepromManDevSerial[5];		
						checksum = AppProtocolChecksum(protocolBuffer,12);	
						protocolBuffer[12] = checksum; 				
						protocolBuffer[13] = 0x04; 		
						if (whichRadio == WHICHRADIO_LORA)
						{
							SX1272LoraTransmit(protocolBuffer,14);
						}
						else
						{
							SX1272FskTransmit(protocolBuffer,14);
						}											
						break;
					}
					case 0x5241:
					{
						//---------RA
						AccelProvideReading(&x,&y,&z); 
						protocolBuffer[0] = '#';
						protocolBuffer[1] = table0.Item.EepromManDevSerial[4];
						protocolBuffer[2] = table0.Item.EepromManDevSerial[5];
						protocolBuffer[3] = 'R';
						protocolBuffer[4] = 'A';
						protocolBuffer[5] = 14;
						itemp = x>>8;
						itemp2 = x & 0x00ff;
						protocolBuffer[6] = itemp;
						protocolBuffer[7] = itemp2;
						itemp = y>>8;
						itemp2 = y & 0x00ff;				
						protocolBuffer[8] = itemp;
						protocolBuffer[9] =	itemp2;		
						itemp = z>>8;
						itemp2 = z & 0x00ff;		
						protocolBuffer[10] = itemp;
						protocolBuffer[11] = itemp2;	
						checksum = AppProtocolChecksum(protocolBuffer,12);	
						protocolBuffer[12] = checksum; 			
						protocolBuffer[13] = 0x04; 	
	 	
						if (whichRadio == WHICHRADIO_LORA)
						{
							SX1272LoraTransmit(protocolBuffer,14);
						}
						else
						{
							SX1272FskTransmit(protocolBuffer,14);
						}								
						break;
					}			
					case 0x5254:
					{
						//---------RT
						if (buffers[7]<4)
						{
							protocolBuffer[0] = '#';
							protocolBuffer[1] = table0.Item.EepromManDevSerial[4];
							protocolBuffer[2] = table0.Item.EepromManDevSerial[5];
							protocolBuffer[3] = 'R';
							protocolBuffer[4] = 'T';
							protocolBuffer[5] =  33;
							protocolBuffer[6] = buffers[7];	
					 
							offset = buffers[7]*3; 
							protocolBuffer[7] = sensorDynamic[offset].Change;
							for (i=0;i<7;i++)
							{
								protocolBuffer[8+i] = sensorDynamic[offset].LastPacket[i];
							}	
							protocolBuffer[15] = sensorDynamic[offset+1].Change;
							for (i=0;i<7;i++)
							{
								protocolBuffer[16+i] = sensorDynamic[offset+1].LastPacket[i];
							}		
							protocolBuffer[23] = sensorDynamic[offset+2].Change;
							for (i=0;i<7;i++)
							{
								protocolBuffer[24+i] = sensorDynamic[offset+2].LastPacket[i];
							}	
							checksum = AppProtocolChecksum(protocolBuffer,31);	
							protocolBuffer[31] = checksum; 			
							protocolBuffer[32] = 0x04; 																	
											
							if (whichRadio == WHICHRADIO_LORA)
							{
								SX1272LoraTransmit(protocolBuffer,33);
							}
							else
							{
								SX1272FskTransmit(protocolBuffer,33);
							}								
						}
						else
						{
							protocolBuffer[0] = '#';
							protocolBuffer[1] = table0.Item.EepromManDevSerial[4];
							protocolBuffer[2] = table0.Item.EepromManDevSerial[5];
							protocolBuffer[3] = 'R';
							protocolBuffer[4] = 'T';
							protocolBuffer[5] = 9;
							protocolBuffer[6] = buffers[6];
							checksum = AppProtocolChecksum(protocolBuffer,7);	
							protocolBuffer[7] = checksum; 			
							protocolBuffer[8] = 0x04; 		
							if (whichRadio == WHICHRADIO_LORA)
							{
								SX1272LoraTransmit(protocolBuffer,9);
							}
							else
							{
								SX1272FskTransmit(protocolBuffer,9);
							}											
						}
						break;
					}						
			
			
				}		
			}
		}
	}
}
#endif	

void SendOneMessage()
{
	uint8_t checksum,done; 
 
		//-------------------
		// address field set to ffff
		// since in pairing mode.
		//-------------------
		buffer[0] = '#';
		buffer[1] = 0xff;
		buffer[2] = 0xff;
		buffer[3] = 'T';
		buffer[4] = 'T';
		buffer[5] = 8;
		checksum = AppProtocolChecksum(buffer,6);
		buffer[6] = checksum;
		buffer[7] = 0x04;
		if (whichRadio == WHICHRADIO_LORA)
		{
			SX1272LoraTransmit(buffer,8);
		}
		else
		{
			SX1272FskTransmit(buffer,8);
		}
 
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX REMOTE ONLY FUNCTIONS XXXXXXXXXXXXXXXXXXXXXX
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

#if REMOTEBOARD


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function
//============================================================================== 
void RemoteFSKProtocol(void)
{
	uint16_t itemp,itemp2;
	uint32_t ltemp,ltemp2; 
 
 /*
	receiveIntercharTimeout = TRUE; 
	receiveLength = usart_instance.rxBufferCount;
	itemp = general_buffer[3];
	itemp = itemp<<8;
	itemp2 = general_buffer[4];
	itemp |= itemp2; 
	
	txBluetoothBuffer[0]='#';
	if (receiveLength >5)
	{
		switch(itemp)
		{
			case COMMAND_RV:
			{
				txBluetoothBuffer[1] = 0;
				txBluetoothBuffer[2] = 9;
				txBluetoothBuffer[3] = 'r';
				txBluetoothBuffer[4] = 'v';
				txBluetoothBuffer[5] = remoteVersionToReport[0];
				txBluetoothBuffer[6] = remoteVersionToReport[1];
				txBluetoothBuffer[7] = '.';		
				txBluetoothBuffer[8] = remoteVersionToReport[2];
				txBluetoothBuffer[9] = remoteVersionToReport[3];
				txBluetoothBuffer[10] = 0x0d;
				BTTransmit(txBluetoothBuffer,11,TRUE);												
				break;
			}
			case COMMAND_BV:
			{
				txBluetoothBuffer[1] = 0;
				txBluetoothBuffer[2] = 9;
				txBluetoothBuffer[3] = 'b';
				txBluetoothBuffer[4] = 'v';
				txBluetoothBuffer[5] = FWVER3;
				txBluetoothBuffer[6] = FWVER2;
				txBluetoothBuffer[7] = '.';
				txBluetoothBuffer[8] = FWVER1;
				txBluetoothBuffer[9] = FWVER0;
				txBluetoothBuffer[10] = 0x0d;
				BTTransmit(txBluetoothBuffer,11,TRUE);
				break;
			}	
			case COMMAND_DB:
			{
				downloadPacketNumber = general_buffer[5];
				//---------------------------
				// load in download info if this is the first buffer; 
				//----------------------------
				if (downloadPacketNumber == 0)
				{
					//----------build app length;
					ltemp = general_buffer[6]<<24;
					ltemp2 = general_buffer[7]<<16; 
					ltemp |=ltemp2; 
					ltemp2 = general_buffer[8]<<8;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[9];
					ltemp |=ltemp2;										
					brakeApp.appLength = ltemp; 
					//----------build checksum length;
					ltemp = general_buffer[10]<<24;
					ltemp2 = general_buffer[11]<<16;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[12]<<8;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[13];
					ltemp |=ltemp2;
					brakeApp.checksum = ltemp;					
					//----------build checksum start 
					ltemp = general_buffer[14]<<24;
					ltemp2 = general_buffer[15]<<16;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[16]<<8;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[17];
					ltemp |=ltemp2;
					brakeApp.checksumStartOffset = ltemp;	
					//----------build version
					ltemp = general_buffer[18]<<24;
					ltemp2 = general_buffer[19]<<16;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[20]<<8;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[21];
					ltemp |=ltemp2;
					brakeApp.version = ltemp;		
					//----------get packet count
					downloadPacketCount = general_buffer[22];			
					downloadLastPacketNumber = 0;
					downloadNextPacketNumber = 1;
					downloadLength = 0;		
					remoteApp.appLength = 0;
					remoteApp.checksum = 0;
					remoteApp.checksumStartOffset = 0; 					
				}
				else
				{
					itemp = general_buffer[1];
					itemp = itemp<<8;
					itemp2 = general_buffer[2];
					itemp |= itemp2;
					itemp = itemp - 7;
					if (downloadPacketNumber == downloadNextPacketNumber)
					{
						if (downloadPacketNumber < (downloadPacketCount+1))
						{
							ProgramMemory(APP_SCRATCH_BASE + downloadLength,&general_buffer[6],itemp);
						}
						if (downloadPacketNumber == downloadPacketCount)
						{
							schedByte |= SCHEDBYTE_DOWNLOAD_DONE; 
						}
						downloadLength+=itemp; 
						downloadNextPacketNumber++;
						
					}	
				}
				txBluetoothBuffer[1] = 0;
				txBluetoothBuffer[2] = 9;
				txBluetoothBuffer[3] = 'd';
				txBluetoothBuffer[4] = 'b';
				txBluetoothBuffer[5] = FWVER3;
				txBluetoothBuffer[6] = FWVER2;
				txBluetoothBuffer[7] = downloadPacketNumber;
				txBluetoothBuffer[8] = downloadPacketCount; 
				txBluetoothBuffer[9] = 0x00;
				txBluetoothBuffer[10] = 0x0d;
				BTTransmit(txBluetoothBuffer,11,TRUE);
				break;
			}			
					
		}		
	}
	BTReceive();    
*/	    
}

#endif


#if REMOTEBOARD		
uint8_t brakesEEPROMData[6];
uint16_t brakesAccelData[3];
uint8_t brakesTireData[MAXSENSORS][8];
uint8_t brakesVersionDate[10];
AppInfo remoteDownloadApp;
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function handles when the remote receives a packet
//==============================================================================
void AppProtocolRemote(uint8_t *buffers)
{
	uint16_t command,itemp,itemp2,testBuffer[2],doffset;
	uint8_t length,i,offset,match,goodmsg;
	uint32_t ltemp,ltemp2; 
	struct nvm_config config;
	
	if ((buffers[1] == '#')&&(buffers[0]>3))
	{
		match = FALSE;
		if((buffers[2] == table0.Item.PairAddress[0])&&
		(buffers[3] == table0.Item.PairAddress[1]))
		{
			match = TRUE;
		}	 
	
		//----------------
		// build the command
		itemp = buffers[4];
		itemp2 = buffers[5];
		itemp = itemp<<8;
		itemp |= itemp2;
		command = itemp;
		//----------------
		// get the length
		length = buffers[0];
	
		goodmsg = AppProtocolChecksum(&buffers[1],length-2);	
		if (goodmsg == buffers[length-1])
		{
			goodmsg = TRUE; 
		}
		else
		{
			goodmsg = FALSE; 
		}	
		if (((buffers[1] == '#')&&(length>3))&&
			(goodmsg == TRUE))
		{
			switch (command)
			{
				case 0x4452:  //DR 
				{
	//uint8_t remoteDownloadPacketNumber; 
	//uint8_t remoteDownloadPacketCount; 
	//uint8_t remoteDownloadNextPacketNumber;
	//uint8_t remoteDownloadLastPacketNumber; 
	//uint32_t remoteDownloadLength;  				
					if ((length >6)&&(match==TRUE))
					{
						AppProtocolClearCommErrors();
						//----------------
						// build the offset
						itemp = buffers[7];
						itemp2 = buffers[8];
						itemp = itemp<<8;
						itemp |= itemp2;
						doffset = itemp;
						if (doffset == 0)
						{
							ltemp = buffers[9];
							ltemp2 = buffers[10];
							ltemp = ltemp<<24;
							ltemp2 = ltemp2<<16;
							ltemp |= ltemp2;	
							ltemp2 = buffers[11];
							ltemp2 = ltemp2<<8;
							ltemp |= ltemp2;	
							ltemp2 = buffers[12];
							ltemp |= ltemp2;	
							remoteDownloadLength = ltemp; 
							if (remoteDownloadLength == 0)
							{
								remoteDownloadPacketCount = 0;
								remoteDownloadPacketNumber = 0; 
								remoteDownloadLastPacketNumber = 0;
								remoteDownloadNextPacketNumber = 0; 
								remoteDownloadLength = 0; 
								remoteDownloadApp.appLength = 0;
								remoteDownloadApp.checksum = 0;
								remoteDownloadApp.version = 0;
								remoteDownloadApp.filetype1 = 0;
								remoteDownloadApp.checksumStartOffset = 0; 
	//							downloadTime = FALSE; 
							}		
							else
							{

	//							downloadTime = TRUE; 
								remoteDownloadPacketCount = 0;
								remoteDownloadPacketNumber = 0; 
								remoteDownloadLastPacketNumber = 0;
								remoteDownloadNextPacketNumber = 0; 
								remoteDownloadApp.appLength = remoteDownloadLength;
								remoteDownloadLength = 0; 
								//-----------checksum
								ltemp = buffers[13];
								ltemp2 = buffers[14];
								ltemp = ltemp<<24;
								ltemp2 = ltemp2<<16;
								ltemp |= ltemp2;	
								ltemp2 = buffers[15];
								ltemp2 = ltemp2<<8;
								ltemp |= ltemp2;	
								ltemp2 = buffers[16];
								ltemp |= ltemp2;	
								remoteDownloadApp.checksum = ltemp; 		
								//-----------checksum start
								ltemp = buffers[17];
								ltemp2 = buffers[18];
								ltemp = ltemp<<24;
								ltemp2 = ltemp2<<16;
								ltemp |= ltemp2;	
								ltemp2 = buffers[18];
								ltemp2 = ltemp2<<8;
								ltemp |= ltemp2;	
								ltemp2 = buffers[20];
								ltemp |= ltemp2;	
								remoteDownloadApp.checksumStartOffset = ltemp; 								
								//-----------version
								ltemp = buffers[21];
								ltemp2 = buffers[22];
								ltemp = ltemp<<24;
								ltemp2 = ltemp2<<16;
								ltemp |= ltemp2;	
								ltemp2 = buffers[23];
								ltemp2 = ltemp2<<8;
								ltemp |= ltemp2;	
								ltemp2 = buffers[24];
								ltemp |= ltemp2;	
								remoteDownloadApp.version = ltemp; 		
	//							AppScreenInit(SCREEN_DOWNLOADREQUEST); 																
							}			
						} 	
						else
						{
													
							schedByte |= SCHEDBYTE_COMMTOBRAKE;
							if (doffset == remoteDownloadNextPacketNumber)
							{
								PlaceDownloadPacket(doffset); 
								schedByte |= SCHEDBYTE_COMMTOBRAKE;
								downloadTime = TRUE; 
								remoteDownloadLastPacketNumber = remoteDownloadNextPacketNumber; 
								itemp = buffers[6];
								itemp = itemp - 10;
	//							if (downloadPacketNumber == downloadNextPacketNumber)
	//							{
									if (doffset < (remoteDownloadPacketCount+1))
									{
										ProgramMemory(APP_SCRATCH_BASE + remoteDownloadLength,&buffers[9],itemp);
										remoteDownloadLength+=itemp; 
										remoteDownloadNextPacketNumber++;
									}
									else
									{
										doffset = 0; //put something here 
										remoteDownloadNextPacketNumber = 0xffff; 
										downloadTime = FALSE; 
										AppScreenInit(SCREEN_HOME);
										schedByte |= SCHEDBYTE_DOWNLOAD_DONE; 
									}

						
	//							}								
							}
							else
							{
							
							}
						}	
					}
					break;				
				}
				case MSG_SW:
				{
					if ((length >6)&&(match==TRUE))
					{
						AppProtocolClearCommErrors();
						if ((buffers[7] == WHICHRADIO_LORA)||(buffers[7]==WHICHRADIO_FSK))
						{
							whichRadio = buffers[7];
						}
						else
						{
							whichRadio = WHICHRADIO_LORA;
						}
					 
						CommInit();
						eventMessageReceived = 1; 
						AppScreenUpdateHome();					
					}
					break;
				}			
				case MSG_FV:
				{
					if ((length >9)&&(match==TRUE))
					{
						AppProtocolClearCommErrors();					
						//---------FV
						for (i=0;i<10;i++)
						{
							brakesVersionDate[i] = buffers[7+i];
						}
					}
					break;
				}			
				case MSG_STATUS:
				{
					if ((length >7)&&(match==TRUE))
					{
						AppProtocolClearCommErrors();										
						//---------BS
						statusBrake.TireRadio = buffers[7];
						statusBrake.ExtRadio = buffers[8];
						statusBrake.EEPROM = buffers[9];
						statusBrake.Accelerometer = buffers[10];
						brakeStatus.VoltageInput = buffers[11];
						brakeStatus.AccelerometerStatus = buffers[12];
						brakeStatus.ActuatorStatus = buffers[13]; 
						brakeStatus.BrakeState = buffers[14];
						brakeStatus.VoltageSupercap = buffers[15]; 		
						brakeState = buffers[16]; 
				 		AppHandleRadioSwitch(buffers[17]); 
						if ((brakeState == BRAKESTATE_ACTIVE_EXTEND_BREAKAWAY)||
						   (brakeState ==BRAKESTATE_ACTIVE_HOLD_BREAKAWAY))	
						{
							SpeakerOn();
						}
						else
						{
							SpeakerOff();
						}		
						itemp = buffers[18];
						itemp = itemp<<8;
						itemp2 = buffers[19];
						itemp2 |= itemp;
						gPrime = itemp2; 		

					}
					break;
				}			
				case 0x5245:
				{
					if ((length >9)&&(match==TRUE))
					{
						AppProtocolClearCommErrors();										
						//---------RE
						for (i=0;i<6;i++)
						{
							brakesEEPROMData[i] = buffers[7+i];
						}
					}
					break;
				}
				case MSG_PR:  //PR
				{
					if (AppPairingActive()==TRUE)
					{
						//-----------------------------------
						//pairing 
						testBuffer[0] = buffers[2];
						I2CEEPROMBufferWrite((uint8_t*)testBuffer,PairAddressMSB,1);
						table0.Item.PairAddress[0] = buffers[2]; 
						testBuffer[0] = buffers[3];
						I2CEEPROMBufferWrite((uint8_t*)testBuffer,PairAddressLSB,1);		
						table0.Item.PairAddress[1] = buffers[3]; 
						SetPairActiveOff();		
					}
					break;
				}
				case 0x5241:
				{
					if (match==TRUE)
					{
						AppProtocolClearCommErrors();												
						//---------RA
						for (i=0;i<3;i++)
						{
							itemp = buffers[7+(2*i)];
							itemp2 = buffers[7+(2*i)+1];
							itemp = itemp<<8;
							itemp &= 0xff00;
							itemp2 &= 0x00ff; 
							itemp |= itemp2; 
							brakesAccelData[i] = itemp;
						}
					}
					break;
				}
				case 0x5254:
				{
					//---------RT
					if ((length >10)&&(match == TRUE))
					{
						AppProtocolClearCommErrors();										
						if (buffers[7] <4)
						{
							offset = buffers[7]*3; 
							for (i=0;i<8;i++)
							{
								brakesTireData[offset][i] = buffers[8+i];
							}
							for (i=0;i<8;i++)
							{
								brakesTireData[offset+1][i] = buffers[16+i];
							}		
							for (i=0;i<8;i++)
							{
								brakesTireData[offset+2][i] = buffers[24+i];
							}										
						}
					}
					break;
				}			
			}
			eventMessageReceived = 1; 
			AppScreenUpdateHome();
		}
	}
}
#endif 
#if REMOTEBOARD

uint8_t downloadTime = FALSE; 
uint8_t DownloadActiveRequest(void)
{
	return downloadTime;
}


uint8_t DownloadActive(void)
{
	if (appScreen == SCREEN_DOWNLOADING)
	{
		return TRUE;
	}
	else
	{
		return FALSE; 
	}
	 
}

void DownloadStart(void)
{
	uint32_t ltemp;
	struct nvm_config config;	
	remoteDownloadNextPacketNumber = 1; 
	//--------------------
	// packet count calculation
	ltemp = remoteDownloadApp.appLength;
	ltemp = ltemp/128;
	ltemp++;
	remoteDownloadPacketCount = ltemp;
							 
	nvm_get_config_defaults(&config);
	nvm_set_config(&config);	
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void BrakeCommTask(void)
{
	uint8_t temp,checksum,done,whichWay;
	uint16_t itemp,itemp2; 
	 
	if ((setCW == 0)&&(setTXContinuous==0)&&(setRXContinuous==0))
	{
		done = FALSE; 
		buffer[0] = '#';
		if ((DownloadActive()==TRUE)&&(done == FALSE))
		{
			 
			//-------------------
			// address field set to ffff
			// since in pairing mode.
			//-------------------
			buffer[1] = table0.Item.PairAddress[0];
			buffer[2] = table0.Item.PairAddress[1];
			buffer[3] = 'D';
			buffer[4] = 'R';
			buffer[5] = 10;
			
			buffer[6] = 0;
			buffer[7] = 0;
			if (remoteDownloadNextPacketNumber != 0)
			{
				itemp = remoteDownloadNextPacketNumber>>8;
				itemp2 = remoteDownloadNextPacketNumber & 0xff;
				buffer[6] = itemp;
				buffer[7] = itemp2;
			}
			checksum = AppProtocolChecksum(buffer,8);
			buffer[8] = checksum;
			buffer[9] = 0x04;
			if (whichRadio == WHICHRADIO_LORA)
			{
				SX1272LoraTransmit(buffer,10);
			}
			else
			{
				SX1272FskTransmit(buffer,10);
			}
			done = TRUE;
			
		}		
		if ((DownloadActiveRequest()==TRUE)&&(done == FALSE))  
		{
			downloadTime = FALSE; 
			//-------------------
			// address field set to ffff 
			// since in pairing mode. 
			//-------------------
			buffer[1] = table0.Item.PairAddress[0];
			buffer[2] = table0.Item.PairAddress[1];
			buffer[3] = 'D';
			buffer[4] = 'R';
			buffer[5] = 10;
			
			buffer[6] = 0;
			buffer[7] = 0;
//			if (remoteDownloadNextPacketNumber != 0)
//			{
//				itemp = remoteDownloadNextPacketNumber>>8;
//				itemp2 = remoteDownloadNextPacketNumber & 0xff; 
//				buffer[6] = itemp;
//				buffer[7] = itemp2; 	
//				downloadTime = TRUE; 		
//			}
			checksum = AppProtocolChecksum(buffer,8);	
			buffer[8] = checksum; 			
			buffer[9] = 0x04;	
			if (whichRadio == WHICHRADIO_LORA)
			{
				SX1272LoraTransmit(buffer,10);
			}
			else
			{
				SX1272FskTransmit(buffer,10);
			}
			done = TRUE; 
			
		}		
		if ((AppPairingActive()==TRUE)&&(done == FALSE))  
		{
			//-------------------
			// address field set to ffff 
			// since in pairing mode. 
			//-------------------
			buffer[1] = 0xff;
			buffer[2] = 0xff;  	
			buffer[3] = 'P';
			buffer[4] = 'M';
			buffer[5] = 8;
			checksum = AppProtocolChecksum(buffer,6);	
			buffer[6] = checksum; 			
			buffer[7] = 0x04;	
			if (whichRadio == WHICHRADIO_LORA)
			{
				SX1272LoraTransmit(buffer,8);
			}
			else
			{
				SX1272FskTransmit(buffer,8);
			}
			done = TRUE; 
		}
		if ((SwitchRadio(&whichWay)==TRUE)&&(done == FALSE)) 
		{
			//-------------------
			// address field set to ffff 
			// since in pairing mode. 
			//-------------------
			buffer[1] = table0.Item.PairAddress[0];
			buffer[2] = table0.Item.PairAddress[1];
			buffer[3] = 'S';
			buffer[4] = 'W';
			buffer[5] = 9;
			buffer[6] = whichWay;
			checksum = AppProtocolChecksum(buffer,7);	
			buffer[7] = checksum; 			
			buffer[8] = 0x04;	
			if (whichRadio == WHICHRADIO_LORA)
			{
				SX1272LoraTransmit(buffer,9);
			}
			else
			{
				SX1272FskTransmit(buffer,9);
			}
			done = TRUE; 
		}	
		if (done == FALSE)
		{
			buffer[1] = table0.Item.PairAddress[0];
			buffer[2] = table0.Item.PairAddress[1];
			buffer[3] = 'R';
			buffer[4] = 'E';
			buffer[5] = 0x00; //length
			buffer[6] = 0x00; //control byte
			//---------------------
			// check which screens you are in. 
			//
			if ((appScreen != SCREEN_MISC)&&(appScreen != SCREEN_SENSOR))
			{
				if (brake_pressed != 0)
				{
					buffer[6] |= REMOTE_MANUALBRAKE_ACTIVE; 
				}
			}
			if ((appScreen == SCREEN_HOME)&&(appScreenHomeType == SCREEN_HOME_BREAKAWAYTIP))
			{
				if (breakaway_pressed != 0)
				{
					buffer[6] |= REMOTE_CLEARBREAKAWAY; 
				}
			}
			buffer[7] = 0x00;
			//----------------load in nibble of max force
			buffer[7] |= (table0.Item.MaxForce & 0x0f);
			temp = table0.Item.ForceMaxSet;
			temp = temp<<4; 
			buffer[7] |= temp; 
		//-------------------------------------------	
			buffer[8] = 0x00;
			if (table0.Item.ActiveBrakeEnable != FALSE)
			{
				buffer[8] |= REMOTE_ACTIVEBRAKEENABLE; 
			}
			if (table0.Item.TPMSEnable != FALSE)
			{
				buffer[8] |= REMOTE_TPMSENABLE; 
			}	
			//----------------load in nibble of sensitivity
			buffer[8] |= ((table0.Item.SensitivitySet & 0x0f)<<4);
		//--------------------------------------------	
			buffer[9] = 0x00;  //checksum
			buffer[10] = 0x04;
			brakeMessageOffset++;
			if (brakeMessageOffset >= MAX_BRAKE_MESSAGE)
			{
				brakeMessageOffset = 0;
			}
			switch (brakeMessageOffset)
			{
				case 0:
				{
					brakeMessageSystemOffset++;
					if (brakeMessageSystemOffset >= MAX_BRAKE_SYSTEM_MESSAGE)
					{
						brakeMessageSystemOffset = 0;
					}
					switch (brakeMessageSystemOffset)
					{
						case 0:
						{
							buffer[3] = 'R';
							buffer[4] = 'E';
							buffer[5] = 11; //length
							checksum = AppProtocolChecksum(buffer,9);	
							buffer[9] = checksum; 			
							if (whichRadio == WHICHRADIO_LORA)
							{
								SX1272LoraTransmit(buffer,11);
							}
							else
							{
								SX1272FskTransmit(buffer,11);
							}
							break;						
						}
						case 1:
						{
							buffer[3] = 'F';
							buffer[4] = 'M';
							buffer[5] = 11; //length
							checksum = AppProtocolChecksum(buffer,9);	
							buffer[9] = checksum; 			
							if (whichRadio == WHICHRADIO_LORA)
							{
								SX1272LoraTransmit(buffer,11);
							}
							else
							{
								SX1272FskTransmit(buffer,11);
							}
							break;
						}	
						case 2:
						{
							buffer[3] = 'R';
							buffer[4] = 'A';
							buffer[5] = 11; //length
							checksum = AppProtocolChecksum(buffer,9);	
							buffer[9] = checksum; 			
							if (whichRadio == WHICHRADIO_LORA)
							{
								SX1272LoraTransmit(buffer,11);
							}
							else
							{
								SX1272FskTransmit(buffer,11);
							}
							break;
						}
/*  ---- prior removing tire pressure stuff 						
						case 3:
						{
							buffer[3] = 'R';
							buffer[4] = 'T';
							buffer[5] = 9; //length
							buffer[6] = 0; 
							checksum = AppProtocolChecksum(buffer,7);	
							buffer[7] = checksum; 			
							buffer[8] = 0x04;
							if (whichRadio == WHICHRADIO_LORA)
							{
								SX1272LoraTransmit(buffer,9);
							}
							else
							{
								SX1272FskTransmit(buffer,9);
							}
							break;
						}
						case 4:
						{
							buffer[3] = 'R';
							buffer[4] = 'T';
							buffer[5] = 9; //length
							buffer[6] =  1;
							checksum = AppProtocolChecksum(buffer,7);	
							buffer[7] = checksum; 			
							buffer[8] = 0x04;
							if (whichRadio == WHICHRADIO_LORA)
							{
								SX1272LoraTransmit(buffer,9);
							}
							else
							{
								SX1272FskTransmit(buffer,9);
							}
							break;
						}					
						case 5:
						{
							buffer[3] = 'R';
							buffer[4] = 'T';
							buffer[5] = 9; //length
							buffer[6] =  2;
							checksum = AppProtocolChecksum(buffer,7);	
							buffer[7] = checksum; 			
							buffer[8] = 0x04;
							if (whichRadio == WHICHRADIO_LORA)
							{
								SX1272LoraTransmit(buffer,9);
							}
							else
							{
								SX1272FskTransmit(buffer,9);
							}
							break;
						}					
*/										
					}
					break;
				}

				case 1:
				{
					buffer[3] = 'B';
					buffer[4] = 'M';
					buffer[5] = 15; 
					
					buffer[9] = FWVER3;
					buffer[10] = FWVER2;
					buffer[11] = FWVER1;
					buffer[12] = FWVER0;					
					checksum = AppProtocolChecksum(buffer,13);	
					buffer[13] = checksum; 			
					if (whichRadio == WHICHRADIO_LORA)
					{
						SX1272LoraTransmit(buffer,15);
					}
					else
					{
						SX1272FskTransmit(buffer,15);
					}
					break;
				}

			}
		}
	}
}
#endif


