//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: BLUETOOTH.C
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor: STM32F103R
// TOOLS: IAR Workbench 
// DATE:
// CONTENTS: This file contains  
//------------------------------------------------------------------------------
// HISTORY: This file  
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// LAST MODIFIED:
// 5/26/2014 (emh) in Hydro project.
//------------------------------------------------------------------------------
 
#include "defs.h"
#include "bluetoothHeader.h"
#include "MC_type.h"
#include "i2c_ee.h"
#include "config.h"
#include "appmain.h"
#include "driverrf.h"
#include "h_motor.h"
#include "driverRTCHeader.h"
#include "appauto.h"
#include "appProtocolHeader.h"
//#include "apiHCI.h"
#include "ymodem.h" 
#include "driverpanasonicheader.h" 

//#include "tcu_common.h"
//#include "ble_api_internal.h"
//#include "tcu_spp.h"
//#include "tcu_mng.h"

//---------------------GLOBAL VARIABLES-----------------------------------

//--------------------------
// BLUETOOTH RECEIVE BUFFERS. 
// general_buffer - is in the interrupt 
// mytempbuffer - for loading messages out of queue to process. 
// panasonicRXQueue - to hold queued messages. 
  //-------------general_buffer is 250 bytes long       
  // this holds the responses from COMMAND MODE commands
//#define MAX_GENERAL_BUFFER 250
uint8_t general_buffer[MAX_GENERAL_BUFFER];
uint16_t general_buffer_counter = 0;
uint8_t response_started = FALSE;
uint8_t response_received = FALSE;
uint8_t receiveIntercharTimeout = FALSE; 

uint16_t download_buffer_counter;
uint8_t download_buffer[MAX_GENERAL_BUFFER]; 
  //------------------mytempbuffer is 128 bytes
  // This buffer is used as temporarily loads from panasonic queue 
  //------------------------------------------------
uint8_t mytempbuffer[MAX_GENERAL_BUFFER]; 
#define MAX_PANASONIC_RX_BUFFERS 8
#define MAX_PANASONIC_RX_BUFFER_SIZE 200
uint8_t panasonicRXQueue[MAX_PANASONIC_RX_BUFFERS][MAX_PANASONIC_RX_BUFFER_SIZE]; 
uint8_t panasonicRXCount=0; 
uint8_t panasonicRXOffset=0; 

  //--------------tx_buffer 
  // this is the transmit buffer for all packets being sent. 
  //------------------------------------
#define MAX_BLUETOOTH_TX_BUFFER  255
uint8_t tempBuffer[MAX_BLUETOOTH_TX_BUFFER]; 
uint8_t tx_buffer[MAX_BLUETOOTH_TX_BUFFER];
uint8_t tx_buffer_counter = 0;
uint16_t tx_len = 0;
uint8_t transmitting = FALSE; 
bool lastChar = FALSE; 
 
uint8_t tx_timeout = FALSE;
uint8_t rx_timeout = FALSE;

bool bluetoothEchoActive;
bool bluetoothCurrentStream;
uint8_t dataResponseBuffer[MAX_BLUETOOTH_BUFFER]; 

//---------------------LOCAL VARIABLES------------------------------------
uint8_t echoTempBuffer[40]; 
uint8_t temp_buf[50];
 
uint8_t myBluetoothID[4];
uint16_t parameterLength;
uint32_t usart2Status; 

//------------usartMode 
uint8_t  usartMode; 

//---------------------LOCAL FUNCTION PROTOTYPES--------------------------
uint8_t BluetoothSTConfig(void);
uint8_t BluetoothMicrochipConfig(void);

 
 


void PanasonicRXQueueReset(void)
{
  panasonicRXCount=0; 
  panasonicRXOffset=0;   
}  

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: 
//------------------------------------------------------------------------------
//  
//==============================================================================
uint8_t PanasonicSubRXFromQueue(uint8_t *buffer)
{
  uint16_t i,itemp,itemp2;
  uint8_t status; 
  uint8_t *ptr; 
  status = 0; 
  ptr = buffer; 
  //---------------------------------
  // take a message out. 
  if (panasonicRXOffset >= MAX_PANASONIC_RX_BUFFERS)
  {
    panasonicRXOffset = 0; 
  }     
  if (panasonicRXOffset != panasonicRXCount)
  {
      itemp = panasonicRXQueue[panasonicRXOffset][0];
      itemp2 = panasonicRXQueue[panasonicRXOffset][1];
      itemp2 = itemp2<<8; 
      itemp |= itemp2; 
      if (itemp < MAX_PANASONIC_RX_BUFFER_SIZE)
      {
        for (i=0;i<(itemp+2);i++)
        {
          *ptr++ = panasonicRXQueue[panasonicRXOffset][i];
        }
        status = 1; 
      }  
      panasonicRXOffset++; 
      if (panasonicRXOffset >= MAX_PANASONIC_RX_BUFFERS)
      {
        panasonicRXOffset = 0; 
      }          
   
  }    
  return status; 
}

uint8_t lastin; 
uint16_t commandRx;
extern uint8_t bluetoothFirmwareUpdate;
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  USART2_IRQHandler
//------------------------------------------------------------------------------
// This function handles USART2 interrupts
//                  request.
//==============================================================================
void USART2_IRQHandler(void)
{
  uint8_t new_char; 
  uint16_t ai,itemp; 

  //--------------------------
  // read the status register - to allow clearing of any errors. 
  // read the data register 
  usart2Status = USART2->SR; 
  //----------------------------
  // check the current errors - 
  // if IDLE (bit 4), Overrun error (bit 3)
  // Noise Detected (bit 2), frameing error (bit 1) 
  // parity error (bit 0. 
  // == to clear this error - must have read the SR 
  // THEN read the data register. 
  //----------------------------
  if ((usart2Status & 0x0f)!= 0)
  {
    new_char = USART2->DR;
  }
  else
  {
    if ((usart2Status & USART_FLAG_RXNE)!= 0)    
    {
      new_char = USART2->DR;
    }      
  }  

  if ((usart2Status & USART_FLAG_RXNE)!= 0)
  {
    USART2->SR &= (~USART_FLAG_RXNE); 
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);  

    if (usartMode == USART_MODE_DOWNLOAD2)
    { 

      intercharTimer = 50; //8; 
      if (general_buffer_counter >= MAX_GENERAL_BUFFER)
      {
        general_buffer_counter = MAX_GENERAL_BUFFER-1;
      }
      general_buffer[general_buffer_counter++] = new_char; 
      if (download_buffer_counter >= MAX_GENERAL_BUFFER)
      {
        download_buffer_counter = MAX_GENERAL_BUFFER-1;
      }
      download_buffer[download_buffer_counter++] = new_char; 
    }              
    if (usartMode == USART_MODE_DOWNLOAD)
    { 

      intercharTimer = 8; 
      if (general_buffer_counter >= MAX_GENERAL_BUFFER)
      {
        general_buffer_counter = MAX_GENERAL_BUFFER-1;
      }
      general_buffer[general_buffer_counter++] = new_char; 
      if (download_buffer_counter >= MAX_GENERAL_BUFFER)
      {
        download_buffer_counter = MAX_GENERAL_BUFFER-1;
      }
      download_buffer[download_buffer_counter++] = new_char; 
    }             
    //-----------------------------------
    //general buffer is always added to
    //-----------------------------------
    if (usartMode == USART_MODE_COMMAND)
    {
      if (general_buffer_counter >= MAX_GENERAL_BUFFER)
      {
        general_buffer_counter = MAX_GENERAL_BUFFER-1;
      }
      general_buffer[general_buffer_counter++] = new_char; 
      switch (new_char)
      {
        case '\n':
        case '\r':
        {  
          //-------------------------
          //if we get a newline or carriage return on the first char ignore it 
          if (general_buffer_counter > 1) 
          {
            //-------------------
            //handle response
 //           if (response_started == TRUE)
 //           {
              response_received = TRUE;
              if (panasonicRXCount >= MAX_PANASONIC_RX_BUFFERS)
              {
                 panasonicRXCount = 0;
              }        
              itemp = general_buffer_counter & 0x00ff; 
              panasonicRXQueue[panasonicRXCount][0] = itemp;
              itemp = general_buffer_counter>>8;
              panasonicRXQueue[panasonicRXCount][1] = itemp;                   
              for (ai=0;ai<general_buffer_counter;ai++)
              {
                panasonicRXQueue[panasonicRXCount][ai+2] = general_buffer[ai]; 
              }
              panasonicRXCount++;
              if (panasonicRXCount >= MAX_PANASONIC_RX_BUFFERS)
              {
                panasonicRXCount = 0; 
              }  
              if (panasonicRXCount== panasonicRXOffset)
              {
                panasonicRXOffset++;
                if (panasonicRXOffset >= MAX_PANASONIC_RX_BUFFERS)
                {
                   panasonicRXOffset = 0; 
                } 
              }         
              general_buffer_counter = 0;
              response_started = FALSE;
              response_received = TRUE;      
              intercharTimer = 0; 
              receiveIntercharTimeout = FALSE;                
            }
//          }
          break;
        }
        case 'A':
        {
            response_started = TRUE; 
            break;
        }  
        case '~':
        {
          //handle responses coming from the module
          if (response_started == FALSE)
          {
            //begin module response
            response_started = TRUE;
          }
          break;
        } 

        case ' ':
        case 0x00:
        {  
          //-------------------------
          //if we get a newline or carriage return on the first char ignore it 
          if (general_buffer_counter ==1) 
          {
            general_buffer_counter = 0; 
          }
          break;
        }        
      }      
    }
    if (usartMode == USART_MODE_BYPASS)
    {    
      if (response_started == TRUE)
      {  
        if (general_buffer_counter >= MAX_GENERAL_BUFFER)
        {
          general_buffer_counter = MAX_GENERAL_BUFFER-1;
        }
        general_buffer[general_buffer_counter++] = new_char;      
      }
      switch (new_char)
      {
        case '\n':
        case '\r':
        {  
          //-------------------------
          //if we get a newline or carriage return on the first char ignore it 
          if (general_buffer_counter > 1) 
          {
            //handle packet
            if (response_started == TRUE)
            {
              //set flag and length so it can be called from the main loop
              if (panasonicRXCount >= MAX_PANASONIC_RX_BUFFERS)
              {
                 panasonicRXCount = 0;
              }
              if (parameterLength > MAX_PANASONIC_RX_BUFFER_SIZE)
              {
                parameterLength = MAX_PANASONIC_RX_BUFFER_SIZE;
              }  
              itemp = general_buffer_counter & 0x00ff; 
              panasonicRXQueue[panasonicRXCount][0] = itemp;
              itemp = general_buffer_counter>>8;
              panasonicRXQueue[panasonicRXCount][1] = itemp;                   
              for (ai=0;ai<general_buffer_counter;ai++)
              {
                panasonicRXQueue[panasonicRXCount][ai+2] = general_buffer[ai]; 
              }
              panasonicRXCount++;
              if (panasonicRXCount >= MAX_PANASONIC_RX_BUFFERS)
              {
                panasonicRXCount = 0; 
              }  
              if (panasonicRXCount== panasonicRXOffset)
              {
                panasonicRXOffset++;
                if (panasonicRXOffset >= MAX_PANASONIC_RX_BUFFERS)
                {
                   panasonicRXOffset = 0; 
                } 
              }         
              general_buffer_counter = 0;
              response_started = FALSE;
              parameterLength = 0; 
              response_received = TRUE;      
              intercharTimer = 0; 
              receiveIntercharTimeout = FALSE;             
              schedByte |=SCHEDBYTE_RUN_SIMPLE_BLUETOOTH;
            }
          }
          break;
        }
        case '#':
        {
          //handle commands coming across BT
          if (response_started == FALSE) 
          {
            //begin simple packet
            response_started = TRUE;
            general_buffer_counter = 0;
          }
          break;
        }
      }
    }  
    if (usartMode == USART_MODE_EEPROM)
    {    
      if (general_buffer_counter >= MAX_GENERAL_BUFFER)
      {
        general_buffer_counter = MAX_GENERAL_BUFFER-1;
      }
      general_buffer[general_buffer_counter++] = new_char;      
      receiveIntercharTimeout = FALSE;        
      intercharTimer = 50; 
    }  
  }  
  //------------------------------------
  // TRANSMIT PROCESSING.
  //------------------------------------  
  if ((usart2Status & USART_FLAG_TXE)!= 0)  
  {
    USART_ClearITPendingBit(USART2, USART_FLAG_TXE);    
    //-----------------------
    //transmit interrrupt   
    if (transmitting == FALSE)
    {
      lastChar = FALSE; 
      USART_ITConfig(USART2, USART_IT_TXE,(FunctionalState)DISABLE);
    }
    else
    {
      if (lastChar == FALSE)
      {
        //-------------------
        //send the next byte
        //--------------------
        USART2->DR = tx_buffer[tx_buffer_counter]; 
        //--------------------  
        // check if finished
        //--------------------
 //        if (tx_buffer[tx_buffer_counter] == '\r' || ((tx_buffer_counter+1) >= MAX_BLUETOOTH_TX_BUFFER) || ((tx_buffer_counter + 1) >= tx_len))      
        if (((tx_buffer_counter+1) >= MAX_BLUETOOTH_TX_BUFFER) || ((tx_buffer_counter + 1) >= tx_len))
        {
          lastChar = TRUE;
        }
        else
        {
          //-------------------------------
          //not finished. increment counter.
          tx_buffer_counter++;            
        }  
      }
      else
      {
        //-----------------------------
        //wait for last char to send
//        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
        transmitting = FALSE;
        lastChar = FALSE;
//        USART_ITConfig(USART2, USART_IT_TXE,DISABLE);
      } 
    }    
//JULY    USART_ClearITPendingBit(USART2, USART_FLAG_TXE);
  }
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

   

  //---------------bluetoothError
  // used for testing the timing on responses 
//uint8_t bluetoothError=FALSE; 
//uint8_t bluetoothClassic;
uint8_t bluetoothRetry;
//#define BLUETOOTH_FIRST_SETTING SW_VER
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: BTInit 
//------------------------------------------------------------------------------
//  
//==============================================================================
void BTInit(void)
{
  uint8_t new_char;  
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
 
  usartMode = USART_MODE_BYPASS; 
  bluetoothEchoActive = FALSE;
  bluetoothCurrentStream = FALSE; 
   
#if PRODUCT_HYDRO  
  InitBTIO();
#endif  
  //----------------------------- 
  // Enable the USART2   
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,(FunctionalState)ENABLE);
  USART_Cmd(USART2,(FunctionalState)DISABLE);  
  //---------------------------
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = (FunctionalState)DISABLE;
  NVIC_Init(&NVIC_InitStructure);      
  //---------------------------
  // Disable all interrupts 
  USART_ITConfig(USART2, USART_IT_RXNE,(FunctionalState)DISABLE);
  USART_ITConfig(USART2, USART_IT_TXE,(FunctionalState)DISABLE);  
  USART_ITConfig(USART2, USART_IT_CTS,(FunctionalState)DISABLE);
  USART_ITConfig(USART2, USART_IT_LBD,(FunctionalState)DISABLE);
  USART_ITConfig(USART2, USART_IT_TC,(FunctionalState)DISABLE);
  USART_ITConfig(USART2, USART_IT_IDLE,(FunctionalState)DISABLE);
  USART_ITConfig(USART2, USART_IT_PE,(FunctionalState)DISABLE);
  USART_ITConfig(USART2, USART_IT_ERR,(FunctionalState)DISABLE);
  //------------------------------------------------------
  // Communication hyperterminal-USART2 using hardware flow control
  // USART2 configured as follow:
  //      - BaudRate = 115200 baud  
  //      - Word Length = 8 Bits
  //      - One Stop Bit
  //      - No parity
  //      - Hardware flow control enabled (RTS and CTS signals)
  //      - Receive and transmit enabled
  //-------------------------------------------------------
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART2, &USART_InitStructure);
  //----------------------------- 
  // Enable the USART2 
  USART_Cmd(USART2,(FunctionalState)ENABLE);
  //interrupts
  USART_ITConfig(USART2, USART_IT_RXNE,(FunctionalState)ENABLE);
  USART_ITConfig(USART2, USART_IT_TXE,(FunctionalState)ENABLE);  
  NVIC_InitStructure.NVIC_IRQChannelCmd = (FunctionalState)ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  usart2Status = USART2->SR; 
  new_char = USART2->DR;
  USART_ClearITPendingBit(USART2, USART_IT_RXNE);
  USART_ClearITPendingBit(USART2, USART_IT_TXE);

  usartMode = USART_MODE_COMMAND; 
  
  BluetoothMicrochipConfig();        
}

#define MODULE_BM77  0
#define MODULE_BM78  1
#define MODULEEEPROMSIZE 0x420
uint8_t moduleEEPROM[MODULEEEPROMSIZE];
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: BluetoothMicrochipConfig
//------------------------------------------------------------------------------
//  
//==============================================================================
uint8_t testBuffer[35]; 
uint8_t BluetoothMicrochipConfig(void)
{
  uint8_t ID[4],temp,temp2;  
  uint16_t  i,loffset,ltemp;
  uint8_t done,offset,whichModule;

  GPIO_InitTypeDef GPIO_InitStructure;
  // Enable GPIO clock and release reset
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD,
                         (FunctionalState)ENABLE);  
  //speed is common to all pins
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  
  if (ReadSetting(BT_First_Setting,TRUE) != BLUETOOTH_UCHIP_SETTING)
  {     
    WriteSetting(BluetoothLE_Setting,FALSE,TRUE);   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_3;
    GPIO_Init(GPIOC, &GPIO_InitStructure);  
    GPIO_WriteBit(GPIOC,GPIO_Pin_15,Bit_RESET);     
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    GPIO_WriteBit(GPIOD,GPIO_Pin_2,Bit_SET);  //bbbbbbbbbbeth was RESET... 04/04/2016    
    
 
    usartMode = USART_MODE_EEPROM; 
    //----------------------
    // reset the devcie. 
    //----------------------
    GPIO_WriteBit(GPIOC,GPIO_Pin_3,Bit_RESET); 
    BlockingTimer(400);
    PanasonicRXQueueReset();
    BTReceiveStart();
    GPIO_WriteBit(GPIOC,GPIO_Pin_3,Bit_SET);      
    BlockingTimer(1000);    
 
    testBuffer[0] = 0x01; 
    testBuffer[1] = 0x03; 
    testBuffer[2] = 0x0C; 
    testBuffer[3] = 0x00; 
    testBuffer[4] = 0x00; 
    testBuffer[5] = 0xfd; 
    BTTransmit(testBuffer,4,TRUE);  
    receiveIntercharTimeout = FALSE;        
    while ((response_received == FALSE)&&(rx_timeout == FALSE)&&(receiveIntercharTimeout == FALSE));   
    BlockingTimer(30);
  
    testBuffer[0] = 0x01; 
    testBuffer[1] = 0x2D; 
    testBuffer[2] = 0xFC; 
    testBuffer[3] = 0x01; 
    testBuffer[4] = 0x08; 
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
      
      table0.Item.BluetoothID4 = ID[3];
      table0.Item.BluetoothID3 = ID[2];
      table0.Item.BluetoothID2 = ID[1];
      table0.Item.BluetoothID1 = ID[0]; 
      WriteSetting(BluetoothID4_Setting, table0.Item.BluetoothID4,TRUE); 
      WriteSetting(BluetoothID3_Setting, table0.Item.BluetoothID3,TRUE); 
      WriteSetting(BluetoothID2_Setting, table0.Item.BluetoothID2,TRUE); 
      WriteSetting(BluetoothID1_Setting, table0.Item.BluetoothID1,TRUE);         
    }        
    
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
    
    
     WriteSetting(BT_First_Setting, BLUETOOTH_UCHIP_SETTING,TRUE);
  }
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // output push/pull
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  
//  GPIO_WriteBit(GPIOC,GPIO_Pin_3,Bit_RESET);  
//  GPIO_WriteBit(GPIOC,GPIO_Pin_3,Bit_SET);  
  //----------------------
  // reset the devcie. 
  //----------------------
  GPIO_WriteBit(GPIOC,GPIO_Pin_3,Bit_RESET); 
//CLEAN  while(MicroDelay(General_Blocking, 1000) == FALSE);  
  BlockingTimer(400);
  usartMode = USART_MODE_BYPASS; 
  BTReceiveStart();  
  PanasonicRXQueueReset(); 
  GPIO_WriteBit(GPIOC,GPIO_Pin_3,Bit_SET); 
//CLEAN  while(MicroDelay(General_Blocking, 2000) == FALSE);    
  BlockingTimer(1000);  
  return 0; 
}

 

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: BTTransmitTimeout
//------------------------------------------------------------------------------
//  
//==============================================================================
void BTTransmitTimeout(void)
{
  tx_timeout = TRUE;
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: BluetoothRFEcho
//------------------------------------------------------------------------------
//  
//==============================================================================
void BluetoothRFEcho(void)
{
  uint16_t i,length; 
 
  length = BTTransmitLoadString("\n", 1, &echoTempBuffer[0]);
  echoTempBuffer[length++] = table0.Item.Instance;  
  echoTempBuffer[length++] = ':';
   
  
  if (bluetoothEchoActive == TRUE)
  {
    for (i=0;i<8;i++)
    {
      length += BTTransmitLoadHex(echoData[i],TRUE,&echoTempBuffer[length]);
    }  
  
    if (echoData[8])
      echoTempBuffer[length++] = 'G';
    else
      echoTempBuffer[length++] = 'B';     
    
    length += BTTransmitLoadString("\n\r>",3, &echoTempBuffer[length]);   
    ProtocolTransmitOut(&echoTempBuffer[0],length,FALSE,bluetoothInterface); 
  }    
}

#if PRODUCT_MICRO
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: BluetoothSpiritEcho
//------------------------------------------------------------------------------
//  
//==============================================================================
void BluetoothSpiritEcho(void)
{
  uint8_t i,length; 
 
  length = BTTransmitLoadString("\n", 1, &echoTempBuffer[0]);
  echoTempBuffer[length++] = table0.Item.Instance;  
  echoTempBuffer[length++] = ':';
 
  if (bluetoothEchoActive == TRUE)
  {
    for (i=0;i<8;i++)
    {
      length += BTTransmitLoadHex(spiritEchoData[i],TRUE,&echoTempBuffer[length]);
    }  
  
    if (spiritEchoData[8])
      echoTempBuffer[length++] = 'G';
    else
      echoTempBuffer[length++] = 'B';     
    
    length += BTTransmitLoadString("\n\r>",3, &echoTempBuffer[length]);   
    ProtocolTransmitOut(&echoTempBuffer[0],length,FALSE,activeInterface); 
  }    
}
#endif 

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------LOCAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: BTReceiveStart
//------------------------------------------------------------------------------
//  
//==============================================================================
void BTReceiveStart(void)
{
  uint16_t i; 
  
  for (i=0;i<20;i++)
  {
//    general_buffer[i] = 0x00;
  }
  general_buffer_counter = 0;
  download_buffer_counter = 0; 
  response_started = FALSE;
  response_received = FALSE; 
  rx_timeout = FALSE;
  schedByte &= ~SCHEDBYTE_RUN_SIMPLE_BLUETOOTH;     
  BT_RX_delay = (2000+14) / 15;
  intercharTimer = 0; 
  receiveIntercharTimeout = FALSE;  

}


 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: BTTransmit
//------------------------------------------------------------------------------
//  
//==============================================================================
void BTTransmit(uint8_t* data, uint8_t length,uint8_t receiveClear)
{
  uint16_t i,mylen;  
  
  //----------------------------
  //wait for tx or timeout
  while(transmitting == TRUE && tx_timeout == FALSE); 
  USART_ITConfig(USART2, USART_IT_TXE,  (FunctionalState)JL_DISABLE);  
  USART_ClearITPendingBit(USART2, USART_FLAG_TXE);
 
  mylen = length; 
  //------------------------------
  //copy into buffer
  //------------------------------
  if (mylen > MAX_BLUETOOTH_TX_BUFFER)
  {
    mylen = MAX_BLUETOOTH_TX_BUFFER; 
  }  
  for (i=0; i<mylen; i++)
  {
    tx_buffer[i] = data[i];
  }
  //-----------------------------
  //reset counter
  tx_buffer_counter = 0;
  //-----------------------------
  //set length
  tx_len = mylen;
  //--------------------
  //set flag
  transmitting = TRUE;
  
  if (receiveClear == TRUE)
  {
    BTReceiveStart(); 
    //-------------------
    for (i=0;i<10;i++)
    {
      general_buffer[i] = 0x88; //' ';
    }      
  }  
 //----------------------
  //start timeout timer
  tx_timeout = FALSE;
//CLEAN  MicroDelay(BT_TX_Timeout, BT_TX_TIMEOUT);  
  BT_TX_delay = (BT_TX_TIMEOUT+14) / 15;

  lastChar = FALSE; 
  USART_ITConfig(USART2, USART_IT_TXE,  (FunctionalState)JL_DISABLE);
  USART_ITConfig(USART2, USART_IT_TXE,  (FunctionalState)JL_ENABLE);  
}

 
    
