//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: driverRTC.C
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor: STM32F103R
// TOOLS: IAR Workbench 
// DATE:
// CONTENTS: This file contains  
//------------------------------------------------------------------------------
// HISTORY: This file  
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#include "defs.h"

#include "utils.h"
#include "h_driverrtc.h" 
#include "appmain.h"
#include "h_motor.h"
#include "bluetoothHeader.h"
#include "config.h"
#include "buttons.h"
#include "appauto.h"
#include "apprfHeader.h"
#include "driverrf.h"

//---------------------LOCAL VARIABLES------------------------------------

int BT_TX_delay  = -1;
int BT_RX_delay  = -1;
int debounce_delay  = -1;

short int Debounce_delay_complete = FALSE;
 
  //------------------------
  // timer_sysmonitor - set up for monitoring the system every TIME_SYSMONITOR second 
  //  - will handle catching faults and logging them. 
#define TIME_SYSMONITOR 2
int timer_sysmonitor;  

//---------------------GLOBAL VARIABLES-----------------------------------
short int RTC_delay_complete;
int ticks_remaining;

  //-------------timerStreamOff, timerEchoOff
  // both used in bluetooth timeouts for streaming and echo when 
  // active. 
uint8_t timerStreamOff;
uint16_t timerEchoOff;

uint8_t timerMotorControl=0;

  //-----------------------------------
  // blink_cnt - incremented every RTC int ~ .0122 secs  (12.2 msec)
  // ... so 67 count is 1 second. 
  //-----------------------------------
short int blink_cnt = 0;          

uint16_t standby_cntr   = 0;     

uint8_t bluetoothTimer;
 
//---------------------LOCAL FUNCTION PROTOTYPES--------------------------  


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// --------------------------INTERRUPT FUNCTIONS -------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

uint16_t count; 

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  RTC_IRQHandler
//------------------------------------------------------------------------------
// This function handles  
//==============================================================================
#ifdef TEST_RTC
unsigned int toggle;
#endif 
void RTC_IRQHandler(void)
{
  /* Wait until last write operation on RTC registers has finished */
  //RTC_WaitForLastTask();   
  /* Clear RTC Alarm interrupt pending bit */
  RTC_ClearITPendingBit(RTC_IT_SEC);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
 
  if (count >0)
  {
    count--;
  }  
#ifdef TEST_RTC
  if (toggle != 0)
  {
    toggle = 0; 
    GPIO_WriteBit(GPIOC,GPIO_Pin_5,Bit_SET);
  }
  else
  {
    toggle = 1; 
    GPIO_WriteBit(GPIOC,GPIO_Pin_5,Bit_RESET);
  }
#endif  
  schedByte |= SCHEDBYTE_RTC_TICKLE;
  
  //--------------------------
  //handle general delay
  //--------------------------
  if (ticks_remaining >= 0 && --ticks_remaining == 0)
    RTC_delay_complete = TRUE;
  //--------------------------
  //handle debounce delay
  //--------------------------
  if (debounce_delay >= 0 && --debounce_delay == 0)
  {
    Debounce_delay_complete = TRUE;  
  }
  //---------------------------
  // timer for message time stamping
  if (timerRFReceive < 0xff)
  {
    timerRFReceive++;
  }  
  //----------------------------------------
  // button timer - must be in IRQ since used 
  // in looping in mainline code
  //---------------------------------------
  if (timerButtonIRQDebounce > 0)
  {
    --timerButtonIRQDebounce;
  }   
  if (timerButton3SecDebounce > 0)
  {
    --timerButton3SecDebounce;
  }     
   //----------------------------------------
  // RF Pair delay if active
  //---------------------------------------
  if (timerPairSession > 0)
  {
    --timerPairSession;
  }   

  //----------------------------------------
  //handle motor turn off delay & auto off
  //---------------------------------------
  if (timerMotorOff >= 0 && --timerMotorOff == 0)
  {
      schedByte|= SCHEDBYTE_MOTOROFF; 
  }
  if (timerMotorOff < 0)
  {
//    schedByte |= SCHEDBYTE_MOTOROFF;
  }
  //----------------------------------------
  //  
  //---------------------------------------
  if (timerConfigMenu > 0)
  {
    --timerConfigMenu;
  }    
  //-------------------------------------
  //poll buttons, no delay
  //-------------------------------------
  IncDebounce(0);
 //--------------------------- 
  //handle BT transmit timeout
  if (BT_TX_delay >= 0 && --BT_TX_delay == 0)
  {
    //end tx
    BTTransmitTimeout();
    //turn off this delay
    BT_TX_delay = -1;
  }
  //------------------------
  //handle BT receive timeout
  if (BT_RX_delay >= 0 && --BT_RX_delay == 0)
  {
    //end rx
    BTReceiveTimeout(); 
    //turn off this delay
    BT_RX_delay = -1;
  }     
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  RTCAlarm_IRQHandler
//------------------------------------------------------------------------------
// This function handles RTC Alarm interrupt request.
//============================================================================== 
void RTCAlarm_IRQHandler(void)
{
    //--------------------------------
    // Clear EXTI line17 pending bit 
    //------------------------------
    EXTI_ClearITPendingBit(EXTI_Line17);
    //-----------------------------------
    // Check if the Wake-Up flag is set 
    //------------------------------------
    if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
    {
      //--------------------------
      // Clear Wake Up flag 
      //--------------------------
      PWR_ClearFlag(PWR_FLAG_WU);
    }
    //------------------------------------------------------------
    /// Wait until last write operation on RTC registers has finished 
    //--------------------------------------------------------
    RTC_WaitForLastTask();   
    //----------------------------------------
    // Clear RTC Alarm interrupt pending bit 
    //----------------------------------------
    RTC_ClearITPendingBit(RTC_IT_ALR);
    //------------------------------------------------------------
    /// Wait until last write operation on RTC registers has finished 
    //--------------------------------------------------------
    RTC_WaitForLastTask();
//  }
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  RTC_Configuration
//------------------------------------------------------------------------------
// This function Sets up the RTC to delay.
//==============================================================================
void RTC_Configuration(uint16_t ms)
{
  //reset flag
  RTC_delay_complete = FALSE;
  
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, (FunctionalState)JL_ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd((FunctionalState)JL_ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSI */
  RCC_LSICmd((FunctionalState)JL_ENABLE);

  /* Select LSI as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd((FunctionalState)JL_ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  //RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
  RTC_SetPrescaler(40000 * ms / 1000);    // using 40kHz LSI
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  
  /* reset counter */
  RTC_SetCounter(0);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

   // Enable the RTC Second
  RTC_ITConfig(RTC_IT_SEC, (FunctionalState)JL_ENABLE);
    
  // Wait until last write operation on RTC registers has finished
  RTC_WaitForLastTask();
  
  /* Enable the RTC Alarm interrupt */
  RTC_ITConfig(RTC_IT_ALR, (FunctionalState)JL_DISABLE);    // Enable when in Standby mode
  
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

}

#define MAX_RSSI_READING 10
uint16_t rssiReading[MAX_RSSI_READING];
uint8_t rssiReadingOffset; 
uint8_t rssiTimer; 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  RtcTickleTask
//------------------------------------------------------------------------------
// This function handles 
//============================================================================== 
void RtcTickleTask(void)
{
  int temp;  
  uint16_t AdcData;
      // Get new sample of the ADC
   if (rssiStream == TRUE)
   {
     motorLibState = MCI_GetSTMState(oMCI[0]);
     if (motorLibState == IDLE)
     {
        AdcData = ADC_GetConversionValue(ADC2);
        ADC_SoftwareStartConvCmd(ADC2, (FunctionalState)ENABLE); 
       if (rssiReadingOffset >= MAX_RSSI_READING)
       {
         rssiReadingOffset = 0;
       }
       rssiReading[rssiReadingOffset] = AdcData;
       rssiReadingOffset++;
       if (rssiTimer >5)
       {
         rssiTimer = 0;
         schedByte |= SCHEDBYTE_RSSISTREAM;
       }
       rssiTimer++;
     }  
  }  
  //----------------------------------------
  //handle motor box dealy
  //---------------------------------------
  if (timerBoxDelay > 0)
  {
    --timerBoxDelay;
  }  
   if (timerRampUp > 0)
  {
    --timerRampUp;
  }  
  if ((PostRunActive == FALSE)&&(motorOn==TRUE))
  {     
    //----------------
    // UP
    //----------------
//    if (timerBoxDelay > 0)
//    {
        if (myC1Direction > 0)
        {
          timeUp++;
          if (timeUp == 0)
          {
            timeUp = 0xff;
          }
          timeDown = 0;
        }
        else
        {
          timeDown++;
          if (timeDown == 0)
          {
            timeDown = 0xff;
          }
          timeUp = 0;
        }  
 //   }  
  }  
  //----------------------------------------
  // Bluetooth streaming timer. 
  //----------------------------------------

  if (bluetoothTimer >= 6)
  {
    bluetoothTimer = 0;
    if  (bluetoothCurrentStream == TRUE)
    {      
      schedByte |= SCHEDBYTE_BT_STREAMCURRENT;
    }  
  }  
  bluetoothTimer++;
   

  
  if (timerMotorControl >= 5)
  {
    schedByte |= SCHEDBYTE_MOTORSTATEMACHINE;      
    timerMotorControl = 0;      
  }  
  timerMotorControl++;
  
  //------------------------------------------------------- 
  //handle LED blink, run meter, and other timing functions
  //-------------------------------------------------------
  blink_cnt++;    // 67 blink_cnt = 1 sec;
  
  // Run Time Counter
  if (blink_cnt == 1 && (motorOn==TRUE))   //power_mode == OP_PWR)
  {  
    run_time_count++;
  }  
  else if (blink_cnt == 43 && (power_mode == IDLE_PWR && com_state != PAIRING) &&
           (table0.Item.BluetoothLE != 0))
  {  
    BlinkLED(TRUE);
  }    
  else if (blink_cnt == 51)  
  {  
    BlinkLED(FALSE);
  }    
  else if (blink_cnt == 59 && (power_mode == IDLE_PWR && com_state != PAIRING))
  {  
    BlinkLED(TRUE);
  }  
  else if (blink_cnt == 67)   // LED Off at end of 1 sec
  {
    //-----------------------------------
    // ONE SECOND EXPIRATION.
    // 1. bluetooth stream timer if active
    // 2. bluetooth echo timer if active
    //-----------------------------------
    if  (bluetoothEchoActive == TRUE)
    {
      timerEchoOff++;
      if (timerEchoOff >= 1800)
      {
        timerEchoOff = 0;
        bluetoothEchoActive = FALSE; 
      }  
    }  
    //----------------------------------------
    // Bluetooth streaming timer. 
    //----------------------------------------
    timerStreamOff++;
    if (timerStreamOff >= 180)
    {
      timerStreamOff = 0;
      if (bluetoothCurrentStream == TRUE)
      {
        bluetoothCurrentStream = FALSE; 
      }
      if (rssiStream== TRUE)
      {
        schedByte |= SCHEDBYTE_RSSI_DONE; 
      }  
    }  
    
    if (bluetoothBondingTimer >0)
    {
      --bluetoothBondingTimer;
      if (bluetoothBondingTimer == 0)
      {
             schedByte |= SCHEDBYTE_BLUETOOTHEXITBOND;

      }  
    }    
    
    //---------------------------
    // reset blink counter
    blink_cnt = 0;                
    enter_standby = TRUE;         // enter standby if in STANDBY_PWR mode
   
    
    if (power_mode != OP_PWR && com_state != PAIRING  )     
      BlinkLED(FALSE);  // LED off when in IDLE and not in pairing mode
    
    // Standby counter routine.
    if (power_mode == IDLE_PWR)   // count only when in IDLE
    {
      standby_cntr++;
//      Hibernate_Time = 0xFC; //TESTING BUFFER 
      if (table0.Item.Hibernate_Time == 0xFC) // check for 3 sec hibernate test mode
        temp = 3;
      else if (table0.Item.Hibernate_Time == 0xFD) // check for 60 secs hibernate test mode
        temp = 60;
      else 
        temp = table0.Item.Hibernate_Time*3600;
      if (table0.Item.Hibernate_Time != 0 && standby_cntr >= temp)  
      { 
          standby_cntr = temp; 
          ChangePwrMode(STANDBY_PWR);  // enter standby
      }
      // turn off motor relay 
      if (standby_cntr == 45)  //TEST OLIVERIO 60)   
      {
        motor_relay_enabled = FALSE;    // reset motor relay flag
        GPIO_WriteBit(GPIOB,GPIO_Pin_12,Bit_RESET);   
      }
    } 
  }  
  
      
  timer_sysmonitor++;
  if (timer_sysmonitor >= TIME_SYSMONITOR)
  { 
    timer_sysmonitor = 0;
    schedByte |= SCHEDBYTE_SYSMONITOR;
  }
  //------------------------------
  //handle start up delay for temp ready for using
  //-----------------------------
  if (timerTempReady > 0)
  {
      timerTempReady--;
  } 

 //---------------------------------- 
 //handle Auto mode Blockout delay
  if (timerAutoBlock >= 0 && --timerAutoBlock == 0)
  {
    autoBlockThrottle= FALSE;
  } 
}  
 


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  Delay
//------------------------------------------------------------------------------
// This function Uses the RTC to implement a delay, blocks during delay 
//
// Parameters: type - delay type (defined in utils.h), ms - milliseconds to delay
// Return: True if the delay was setup, false if the type is General_Nonblocking or General_Blocking and one is already running
// 
//==============================================================================
short int MicroDelay(DelayType type, uint16_t ms) //must be a multiple of the RTC tick to be accurate
{
  switch(type)
  {
    case General_Blocking: //set up a delay then block while executing
      if (ticks_remaining != -1)
        return FALSE; //already running a general delay
          
      //setup delay
      ticks_remaining = (ms+14) / 15; //adding 14 before dividing causes it to effectively round up
      RTC_delay_complete = FALSE;

      while(RTC_delay_complete == FALSE)
      {
            /* Reload IWDG counter */
            IWDG_ReloadCounter();
      } 
        
      break;
      
    case BT_TX_Timeout:
      //stop waiting for tx
      BT_TX_delay = (ms+14) / 15;
      break;
    
    case BT_RX_Timeout:
      //stop waiting for rx
      BT_RX_delay = (ms+14) / 15;
      break;
      
    case Debounce_Blocking: //set up a delay then block while executing
      //setup delay
      debounce_delay = (ms+14) / 15;  
      Debounce_delay_complete = FALSE;
      while(Debounce_delay_complete == FALSE); 
      break;      
          
  } 
  return TRUE;
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------LOCAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX



 