//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: driverAccel.c 
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
#include "driverASPI.h"
#include "appAccel.h"
#include "driverI2C.h"
#include "config.h"
#if BRAKEBOARD
#include "appMotor.h" 
#endif 

#define ACCEL_AVG
//---------------------GLOBAL VARIABLES-----------------------------------
 

//---------------------LOCAL VARIABLES------------------------------------
uint16_t accel_packet;  

#define MAX_ACCELEROMETER_BUFFER 20
int16_t xPositionBuffer[MAX_ACCELEROMETER_BUFFER];
int16_t yPositionBuffer[MAX_ACCELEROMETER_BUFFER];
int16_t zPositionBuffer[MAX_ACCELEROMETER_BUFFER];
uint8_t accelerometerOffset;
uint8_t acceleromterFirstFill; 
uint8_t accelerometerChange; 

#define MAX_DECISION_BUFFER 5
int16_t xDecisionBuffer[MAX_DECISION_BUFFER];
uint8_t xDecisionBufferOffset;
uint8_t xDecisionBufferFill; 

  //--------------------------------
  // single reading position in free-space
  //--------------------------------
int16_t position_x=0;
int16_t position_y=0;
int16_t position_z=0; 
  //--------------------------------
  // single reading position in free-space
  //--------------------------------
int16_t positionXaverage=0;
int16_t positionYaverage=0;
int16_t positionZaverage=0; 
 
int32_t sumx;
int32_t sumy;
int32_t sumz;
//---------------------LOCAL FUNCTION PROTOTYPES--------------------------  
uint8_t AccelDataAvailable(void);  
uint8_t AccelStatus(void);
void AccelReset(void);
uint8_t AccelWhoAmI(void); 

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:    
//------------------------------------------------------------------------------
// This function  
//==============================================================================
uint8_t AccelProvideDecisions(int16_t value, uint8_t direction,int16_t baseline)
{
	uint8_t status,i,done; 
	status = 0;
	int32_t temp,temp2;
	uint8_t gainin;

	gainin = table0.Item.MaxForce;
	if (xDecisionBufferFill != 0)
	{
		done = 0;
		for (i=0;i<MAX_DECISION_BUFFER;i++)
		{
			temp = xDecisionBuffer[i] - baseline;
			temp2 = gainin*10;
			temp2 = temp2/5; 
			temp = temp * temp2;
			temp = temp/10; 
			if (direction == DECISION_GREATER)
			{
				if (temp <value)
				{
					done = 1; 
				}
			}
			else
			{
				if (temp >value)
				{
					done = 1; 
				}				
			}
		}
		if (done == 0)
		{
			status = 1;
		}
	}
/*		
	if (acceleromterFirstFill!= 0)
	{
		*x = positionXaverage;
		*y = positionYaverage;
		*z = positionZaverage;
		status = 1; 
	}
*/	
	return status; 
}
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:    
//------------------------------------------------------------------------------
// This function  
//==============================================================================
uint8_t AccelProvideReadingChange(uint16_t *x, uint16_t *y, uint16_t *z,uint8_t *change)
{
	uint8_t status; 
	status = 0;
	 

 
	if (acceleromterFirstFill!= 0)
	{
		*x = positionXaverage;
		*y = positionYaverage;
		*z = positionZaverage;
		status = 1; 
		if (accelerometerChange!=0)
		{
			accelerometerChange = 0; 
			*change = 1; 
		}
	}
	return status; 
} 
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:    
//------------------------------------------------------------------------------
// This function  
//==============================================================================
uint8_t AccelProvideReading(uint16_t *x, uint16_t *y, uint16_t *z)
{
	uint8_t status; 
	status = 0;
	 

 
	if (acceleromterFirstFill!= 0)
	{
		*x = positionXaverage;
		*y = positionYaverage;
		*z = positionZaverage;
		status = 1; 
	}
	return status; 
}
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   AccelRead
//------------------------------------------------------------------------------
// This function Read a byte from the accelerometer. 
//==============================================================================
uint8_t AccelRead(uint8_t address)
{
	uint8_t tempBuffer[3],value; 

#if REMOTEBOARD
	value = 0; 	
	tempBuffer[0] = 0;
	if (SPIAInOut(address | 0x80,tempBuffer,1)!= 0)
	{
		value = tempBuffer[0]; 
	}
#endif 
#if BRAKEBOARD
	value = 0;
	tempBuffer[0] = 0;
	if (I2CAccelBufferRead(tempBuffer,address,1)!= 0)
	{
		value = tempBuffer[0];
	}
#endif
	return value;
} 
  

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   AccelWrite
//------------------------------------------------------------------------------
// This function will write a byte to the accelerometer
//==============================================================================
void AccelWrite(uint8_t address, uint8_t data)
{
	uint8_t tempBuffer[3];
 	tempBuffer[0] = data;
#if REMOTEBOARD	 
 	if (SPIAInOut(address,tempBuffer,1)!= 0)
 	{
 	}
#endif
#if BRAKEBOARD
	if (I2CAccelBufferWrite(tempBuffer,address,1)!= 0)
	{
		
	}
#endif	 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   AccelIinit
//------------------------------------------------------------------------------
// This function will Initialize the accelerometer
//==============================================================================
uint8_t AccelInit(void)
{
	uint8_t i; 
	//-----------------------------------------
	// Configure I/O for Accel Interrupt 
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // in pull up
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//  GPIO_Init(GPIOC, &GPIO_InitStructure);
 
  //------------------------------------------------
  // Turn on the sensor and Enable X, Y, and Z
  //------------------------------------------------
  AccelWrite(MEMS_CTRL_REG1, ( 0x90 | MEMS_Z_ENABLE
                                       | MEMS_Y_ENABLE | MEMS_X_ENABLE ));
  //-----------------------------------------
  // Set up the Interrupt pin configuration(s)
//  AccelWrite(MEMS_CTRL_REG3, ( MEMS_ICFG_FF_WU ));
	AccelWrite(0x23, ( 0x08 )); 
  //----------------------------------------------
  // Setup Interrupt 
  //----------------------------------------------
  // Enable interrupt generation from Y axis low, latch the interrupt
//  AccelWrite(MEMS_FF_WU_CFG, ( MEMS_LIR | MEMS_YLOW_IE));                   
  //--------------------------
  // Set the Threshold    
//  AccelWrite(MEMS_FF_WU_THS, (0x30)); // Y axis value must be below
  
  //------------------------------
  // Set the Event duration to immediate
 // AccelWrite(MEMS_FF_WU_DURATION, MEMS_DURATION_NONE);     
	for (i=0;i<MAX_ACCELEROMETER_BUFFER;i++)
	{
		xPositionBuffer[i]=0;
		yPositionBuffer[i]=0;
		zPositionBuffer[i]=0;
	}
	accelerometerOffset=0;
	acceleromterFirstFill=0;
	accelerometerChange=0;
	for (i=0;i<MAX_DECISION_BUFFER;i++)
	{
		xDecisionBuffer[MAX_DECISION_BUFFER]=0;
	}
	xDecisionBufferOffset=0;
	xDecisionBufferFill=0; 	
  //---------------------------------
  // Clear the interrupt latch
 // AccelRead(MEMS_FF_WU_SRC);    // Only use if int latched
 	AppStatusUpdate(INTERFACE_ACCELEROMETER,STATUS_PARTTALKING,0); 
	if (AccelWhoAmI()== 0x33)
	{
		AppStatusUpdate(INTERFACE_ACCELEROMETER,STATUS_PARTTALKING,1);		
	}
   return 1;
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   AccelProcess
//------------------------------------------------------------------------------
// This function will Process accelerometer data
//==============================================================================
void AccelProcess(void)
{
	uint16_t itemp,itemp2; 
	int32_t ltemp; 
	uint8_t i; 
  // Is new 3-axis accelerometer data is available?
  
  if(AccelDataAvailable())
  {
    //------------------------------
    // Get movement
    //------------------------------
    //----------------------------------------
    // Store instantaneous accelerometer measurements in averaging Array
	itemp = AccelRead(MEMS_OUT_X_H);
	itemp2 = AccelRead(MEMS_OUT_X_L);
	itemp = itemp <<8;
	itemp |= itemp2; 
	position_x  = itemp;
	
	itemp = AccelRead(MEMS_OUT_Y_H);
	itemp2 = AccelRead(MEMS_OUT_Y_L);
	itemp = itemp <<8;
	itemp |= itemp2; 	
	position_y  = itemp;
	
	itemp = AccelRead(MEMS_OUT_Z_H);
	itemp2 = AccelRead(MEMS_OUT_Z_L);
	itemp = itemp <<8;
	itemp |= itemp2; 	 
	position_z  = itemp;
 
	//--------------------------
	// place in buffer 
 	xPositionBuffer[accelerometerOffset]=position_x;
	yPositionBuffer[accelerometerOffset]=position_y;
	zPositionBuffer[accelerometerOffset]=position_z;
	accelerometerOffset++;
	if (accelerometerOffset >= MAX_ACCELEROMETER_BUFFER)
	{
	
		accelerometerOffset=0;
		acceleromterFirstFill=1; 
		accelerometerChange = 1; 
		 //-----------------------------  
		// Average the measurements
		sumx=sumy=sumz=0;
		for(i=0; i<MAX_ACCELEROMETER_BUFFER; i++)
		{
		  sumx += xPositionBuffer[i];
		  sumy += yPositionBuffer[i];
		  sumz += zPositionBuffer[i];
		}
		ltemp = sumx/MAX_ACCELEROMETER_BUFFER;
		positionXaverage = ltemp;
		ltemp = sumy/MAX_ACCELEROMETER_BUFFER;
		positionYaverage = ltemp;	
		ltemp = sumz/MAX_ACCELEROMETER_BUFFER;
		positionZaverage = ltemp;	
	}
	//-----------------fill decision buffer
	if (accelerometerOffset == 0)
	{
		xDecisionBuffer[xDecisionBufferOffset] = positionXaverage;
		xDecisionBufferOffset++;
		if (xDecisionBufferOffset >= MAX_DECISION_BUFFER)
		{
			xDecisionBufferOffset = 0; 
			xDecisionBufferFill=1; 	
		}
	}
  }
#if BRAKEBOARD  
  if (accelerometerChange != 0)
  {
	MotorBuildGetAcc();
  }
#endif  
}


 
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------LOCAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  HasAccelMoved
//------------------------------------------------------------------------------
// This function will determined if moved
//============================================================================== 
uint8_t HasAccelMoved(void)
{
	/*
	// Has the accelerometer moved?
	if( accel_x>=(position_x+X_THRESHOLD) | accel_x<=(position_x-X_THRESHOLD)
		| accel_y>=(position_y+Y_THRESHOLD) | accel_y<=(position_y-Y_THRESHOLD)
		| accel_z>=(position_z+Z_THRESHOLD) | accel_z<=(position_z-Z_THRESHOLD) )
	{
		return TRUE;
	}
*/
	return 0;
}
 

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   
//------------------------------------------------------------------------------
// This function will get the 
//============================================================================== 
uint8_t AccelWhoAmI(void)
{
  uint8_t x;
  //-------------------------
  // Get the Status Register
  //-------------------------
  x = AccelRead(0x0f);

  return x;
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  AccelStatus
//------------------------------------------------------------------------------
// This function will get the accelerometer status
//============================================================================== 
uint8_t AccelStatus(void)
{
  uint8_t x;
  //-------------------------
  // Get the Status Register
  //-------------------------
  x = AccelRead(MEMS_STATUS_REG);

  return x;
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  AccelReset
//------------------------------------------------------------------------------
// This function Reset the accelerometer
//============================================================================== 
void AccelReset(void)
{
  //----------------------
  // Read all of the axis values
  AccelRead(MEMS_OUT_X_H);
  AccelRead(MEMS_OUT_Y_H);
  AccelRead(MEMS_OUT_Z_H);
  //----------------------------
  // Read the interrupt flags
  AccelStatus();

}



//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  AccelDataAvailable
//------------------------------------------------------------------------------
// This function handles Check for Accelerometer data. Returns TRUE if data is 
// available (for all 3 axis), FALSE otherwise
//============================================================================== 
uint8_t AccelDataAvailable(void)
{
//  uint8_t x;

/*
  //----------------------------    
  // Get the Status Register
  //----------------------------
  x = AccelRead(MEMS_STATUS_REG);

  //-------------------------------------  
  // Is data available for all three axis?
  //-------------------------------------
  if(x & MEMS_ZYX_DATA_AVAIL)
  { 
    return 1;
  }
  return 0;
*/
	return 1;   
}




