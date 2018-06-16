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
#include "config.h"
#include "driverI2C.h"
  
uint8_t Run_Time_Ctr_H; 
uint8_t Run_Time_Ctr_M; 
uint8_t Run_Time_Ctr_L; 
uint8_t Run_Time_Ctr_X; 
 
Table0 table0;
TABLESENSOR tableSensor;  
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   
//------------------------------------------------------------------------------
// This function handles allows all the RAM copies of EEPROM values to be loaded 
// in.
//==============================================================================
void ConfigInit(void)
{
  uint8_t testBuffer[10],value,n,eepromManDevSerial[6],i; 
  
 
  if (I2CEEPROMBufferRead(testBuffer,TableHeader1_Setting,10)!= 0)
  { 
	//------------------------
	// check the HEADER
	if ((testBuffer[0] == 'C')&&(testBuffer[1] == 'R')&&(testBuffer[2]=='E')&&
	    (testBuffer[3] == 'E')&&(testBuffer[4] == 'D')&&
		(testBuffer[6] == CONFIGDB_VER0)&&(testBuffer[8] == CONFIGDB_VER1)&&
		(testBuffer[9] == CONFIGDB_VER2))
	{
		 
	}
	else
	{
		//------------------------setup header and default values 
		testBuffer[0] = 'C';
		testBuffer[1] = 'R';
		testBuffer[2] = 'E';
		testBuffer[3] = 'E';
		testBuffer[4] = 'D';
		testBuffer[5] = 'V';
		testBuffer[6] = CONFIGDB_VER0;
		testBuffer[7] = '.';
		testBuffer[8] = CONFIGDB_VER1;
		testBuffer[9] = CONFIGDB_VER2; 					
		for (n=0;n<10;n++)
		{
			if (I2CEEPROMBufferWrite(&testBuffer[n],TableHeader1_Setting+n,1)!= 0)
			{
			}		
		}
		//----------------------------------------
		// place in defaults
		//---------------------------------------
		testBuffer[0] = 0x5A; 
		I2CEEPROMBufferWrite(testBuffer,Key_Setting,1);
		testBuffer[0] = 0x01; 
		I2CEEPROMBufferWrite(testBuffer,Mode_Setting,1);
		testBuffer[0] = 0x05; 
		I2CEEPROMBufferWrite(testBuffer,MaxForce_Setting,1);		
		
		//------------run time counters
		testBuffer[0] = 0x00; 
		I2CEEPROMBufferWrite(testBuffer,RT_Ctr_X_Setting,1);
		testBuffer[0] = 0x00; 
		I2CEEPROMBufferWrite(testBuffer,RT_Ctr_H_Setting,1);
		testBuffer[0] = 0x00; 
		I2CEEPROMBufferWrite(testBuffer,RT_Ctr_M_Setting,1);	
		testBuffer[0] = 0x00; 
		I2CEEPROMBufferWrite(testBuffer,RT_Ctr_L_Setting,1);	 		 
		
		testBuffer[0] = 0x00;
		I2CEEPROMBufferWrite(testBuffer,PairAddressMSB,1);
		testBuffer[0] = 0x00;
		I2CEEPROMBufferWrite(testBuffer,PairAddressLSB,1);		
		
		testBuffer[0] = 0x07;
		I2CEEPROMBufferWrite(testBuffer,ScreenColorSetting,1);
		testBuffer[0] = FALSE;
		I2CEEPROMBufferWrite(testBuffer,BackLightOnSetting,1);		
		testBuffer[0] = TRUE;
		I2CEEPROMBufferWrite(testBuffer,TempFarenheitOnSetting,1);		

		testBuffer[0] = 0x07;
		I2CEEPROMBufferWrite(testBuffer,ForceMaxSetting,1);
		testBuffer[0] = FALSE;
		I2CEEPROMBufferWrite(testBuffer,HybridSetting,1);		
		testBuffer[0] = FALSE;
		I2CEEPROMBufferWrite(testBuffer,ActiveBrakeEnableSetting,1);
		testBuffer[0] = FALSE;
		I2CEEPROMBufferWrite(testBuffer,TPMSEnableSetting,1);		
		testBuffer[0] = 0x00;
		I2CEEPROMBufferWrite(testBuffer,SensitivitySetting,1);		
	}
	  //------------------------------
	  // read table 0 values.
	  //------------------------------ 
	  for (n=0;n<28;n++)
	  {
		I2CEEPROMBufferRead( &value, (uint8_t)Key_Setting+n, 1);
		table0.Index[n] = value;   
	  }  	
  }
	for (n=0;n<6;n++)
	{
		eepromManDevSerial[n] = 0; 
	}
	if (I2CEEPROMBufferRead(eepromManDevSerial,0xFA,6)!= 0)
	{
		AppStatusUpdate(INTERFACE_EEPROM,STATUS_PARTTALKING,1);	
		for (n=0;n<6;n++)
		{
			table0.Item.EepromManDevSerial[n] =eepromManDevSerial[n] ;
		}	
	}
//  table1.Item.Firmware0_Setting = SW_VER0;
//  table1.Item.Firmware1_Setting = SW_VER1;
//  table1.Item.Firmware2_Setting = SW_VER3;
	for (i=0;i<MAXSENSORS;i++)
	{
		if (I2CEEPROMBufferRead((uint8_t *)&tableSensor.Item.WhichSensor[i],TableSensorStart + (i*4),4)!= 0)
		{ 
	  
		}
	}
}

void ConfigUpdate(uint8_t value,uint8_t settingAddress)
{
	I2CEEPROMBufferWrite(&value,settingAddress,1);	 
}

#if 0
/*************************************************************************
 * Function Name: UpdateRTCtr
 * Parameters: none
 *
 * Return: none
 *
 * Description: Calculates run time and updates the counter
 *
 *************************************************************************/

 uint32_t wtemp;
void UpdateRTCtr(void)
{
 

  wtemp = ReadSetting(RT_Ctr_X_Setting) << 16;
  wtemp += ReadSetting(RT_Ctr_H_Setting) << 8;
  wtemp += ReadSetting(RT_Ctr_L_Setting);
  wtemp += run_time_count;
  WriteSetting( RT_Ctr_L_Setting, (uint8_t)(wtemp & 0xFF));
  WriteSetting( RT_Ctr_H_Setting, (uint8_t)((wtemp >> 8)&0xff));
  WriteSetting( RT_Ctr_X_Setting, (uint8_t)((wtemp >> 16)&0xff));
  run_time_count = 0;
  
  wtemp = ReadSetting(Cycle_Ctr_H_Setting) << 8;
  wtemp += ReadSetting(Cycle_Ctr_L_Setting);
  wtemp += 1;
  WriteSetting( Cycle_Ctr_L_Setting, (uint8_t)(wtemp & 0xFF));
  WriteSetting( Cycle_Ctr_H_Setting, (uint8_t)(wtemp >> 8));
}


 
/*************************************************************************
 * Function Name: SetDefaultRemote
 * Parameters: none
 *
 * Return: none
 *
 * Description:  Stores the default ID into the Remote ID memory
 *                for manufacturing purposes.
 *
 *************************************************************************/

void SetDefaultRemote(void)
{
  uint8_t temp, i;
  uint16_t addr;
  
  // set entry to 0x555555
  for(i=0; i<3; i++)
  {
    temp = 0x55;
    addr = 0x0001 + i;
    I2C_EE_BufferWrite( &temp, addr, 1); 
  }
}

/*************************************************************************
 * Function Name: SetDefaultRemoteSpeeds
 * Parameters: none
 *
 * Return: none
 *
 * Description:  Stores the default speeds for the remote types
 *                
 *
 *************************************************************************/

void SetDefaultRemoteSpeeds(void)
{
//  uint8_t temp;
  uint16_t i;
  
  // set all min speeds to 1
  for(i=(uint16_t)RTYPE_1_MIN; i<=(uint16_t)RTYPE_13_MIN; i+=2)
    WriteSetting(i, 1);  

  // set all max speeds to 100
  for(i=(uint16_t)RTYPE_1_MAX; i<=(uint16_t)RTYPE_13_MAX; i+=2)
    WriteSetting(i, 100);  

}

/*************************************************************************
 * Function Name: ResetActiveFault
 * Parameters: code - Fault code
 *
 * Return: none
 *
 * Description:  Reset the Fault Active flag status and turn off the 
 *                 diag error LED
 *
 *************************************************************************/
void ResetActiveFault(Fault_Code code)
{
    active_fault &= ~(0x0001 << code); 
}

/*************************************************************************
 * Function Name: GetActiveFault
 * Parameters: code - Fault code
 *
 * Return: bool - True/False
 *
 * Description:  Get the active fault status of the requested fault
 *                  
 *
 *************************************************************************/
bool GetActiveFault(Fault_Code code)
{
    if (active_fault & (0x0001 << code))
      return TRUE;
    else
      return FALSE;
}

#endif