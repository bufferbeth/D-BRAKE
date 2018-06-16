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
#include "appBluetooth.h"

#if BRAKEBOARD
//---------------------GLOBAL VARIABLES-----------------------------------


uint8_t testBuffer[35]; 
#define MAX_GENERAL_BUFFER 1040
uint8_t general_buffer[MAX_GENERAL_BUFFER];
uint8_t receiveIntercharTimeout;
uint8_t response_received=FALSE;
uint8_t rx_timeout = FALSE; 
uint8_t bluetoothAwake = 0; 
 
//---------------------LOCAL VARIABLES------------------------------------


//--------------------------
//button check histories
 
#define MODULEEEPROMSIZE 0x420
uint8_t moduleEEPROM[MODULEEEPROMSIZE];

//---------------------LOCAL FUNCTION PROTOTYPES--------------------------

extern uint16_t blockingTime;
void BTReceive(void);
void BTTransmit(uint8_t *buffer,uint8_t length,uint8_t state);
 
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: BlockingTimer
//------------------------------------------------------------------------------
//  
//==============================================================================
void BlockingTimer(uint16_t count)
{
	blockingTime = count; 
	while (blockingTime >0);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: BluetoothWakeUp
//------------------------------------------------------------------------------
//  
//==============================================================================
uint8_t BluetoothWakeUp(void)
{
 
	struct port_config pin_conf;
	
	port_get_config_defaults(&pin_conf);	
 
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(BLUETOOTH_RESET, &pin_conf);
	port_pin_set_output_level(BLUETOOTH_RESET, false);
	port_pin_set_config(BLUETOOTH_PROG, &pin_conf);
	port_pin_set_output_level(BLUETOOTH_PROG, true);   
	BlockingTimer(40);
	 
	port_pin_set_output_level(BLUETOOTH_RESET, true);
	BlockingTimer(40);
	port_pin_set_output_level(BLUETOOTH_PROG, true);
	BlockingTimer(20);
	bluetoothAwake = 1; 
	brakeBlueLED = BRAKEBLUELED_SOLID;
	brakeBiLED = BRAKEBILED_GREENSOLID;
	
//while(1)
//{	
	BTReceive();
//    receiveIntercharTimeout = FALSE;        
//    while ((response_received == FALSE)&&(rx_timeout == FALSE)&&(receiveIntercharTimeout == FALSE));   
//}
	 
	
}
	
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: BluetoothSleep
//------------------------------------------------------------------------------
//
//==============================================================================
uint8_t BluetoothSleep(void)
{
	
	struct port_config pin_conf;
	
	port_get_config_defaults(&pin_conf);
	
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(BLUETOOTH_RESET, &pin_conf);
	port_pin_set_output_level(BLUETOOTH_RESET, false);
	port_pin_set_config(BLUETOOTH_PROG, &pin_conf);
	port_pin_set_output_level(BLUETOOTH_PROG, true);
	BlockingTimer(40);
	
	bluetoothAwake = 0;
	 brakeBlueLED = BRAKEBLUELED_OFF;
	 brakeBiLED = BRAKEBILED_OFF;
}	
	
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: BluetoothMicrochipConfig
//------------------------------------------------------------------------------
//  
//==============================================================================
uint8_t BluetoothMicrochipConfig(void)
{
	
	uint8_t ID[4],temp,temp2;  
	uint16_t  i,loffset,ltemp;
	uint8_t done,offset,whichModule;
	struct port_config pin_conf;
	 
	
	port_get_config_defaults(&pin_conf);
  
//  if (ReadSetting(BT_First_Setting,TRUE) != BLUETOOTH_UCHIP_SETTING)
//  {     
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(BLUETOOTH_RESET, &pin_conf);
	port_pin_set_output_level(BLUETOOTH_RESET, false);
	port_pin_set_config(BLUETOOTH_PROG, &pin_conf);
	port_pin_set_output_level(BLUETOOTH_PROG, true);  //false);
		
//    usartMode = USART_MODE_EEPROM; 
    //----------------------
    // reset the devcie. 
    //----------------------
	port_pin_set_output_level(BLUETOOTH_RESET, false);
    BlockingTimer(40);
 //   PanasonicRXQueueReset();
 //   BTReceiveStart();
	port_pin_set_output_level(BLUETOOTH_RESET, true);  
    BlockingTimer(40);    
	port_pin_set_output_level(BLUETOOTH_PROG, true);  
    BlockingTimer(200);    
 	
 
/* 
    testBuffer[0] = 0x01; 
    testBuffer[1] = 0x03; 
    testBuffer[2] = 0x0C; 
    testBuffer[3] = 0x00; 
    testBuffer[4] = 0x00; 
    testBuffer[5] = 0xfd; 
	*/
    testBuffer[0] = 0xaa;
    testBuffer[1] = 0x00;
    testBuffer[2] = 0x01;
    testBuffer[3] = 0x01;
    testBuffer[4] = 0xfe;
    testBuffer[5] = 0xfd;	
while (1)
{	
	BTReceive();
    BTTransmit(testBuffer,5,TRUE); 
/*
    testBuffer[0] = 0xaa; 
    testBuffer[1] = 0x00; 
    testBuffer[2] = 0x02; 
    testBuffer[3] = 0x01; 
    testBuffer[4] = 0x00; 
    testBuffer[5] = 0xfd; 
	BTReceive();
    BTTransmit(testBuffer,6,TRUE,general_buffer);  	 
*/	
    receiveIntercharTimeout = FALSE;        
    while ((response_received == FALSE)&&(rx_timeout == FALSE)&&(receiveIntercharTimeout == FALSE));   
}
    BlockingTimer(30);
 
    testBuffer[0] = 0x01; 
    testBuffer[1] = 0x2D; 
    testBuffer[2] = 0xFC; 
    testBuffer[3] = 0x01; 
    testBuffer[4] = 0x08; 
	BTReceive();
    BTTransmit(testBuffer,5,TRUE);  
    receiveIntercharTimeout = FALSE;        
    while ((response_received == FALSE)&&(rx_timeout == FALSE)&&(receiveIntercharTimeout == FALSE));  
    BlockingTimer(30);    
 
    //-------------reqeust BD ADDRESS 
    //-----------------------------------
    testBuffer[0] = 0x01; 
    testBuffer[1] = 0x29; 
    testBuffer[2] = 0xFC; 
    testBuffer[3] = 0x03; 
    testBuffer[4] = 0x00;
    testBuffer[5] = 0x00; 
    testBuffer[6] = 0x06; 
    BTTransmit(testBuffer,7,TRUE);  
    receiveIntercharTimeout = FALSE;        
    while ((response_received == FALSE)&&(rx_timeout == FALSE)&&(receiveIntercharTimeout == FALSE));      
    BlockingTimer(30); 
    done = FALSE; 
    i = 0; 
    while ((done == FALSE)&&(i<10))
    {
      if ((general_buffer[i]== 0xFC)&&(general_buffer[i+1]==0x00)&&(general_buffer[i+2]==0x00)&&
          (general_buffer[i+3] == 0x00)&&(general_buffer[i+4] == 0x06))
      {
        done = TRUE; 
      }
      i++;
    }  
    if (done == TRUE)
    {
      offset = i+4;
      temp = general_buffer[offset];
      temp2 = temp; 
      temp = temp>>4;
      temp2 &= 0x0f; 
      if (temp>9)
      {
        temp += (0x41-10);
      }
      else
      {
        temp |= 0x30;
      }
      ID[0] = temp; 
      if (temp2>9)
      {
        temp2 += (0x41-10);
      }
      else
      {
        temp2 |= 0x30;
      }
      ID[1] = temp2;     
       
      temp = general_buffer[offset+1];
      temp2 = temp; 
      temp = temp>>4;
      temp2 &= 0x0f; 
      if (temp>9)
      {
        temp += (0x41-10);
      }
      else
      {
        temp |= 0x30;
      }
      ID[2] = temp; 
      if (temp2>9)
      {
        temp2 += (0x41-10);
      }
      else
      {
        temp2 |= 0x30;
      }
      ID[3] = temp2;     
      
//      table0.Item.BluetoothID4 = ID[3];
//      table0.Item.BluetoothID3 = ID[2];
//      table0.Item.BluetoothID2 = ID[1];
//      table0.Item.BluetoothID1 = ID[0]; 
//      WriteSetting(BluetoothID4_Setting, table0.Item.BluetoothID4,TRUE); 
//      WriteSetting(BluetoothID3_Setting, table0.Item.BluetoothID3,TRUE); 
//      WriteSetting(BluetoothID2_Setting, table0.Item.BluetoothID2,TRUE); 
//      WriteSetting(BluetoothID1_Setting, table0.Item.BluetoothID1,TRUE);         
    }        
#if 0     
   //------------------------ READ location 0x0323
    // if 0x38  for the BM78 to the below. 
    //-------------------------------------
    PanasonicRXQueueReset();
    testBuffer[0] = 0x01; 
    testBuffer[1] = 0x29; 
    testBuffer[2] = 0xFC; 
    testBuffer[3] = 0x03; 
    testBuffer[4] = 0x03;
    testBuffer[5] = 0x23; 
    testBuffer[6] = 0x02; 
    BTTransmit(testBuffer,7,TRUE);  
    receiveIntercharTimeout = FALSE;        
    while ((response_received == FALSE)&&(rx_timeout == FALSE)&&(receiveIntercharTimeout == FALSE));    
    BlockingTimer(30);
    if (general_buffer[10] == 0x38)
    {    
      whichModule = MODULE_BM78; 
    }
    else
    {
      whichModule = MODULE_BM77; 
    }
    
    switch (whichModule)
    {
      case MODULE_BM77:
      {
        PanasonicRXQueueReset();
        testBuffer[0] = 0x01; 
        testBuffer[1] = 0x27; 
        testBuffer[2] = 0xFC; 
        testBuffer[3] = 0x15; 
        testBuffer[4] = 0x00;
        testBuffer[5] = 0x0B; 
        testBuffer[6] = 0x12; 
        testBuffer[7] = 'P'; 
        testBuffer[8] = 'P'; 
        testBuffer[9] = '-'; 
        testBuffer[10] = 'H'; 
        testBuffer[11] = 'y'; 
        testBuffer[12] = 'd'; 
        testBuffer[13] = 'C'; 
        testBuffer[14] = 'M';
        testBuffer[15] = '2'; 
        testBuffer[16] = ' ';        
        testBuffer[17] = ID[0]; //'4'; 
        testBuffer[18] = ID[1]; //'4'; 
        testBuffer[19] = ID[2]; //'4'; 
        testBuffer[20] = ID[3]; //'4';
        testBuffer[21] = 0x00; 
        testBuffer[22] = 0x00;
        testBuffer[23] = 0x00;
        testBuffer[24] = 0x10;   
        BTTransmit(testBuffer,25,TRUE);  
        receiveIntercharTimeout = FALSE;        
        while ((response_received == FALSE)&&(rx_timeout == FALSE)&&(receiveIntercharTimeout == FALSE));  
        BlockingTimer(30);
  
        PanasonicRXQueueReset();
        testBuffer[0] = 0x01; 
        testBuffer[1] = 0x29; 
        testBuffer[2] = 0xFC; 
        testBuffer[3] = 0x03; 
        testBuffer[4] = 0x00;
        testBuffer[5] = 0xdc; 
        testBuffer[6] = 0x02; 
        BTTransmit(testBuffer,7,TRUE);  
        receiveIntercharTimeout = FALSE;        
        while ((response_received == FALSE)&&(rx_timeout == FALSE)&&(receiveIntercharTimeout == FALSE));  
        BlockingTimer(30);
        
        PanasonicRXQueueReset();
        testBuffer[0] = 0x01; 
        testBuffer[1] = 0x27; 
        testBuffer[2] = 0xFC; 
        testBuffer[3] = 0x05; 
        testBuffer[4] = 0x00;
        testBuffer[5] = 0xdc; 
        testBuffer[6] = 0x02;
        testBuffer[7] = 0x00;
        testBuffer[8] = 0x10;
        BTTransmit(testBuffer,9,TRUE);  
        receiveIntercharTimeout = FALSE;        
        while ((response_received == FALSE)&&(rx_timeout == FALSE)&&(receiveIntercharTimeout == FALSE));  
        BlockingTimer(30);
        if (PanasonicSubRXFromQueue(&mytempbuffer[0])!= 0)
        {
        }                
        
        PanasonicRXQueueReset();
        testBuffer[0] = 0x01; 
        testBuffer[1] = 0x27; 
        testBuffer[2] = 0xFC; 
        testBuffer[3] = 0x04; 
        testBuffer[4] = 0x01;
        testBuffer[5] = 0xc6; 
        testBuffer[6] = 0x01; 
        testBuffer[7] = 0x00;    
        BTTransmit(testBuffer,8,TRUE);  
        receiveIntercharTimeout = FALSE;        
        while ((response_received == FALSE)&&(rx_timeout == FALSE)&&(receiveIntercharTimeout == FALSE));  
        BlockingTimer(30);
        if (PanasonicSubRXFromQueue(&mytempbuffer[0])!= 0)
        {
        }                   
    
        break;
      }
       case MODULE_BM78:
      {
         //------------------------ Build device name for programming 
        PanasonicRXQueueReset();
        testBuffer[0] = 0x01; 
        testBuffer[1] = 0x27; 
        testBuffer[2] = 0xFC; 
        testBuffer[3] = 0x12;  //0x14; 
        testBuffer[4] = 0x00;
        testBuffer[5] = 0x0B; 
        testBuffer[6] = 0x0f;  //0x11; 
        testBuffer[7] = 'P'; 
        testBuffer[8] = 'P'; 
        testBuffer[9] = '-'; 
        testBuffer[10] = 'H'; 
        testBuffer[11] = 'y'; 
        testBuffer[12] = 'd'; 
        testBuffer[13] = 'C'; 
        testBuffer[14] = 'M';
        testBuffer[15] = '2'; 
        testBuffer[16] = ' ';
        testBuffer[17] = ID[0]; //'4'; 
        testBuffer[18] = ID[1]; //'4'; 
        testBuffer[19] = ID[2]; //'4'; 
        testBuffer[20] = ID[3]; //'4';
        testBuffer[21] = 0x00;
     //   testBuffer[22] = 0x00;
     //   testBuffer[23] = 0x10;   
        BTTransmit(testBuffer,22,TRUE); //24  
        receiveIntercharTimeout = FALSE;        
        while ((response_received == FALSE)&&(rx_timeout == FALSE)&&(receiveIntercharTimeout == FALSE));  
        BlockingTimer(30);
        if (PanasonicSubRXFromQueue(&mytempbuffer[0])!= 0)
        {
        }    
        //---------------------------------
        testBuffer[0] = 0x01; 
        testBuffer[1] = 0x27; 
        testBuffer[2] = 0xFC; 
        testBuffer[3] = 0x17;  //0x14; 
        testBuffer[4] = 0x03;
        testBuffer[5] = 0xA0; 
        testBuffer[6] = 0x14;  //0x11;      
        testBuffer[7] = 0x13; 
        testBuffer[8] = 0x0f; 
        testBuffer[9] = 0x09;    
        testBuffer[10] = 'P'; 
        testBuffer[11] = 'P'; 
        testBuffer[12] = '-'; 
        testBuffer[13] = 'H'; 
        testBuffer[14] = 'y'; 
        testBuffer[15] = 'd'; 
        testBuffer[16] = 'C'; 
        testBuffer[17] = 'M';
        testBuffer[18] = '2'; 
        testBuffer[19] = ' ';
        testBuffer[20] = ID[0]; //'4'; 
        testBuffer[21] = ID[1]; //'4'; 
        testBuffer[22] = ID[2]; //'4'; 
        testBuffer[23] = ID[3]; //'4';
        testBuffer[24] = 0x02;
        testBuffer[25] = 0x01;
        testBuffer[26] = 0x02;   
        BTTransmit(testBuffer,27,TRUE); //24  
        receiveIntercharTimeout = FALSE;        
        while ((response_received == FALSE)&&(rx_timeout == FALSE)&&(receiveIntercharTimeout == FALSE));  
        BlockingTimer(30);
  
        break;
      }     
      
    }  

    PanasonicRXQueueReset();
    testBuffer[0] = 0x01; 
    testBuffer[1] = 0x29; 
    testBuffer[2] = 0xFC; 
    testBuffer[3] = 0x03; 
    testBuffer[4] = 0x00;
    testBuffer[5] = 0x0b; 
    testBuffer[6] = 0x11; 
    BTTransmit(testBuffer,7,TRUE);  
    receiveIntercharTimeout = FALSE;        
    while ((response_received == FALSE)&&(rx_timeout == FALSE)&&(receiveIntercharTimeout == FALSE));    
    BlockingTimer(30);
    if (PanasonicSubRXFromQueue(&mytempbuffer[0])!= 0)
    {
    }     
   
 

/*    
    loffset = 0; 
    while (loffset< 0x410)
    {
      PanasonicRXQueueReset();
      testBuffer[0] = 0x01; 
      testBuffer[1] = 0x29; 
      testBuffer[2] = 0xFC; 
      testBuffer[3] = 0x03; 
      ltemp = loffset; 
      ltemp = ltemp>>8; 
      testBuffer[4] = ltemp;
      ltemp = loffset; 
      ltemp &= 0xff;
      testBuffer[5] = ltemp; 
      testBuffer[6] = 0x10; 
      BTTransmit(testBuffer,7,TRUE);  
      receiveIntercharTimeout = FALSE;        
      while ((response_received == FALSE)&&(rx_timeout == FALSE)&&(receiveIntercharTimeout == FALSE));    
      BlockingTimer(30);
//      if (PanasonicSubRXFromQueue(&mytempbuffer[0])!= 0)
//      {
        for (i=0;i<16;i++)
        {
          moduleEEPROM[i+loffset] = general_buffer[10+i]; 
        }
//      }           
      loffset += 16; 
    }  
*/    
#endif     
    
//     WriteSetting(BT_First_Setting, BLUETOOTH_UCHIP_SETTING,TRUE);
//  }
  
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(BLUETOOTH_PROG, &pin_conf);
  //----------------------
  // reset the devcie. 
  //----------------------
  	port_pin_set_output_level(BLUETOOTH_RESET, false);
//CLEAN  while(MicroDelay(General_Blocking, 1000) == FALSE);  
  BlockingTimer(400);
//  usartMode = USART_MODE_BYPASS; /
//  BTReceiveStart();  
//  PanasonicRXQueueReset(); 
  	port_pin_set_output_level(BLUETOOTH_RESET, true);
//CLEAN  while(MicroDelay(General_Blocking, 2000) == FALSE);    
  BlockingTimer(1000);  
  return 0; 
}
 

#endif
