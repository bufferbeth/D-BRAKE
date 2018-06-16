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
#include "appProtocol.h"

#if BRAKEBOARD
//---------------------GLOBAL VARIABLES----------------------------------- 
#define EDBG_CDC_MODULE              SERCOM2
#define EDBG_CDC_SERCOM_MUX_SETTING  USART_RX_3_TX_2_XCK_3
#define EDBG_CDC_SERCOM_PINMUX_PAD0  PINMUX_UNUSED
#define EDBG_CDC_SERCOM_PINMUX_PAD1  PINMUX_UNUSED
#define EDBG_CDC_SERCOM_PINMUX_PAD2  PINMUX_PA10D_SERCOM2_PAD2
#define EDBG_CDC_SERCOM_PINMUX_PAD3  PINMUX_PA11D_SERCOM2_PAD3

AppInfo brakeApp;
AppInfo remoteApp;
//---------------------LOCAL VARIABLES------------------------------------
 
struct usart_module usart_instance;
 
#define MAX_RX_BUFFER_LENGTH   5
volatile uint8_t rx_buffer[MAX_RX_BUFFER_LENGTH];
 
#define MAX_TX_BUFFER_LENGTH   40
uint8_t txBluetoothBuffer[MAX_TX_BUFFER_LENGTH]; 
 
uint16_t receiveLength;  

uint8_t downloadPacketNumber; 
uint8_t downloadPacketCount; 

uint8_t downloadNextPacketNumber;
uint8_t downloadLastPacketNumber; 
uint32_t downloadLength; 

 
//---------------------LOCAL FUNCTION PROTOTYPES--------------------------   
 
void usart_read_callback(const struct usart_module *const usart_module);
void usart_write_callback(const struct usart_module *const usart_module);
void configure_usart(void);
void configure_usart_callbacks(void);
void UsartMain(void);
void BTReceive(void);
void BTTransmit(uint8_t *buffer,uint8_t length,uint8_t state);
 
extern uint8_t receiveIntercharTimeout;
 
#define COMMAND_RV 0x5256
#define COMMAND_BV 0x4256
#define COMMAND_DB 0x4442
#define COMMAND_DR 0x4452
#define COMMAND_DC 0x4443   //second generation files - brake
#define COMMAND_DS 0x4453	//second generation files - remote

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function
//============================================================================== 
void usart_read_callback(const struct usart_module *const usart_module)
{
	uint16_t itemp,itemp2;
	uint32_t ltemp,ltemp2; 
	 
//	usart_write_buffer_job(&usart_instance,
//			(uint8_t *)rx_buffer, MAX_RX_BUFFER_LENGTH);
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
			case COMMAND_DR:
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
					remoteApp.appLength = ltemp; 
					//----------build checksum length;
					ltemp = general_buffer[10]<<24;
					ltemp2 = general_buffer[11]<<16;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[12]<<8;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[13];
					ltemp |=ltemp2;
					remoteApp.checksum = ltemp;					
					//----------build checksum start 
					ltemp = general_buffer[14]<<24;
					ltemp2 = general_buffer[15]<<16;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[16]<<8;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[17];
					ltemp |=ltemp2;
					remoteApp.checksumStartOffset = ltemp;	
					//----------build version
					ltemp = general_buffer[18]<<24;
					ltemp2 = general_buffer[19]<<16;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[20]<<8;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[21];
					ltemp |=ltemp2;
					remoteApp.version = ltemp;		
					//----------get packet count
					downloadPacketCount = general_buffer[22];			
					downloadLastPacketNumber = 0;
					downloadNextPacketNumber = 1;
					downloadLength = 0;		
					brakeApp.appLength = 0;
					brakeApp.checksum = 0;
					brakeApp.checksumStartOffset = 0; 					
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
				txBluetoothBuffer[4] = 'r';
				txBluetoothBuffer[5] = FWVER3;
				txBluetoothBuffer[6] = FWVER2;
				txBluetoothBuffer[7] = downloadPacketNumber;
				txBluetoothBuffer[8] = downloadPacketCount; 
				txBluetoothBuffer[9] = 0x00;
				txBluetoothBuffer[10] = 0x0d;
				BTTransmit(txBluetoothBuffer,11,TRUE);
				break;
			}		
//----------------------- second generation 
			case COMMAND_DC:
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
				txBluetoothBuffer[4] = 'c';
				txBluetoothBuffer[5] = FWVER3;
				txBluetoothBuffer[6] = FWVER2;
				txBluetoothBuffer[7] = downloadPacketNumber;
				txBluetoothBuffer[8] = downloadPacketCount;
				txBluetoothBuffer[9] = 0x00;
				txBluetoothBuffer[10] = 0x0d;
				BTTransmit(txBluetoothBuffer,11,TRUE);
				break;
			}
			case COMMAND_DS:
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
					remoteApp.appLength = ltemp;
					//----------build checksum length;
					ltemp = general_buffer[10]<<24;
					ltemp2 = general_buffer[11]<<16;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[12]<<8;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[13];
					ltemp |=ltemp2;
					remoteApp.checksum = ltemp;
					//----------build checksum start
					ltemp = general_buffer[14]<<24;
					ltemp2 = general_buffer[15]<<16;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[16]<<8;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[17];
					ltemp |=ltemp2;
					remoteApp.checksumStartOffset = ltemp;
					//----------build version
					ltemp = general_buffer[18]<<24;
					ltemp2 = general_buffer[19]<<16;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[20]<<8;
					ltemp |=ltemp2;
					ltemp2 = general_buffer[21];
					ltemp |=ltemp2;
					remoteApp.version = ltemp;
					//----------get packet count
					downloadPacketCount = general_buffer[22];
					downloadLastPacketNumber = 0;
					downloadNextPacketNumber = 1;
					downloadLength = 0;
					brakeApp.appLength = 0;
					brakeApp.checksum = 0;
					brakeApp.checksumStartOffset = 0;
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
				txBluetoothBuffer[4] = 's';
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
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void usart_write_callback(const struct usart_module *const usart_module)
{
	
    general_buffer[0] = 0x01; 
    general_buffer[1] = 0x29; 
    general_buffer[2] = 0xFC; 
    general_buffer[3] = 0x03; 
    general_buffer[4] = 0x00;
    general_buffer[5] = 0x00; 
    general_buffer[6] = 0x06; 
/*	
    general_buffer[0] = 0x31;
    general_buffer[1] = 0x32;
    general_buffer[2] = 0x33;
    general_buffer[3] = 0x34;
    general_buffer[4] = 0x35;
    general_buffer[5] = 0x36;
    general_buffer[6] = 0x37;
*/		
//	usart_read_buffer_job(&usart_instance,(uint8_t *)general_buffer,3);
//	usart_write_buffer_job(&usart_instance, general_buffer,7); 
}
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//============================================================================== 
void configure_usart(void)
{
	struct usart_config config_usart;
 
	usart_get_config_defaults(&config_usart);
 
	config_usart.baudrate    = 115200;
	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	config_usart.generator_source = GCLK_GENERATOR_3; 
 
	while (usart_init(&usart_instance,
			EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
	}
 
	usart_enable(&usart_instance);
 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void USARTDisable(void)
{
	usart_disable(&usart_instance);	
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void USARTEnable(void)
{
	usart_enable(&usart_instance);
}

void configure_usart_callbacks(void)
{
//! [setup_register_callbacks]
	usart_register_callback(&usart_instance,
			usart_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&usart_instance,
			usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
//! [setup_register_callbacks]

//! [setup_enable_callbacks]
	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_RECEIVED);
//! [setup_enable_callbacks]
}
//! [setup]

void UsartMain(void)
{
	configure_usart();
	configure_usart_callbacks();
}

void BTReceive(void)
{
	usart_read_buffer_job(&usart_instance,(uint8_t *)general_buffer,5);
	receiveIntercharTimeout = FALSE;        
}

void BTTransmit(uint8_t *buffer,uint8_t length,uint8_t state)
{
	usart_write_buffer_job(&usart_instance, buffer, length); 
	
}

#endif 


