#include <asf.h>
#include "dbrakeDefs.h"
#include "driverLCD.h"
#include "appLCD.h"
#include "driverTSPI.h"
#include "AppConversions.h"

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
//returns the number of digits. 0 to 3
uint8_t AppGetPressureConverted(uint8_t whichSensor,uint8_t *dataptr,uint8_t staticList)
{
	uint8_t numDigits; 
	uint16_t pressure,temperature,itemp; 
	
	numDigits = 0;
	if (PressureProvideData(whichSensor,&pressure,&temperature,staticList)!= 0)
	{
		if (pressure <1000)
		{
			dataptr[0] = ' ';
			dataptr[1] = ' ';
			dataptr[2] = ' ';
			if (pressure >= 100)
			{
				itemp = pressure/100;
				pressure = pressure - (itemp *100);
				dataptr[0] = itemp;
				 
			}
			if ((pressure >= 10)||(dataptr[0]!= ' '))
			{
				itemp = pressure/10;
				pressure = pressure - (itemp *10);
				dataptr[1] = itemp;				
			}
			itemp = pressure;
			dataptr[2] = itemp;		
			numDigits = 3;	
		}
	}
	else
	{
			dataptr[0] = ' ';
			dataptr[1] = ' ';
			dataptr[2] = ' ';		
	}
	return numDigits;
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
//returns the number of digits. 0 to 3
uint8_t AppGetTemperatureConverted(uint8_t whichSensor,uint8_t *dataptr,uint8_t staticList)
{
	uint8_t numDigits;
	uint16_t pressure,temperature,itemp;
	
	numDigits = 0;
	if (PressureProvideData(whichSensor,&pressure,&temperature,staticList)!= 0)
	{
		if (temperature <1000)
		{
			dataptr[0] = ' ';
			dataptr[1] = ' ';
			dataptr[2] = ' ';
			if (temperature >= 100)
			{
				itemp = temperature/100;
				temperature = temperature - (itemp *100);
				dataptr[0] = itemp;
			}
			if ((temperature >= 10)||(dataptr[0]!= ' '))
			{
				itemp = temperature/10;
				temperature = temperature - (itemp *10);
				dataptr[1] = itemp;
			}
			itemp = temperature;
			dataptr[2] = itemp;
			numDigits = 3;
		}
	}
	else
	{
			dataptr[0] = ' ';
			dataptr[1] = ' ';
			dataptr[2] = ' ';		
	}			
	return numDigits;
}
