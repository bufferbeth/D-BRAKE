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
#include "driverTSPI.h"
#include "appLCD.h"
#include "config.h"

#define TSLAVE_SELECT_PIN PIN_PB00
#define MAX_SPIT_BUFFER_SIZE 15



//---------------------GLOBAL VARIABLES----------------------------------- 


//---------------------LOCAL VARIABLES------------------------------------
//------------rf433Eeprom--
//--- holds the values in EEPROM on the RF chip
// 
#define RF433_MAXEEPROM 620
const uint8_t RF433EEPROM_SETTINGS[RF433_MAXEEPROM] = 
		{0x2a,0x07,0x02,0x68,0xdd,0x72,0x01,0x06,
		 0x00,0x02,0x00,0x00,0x00,0xc9,0x3c,0x08,
		 0x03,0x00,0x00,0x18,0x03,0x07,0x00,0x10,
		 0x03,0x32,0x40,0x10,0x00,0x85,0x00,0x00,
		 0x07,0x0c,0x0e,0x0f,0x0e,0x0c,0x09,0x05,
		 0x01,0xfc,0xf7,0xf3,0xef,0xeb,0xe9,0xe8,
		 0xe9,0xec,0xf0,0xf7,0x01,0x0d,0x00,0xff,
		 0xff,0xff,0xff,0x83,0xff,0xff,0xff,0xff,
		 0x83,0xff,0xff,0xff,0xff,0x83,0xff,0xff,
		 0xff,0xff,0x83,0xff,0xff,0xff,0xff,0x83, //48
		 0xff,0xff,0xff,0xff,0x83,0xff,0xff,0xff,
		 0xff,0x83,0xff,0xff,0xff,0xff,0x83,0xff, //58
		 0xff,0xff,0xff,0x83,0xff,0xff,0xff,0xff, 
		 0x83,0xff,0xff,0xff,0xff,0x83,0xff,0xff, //68
		 0xff,0xff,0x83,0xff,0xff,0xff,0xff,0x83, 
		 0xff,0xff,0xff,0xff,0x83,0xff,0xff,0xff, //78
		 0xff,0x83,0xff,0xff,0xff,0xff,0x83,0xff, 
		 0xff,0xff,0xff,0x83,0xff,0xff,0xff,0xff, //88
		 0x83,0x00,0x00,0x03,0x80,0xff,0xff,0x04, 
		 0xbf,0xc3,0x14,0x20,0x42,0x00,0x52,0x01, //98
		 0x62,0x00,0x52,0x00,0x62,0x01,0x42,0x00,
		 0x62,0x00,0x42,0x03,0x52,0x00,0x00,0x00, //a8
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x02,0x03,0x14,0x26,0x63, //b8
		 0xa3,0xc5,0x3a,0x26,0x28,0x28,0x1f,0x1f,
		 0x1f,0x1f,0x0f,0x0f,0x00,0x07,0x2b,0x10, //c8
		 0x07,0xaf,0x01,0xad,0x00,0xad,0x00,0x7e,
		 0x00,0x7e,0x00,0x95,0x0a,0x08,0x08,0x47, //d8  was 0x08 now 0xa2
		 0x51,0x00,0x40,0x31,0x20,0x96,0x55,0x99,
		 0x59,0xa9,0xaa,0xaa,0xaa,0x07,0x17,0x08, //e8
		 0x18,0x3f,0x3f,0x2f,0x2f,0xc9,0xd3,0xb6,
		 0xf6,0x60,0x70,0x96,0x04,0x7b,0x01,0x7b, //f8
		 0x01,0x01,0x00,0x01,0x00,0x00,0x00,0x00,
		 0x00,0xc9,0xd6,0x55,0x99,0x59,0x55,0xaa, //108
		 0xaa,0xaa,0xaa,0x08,0x16,0x07,0x15,0x1d,
		 0x00,0xff,0xff,0x00,0x00,0x00,0x1d,0x00, //118
		 0xff,0xff,0x00,0x00,0x00,0xaa,0x72,0x52,
		 0xcf,0x2f,0x00,0x00,0x00,0x00,0x1d,0x00, //128
		 0x1d,0x00,0x00,0x00,0xff,0xff,0x00,0x10,
		 0x08,0xce,0x6a,0x02,0x85,0x03,0xce,0x6a, //138 
		 0x02,0x85,0x03,0xce,0x6a,0x02,0x85,0x03,
		 0x02,0x03,0x14,0x42,0x42,0x8b,0xcb,0x1a, //148
		 0x1a,0x23,0x23,0x1f,0x1f,0x1f,0x1f,0x0f, 
		 0x0c,0x00,0x07,0x2b,0x10,0x07,0xaf,0x01, //158
		 0xaf,0x01,0xaf,0x01,0x34,0x00,0x34,0x00, 
		 0x8e,0x0a,0x00,0x08,0x41,0x40,0x5e,0x40, //168
		 0x11,0x20,0x56,0x55,0x55,0x55,0xa9,0xaa, 
		 0xaa,0xaa,0x0f,0x11,0x10,0x12,0x3f,0x3f, //178
		 0x2f,0x2f,0xc9,0xd3,0xb6,0xf6,0x60,0x60,
		 0x95,0x00,0x9d,0x00,0x9d,0x00,0x01,0x00, //188oooo
		 0x01,0x00,0x00,0x00,0x00,0x00,0xc9,0xd6, 
		 0x55,0x55,0x55,0x55,0xaa,0xaa,0xaa,0xaa, //198
		 0x0e,0x10,0x0d,0x0f,0x1d,0x00,0xff,0xff,
		 0x00,0x00,0x00,0x1d,0x00,0xff,0xff,0x00, //1a8
		 0x00,0x00,0xaa,0x72,0x50,0xcf,0x20,0x00,
		 0x00,0x00,0x00,0x1d,0x00,0x1d,0x00,0x00, //1b8
		 0x00,0x00,0x00,0x00,0x00,0x08,0xb3,0x40, 
		 0x02,0x85,0x03,0x55,0x7d,0x02,0x85,0x03, //1c8
		 0x04,0x5f,0x02,0x85,0x03,0x02,0x03,0x14, 
		 0x63,0x63,0xc7,0x87,0x10,0x10,0x06,0x06, //1d8
		 0x1f,0x1f,0x1f,0x1f,0x0f,0x0f,0x00,0x07, 
		 0x23,0x10,0x07,0xaf,0x01,0xd8,0x00,0xd8, //1e8
		 0x00,0x64,0x00,0x64,0x00,0x90,0x0a,0x18,
		 0x08,0x00,0x40,0x00,0x40,0x11,0x20,0x56, //1f8
		 0x55,0x55,0x55,0xa9,0xaa,0xaa,0xaa,0x0f, 
		 0x0f,0x10,0x10,0x3f,0x3f,0x2f,0x2f,0xd6, //208
		 0xd6,0xf6,0xf6,0x80,0x80,0x96,0x0c,0x2f, 
		 0x01,0x2f,0x01,0x01,0x01,0x05,0x05,0x02, //218
		 0x00,0x00,0x00,0xd6,0xd6,0x55,0x55,0x55, 
		 0x55,0xaa,0xaa,0xaa,0xaa,0x0e,0x0e,0x0d, //228
		 0x0d,0x1d,0x00,0xff,0xff,0x00,0x00,0x00,
		 0x1d,0x00,0xff,0xff,0x00,0x00,0x00,0x22, //238
		 0x02,0x02,0x2f,0x2f,0x00,0x00,0x00,0x00, 
		 0x1d,0x00,0x1d,0x00,0xff,0xff,0xff,0xff, //248
		 0x00,0x00,0x08,0x11,0x4c,0x02,0x85,0x03,
		 0xf7,0x71,0x02,0x85,0x03,0x04,0x5f,0x02, //258
		 0x85,0x03,0x80,0x04,0x00,0x00,0x00,0x3f,
		 0x00,0x3f,0x00,0x3f
/*						,0x00,0x00,0x00,0x00,0x00, //268
// location x026d  to 0x27f   		 
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //278
//---------------------------		 
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //288
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //298
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //2a8		 
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //2b8		 
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //2c8		 
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //2d8		 		 		 
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //2e8	
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //2f8
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //308
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //318
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //328
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //338
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //348
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //358		 
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //368
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //378
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //388
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //398
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //3a8
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //3b8
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //3c8		 
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //3d8
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //3e8
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
		 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  //3f8		 
		*/ 		
		};
	
#define MAX_TEST_STATUS 8	
uint8_t nextTestStatus = 0;
uint8_t testStatus[MAX_TEST_STATUS]; 	
				
uint8_t rf433Eeprom[RF433_MAXEEPROM]; 

uint8_t txBuffT[MAX_SPIT_BUFFER_SIZE];
uint8_t rxBuffT[MAX_SPIT_BUFFER_SIZE]; 
 
struct spi_module spit_master_instance;
struct spi_slave_inst slavet;


uint8_t tpsIRQ;
uint8_t tpsEvents[5];
uint8_t tpsRXLevel;
uint8_t systemChange;
uint8_t	eventsChange;
uint8_t powerChange;
uint8_t tpsRXBuffer[30];
uint8_t tempBuffer[20];
uint8_t tpsCRC;

#define MAXRF433BUILDBUFFER 20
uint8_t rf433BuildBuffer[MAXRF433BUILDBUFFER];
uint8_t rf433BuildOffset;

//SENSORDATA sensorPlaced[MAXSENSORS]; 
SENSORDATA sensorDynamic[MAXSENSORS];

	//---------------RSSI processing
#define MAX_RSSI_BUFFER 4
uint8_t rssiOffset;
uint8_t rssiBuffer[MAX_RSSI_BUFFER]; 
uint8_t tpsRSSIAvg = 0;
uint8_t tpsRSSIPeak = 0;


//---------------------LOCAL FUNCTION PROTOTYPES--------------------------   
void SPITConfigure(void);
int SPITInOut(uint8_t *buffer,uint8_t size);
void RF433ReadEeprom(uint16_t address,uint8_t length,uint8_t *readBuffer);
void RF433WriteEeprom(uint16_t address,uint8_t length,const uint8_t *writeBuffer); 
void RF433ResetROM(void);
void RF433SetSystemMode(void);
void RF433ReadEvents(void);
void RF433ReadRXLevel(void);
void RF433ReadRXBuffer(uint8_t *buffer,uint8_t length);
uint8_t crc8 ( uint8_t *data_in, uint16_t number_of_bytes_to_read );
uint8_t CRC8(uint8_t *data, uint8_t len);
void RF433ProcessPacket(uint8_t length); 
void extint_detection_callback(void);
void configure_extint_channel(void);
void configure_extint_callbacks(void);
void RF433ReadRssiBuffer(uint8_t *buffer,uint8_t length);
void RF433StartRSSI(void);
void RF433GetRSSI(void);

void PressureProvidRSSI(uint8_t *rssiPeak,uint8_t *rssiAvg)
{
	RF433GetRSSI();
	*rssiPeak = tpsRSSIPeak;
	*rssiAvg = tpsRSSIAvg;
} 

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function
//============================================================================== 	
uint8_t PressureProvideData(uint8_t which,uint16_t *pressure,uint16_t *temperature,uint8_t staticList)
{
	uint8_t status,done,i,sid0,sid1,sid2,id0,id1,id2; 
	status = 0; 

	if (staticList == 0)
	{
		if ((which < MAXSENSORS)&&
	    ((sensorDynamic[which].ID[0]!= 0)||(sensorDynamic[which].ID[1]!= 0)||(sensorDynamic[which].ID[2]!=0)))
		{
			*pressure = sensorDynamic[which].Pressure;
			*temperature = sensorDynamic[which].Temperature; 	
			status = 1; 
		}
	}
	else
	{
		//--------------------- which points now to position in static list. 
		// see if you can find the date in the dynamic list that matches the 
		// same ID and that is the values you return. 
		sid0 = tableSensor.Item.WhichSensor[which].ID[0];
		sid1 = tableSensor.Item.WhichSensor[which].ID[1];
		sid2 = tableSensor.Item.WhichSensor[which].ID[2];	
		done = 0; 
		i = 0; 
		while ((done == 0) && (i<MAXSENSORS))
		{
			id0 = sensorDynamic[i].ID[0];
			id1 = sensorDynamic[i].ID[1];
			id2 = sensorDynamic[i].ID[2];
			if ((sid0==id0)&&(sid1==id1)&&(sid2==id2))
			{
				done = 1; 
				*pressure = sensorDynamic[i].Pressure;
				*temperature = sensorDynamic[i].Temperature;
				status = 1;				
			}
			i++;
		}
	}
	return status; 
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function
//============================================================================== 	
void PressureUpdateTask(void)
{
	uint8_t i;
	uint16_t itemp,itemp2; 
	uint32_t ltemp; 
		
	for (i=0;i<MAXSENSORS;i++)
	{
		if ((sensorDynamic[i].Change != 0)&&(sensorDynamic[i].ID[0]!= 0))
		{
			//------------------------
			// following for testing status values on the sensors
			// usually this should be disabled. 
			if (nextTestStatus >= MAX_TEST_STATUS)
			{
				nextTestStatus= 0; 
			}			
			testStatus[nextTestStatus++] = sensorDynamic[i].LastPacket[5]; 
			//-------------------------------- 				
			sensorDynamic[i].Change = 0;
			//--------------------------
			// handle the temperature 
			// * subtract 40 and you get the Celcius
			sensorDynamic[i].Temperature = sensorDynamic[i].LastPacket[4];
			sensorDynamic[i].Temperature -= 40; 
			//---------------------------
			// handle the Pressure 
			// 
			sensorDynamic[i].Pressure = 0;
			itemp = sensorDynamic[i].LastPacket[5]<<4;
			itemp &= 0x0700;
			itemp2 = sensorDynamic[i].LastPacket[3];
			itemp |= itemp2; 
			sensorDynamic[i].Pressure = itemp;
			ltemp = itemp; 
			ltemp = ltemp * 145038;
			ltemp = ltemp/1000000;
			itemp = ltemp;
			sensorDynamic[i].Pressure = itemp; 	
#if REMOTEBOARD			
#if REMOTE_TIREON
			AppScreenCarRadioReadingIn(i,TRUE);
			AppScreenPlacePressure();		
#endif
#endif
#if BRAKEBOARD 
//			AppRadioCarRadioReadingIn(i,TRUE);
#endif				
		}
	}		
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function
//============================================================================== 
void RF433Task(void)
{	
		RF433ReadEvents();
		
		systemChange = 0;
		eventsChange = 0;
		powerChange = 0;
		if (tpsEvents[0] != 0)
		{
			systemChange = 1;
			if ((tpsEvents[0] & 0x04)!= 0)
			{
/*				
				//-----------------SFIFO ... READ THE RSSI VALUE
				RF433ReadRssiBuffer(tempBuffer,2);
				if (rssiOffset >= MAX_RSSI_BUFFER)
				{
					rssiOffset = 0; 
				}
				rssiBuffer[rssiOffset++] = tempBuffer[3];
				if (rssiOffset >= MAX_RSSI_BUFFER)
				{
					rssiOffset = 0; 
				}				
				rssiBuffer[rssiOffset++] = tempBuffer[4];
*/				
			}
		}
		if (tpsEvents[1] != 0)
		{
			eventsChange = 1;
			if ((tpsEvents[1] & 0x10)!= 0)
			{
				RF433ReadRXLevel();
				while (tpsRXLevel >0)
				{
					if (tpsRXLevel >10)
					{
						RF433ReadRXBuffer(tempBuffer,10);
						RF433ProcessPacket(10);
						tpsRXLevel -= 10;
					}
					else
					{
						RF433ReadRXBuffer(tempBuffer,tpsRXLevel);
						RF433ProcessPacket(tpsRXLevel);
						tpsRXLevel = 0;
					}
				}
			}
		}
		if (tpsEvents[2] != 0)
		{
			powerChange = 1;
		}	
}
	 		
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: RF433Init
//------------------------------------------------------------------------------
// This function
//============================================================================== 
void RF433Init(void)
{
	uint16_t address,i,j;
	uint8_t done; 
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);
		
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(TPS_POWERON, &pin_conf);
	port_pin_set_output_level(TPS_POWERON, false); 
	port_pin_set_output_level(TPS_POWERON, true);    
   
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(TPS_IRQ, &pin_conf);			
	tpsIRQ = port_pin_get_input_level(TPS_IRQ);
	
	SPITConfigure();
	
	rf433BuildOffset = 0;
	for (i=0;i<MAXSENSORS;i++)
	{ 
		sensorDynamic[i].ID[0] = 0x00;
		sensorDynamic[i].ID[1] = 0x00;
		sensorDynamic[i].ID[2] = 0x00;				
		sensorDynamic[i].State = 0x00;
	}
//	sensor[0].ID = 0xD7;
//	sensor[1].ID = 0xDA;

	//---- rssi PROCESSING
	rssiOffset=0;
	for (i=0;i<MAX_RSSI_BUFFER;i++)
	{
		rssiBuffer[i]=0; 	
	}

	//------------------------- 
	// Following tests the CRC calcualtion
	// result should be 0xe3.
	//------------------------
	/*
	tpsRXBuffer[0] = 0xfd; 
	tpsRXBuffer[1] = 0x5f; 
	tpsRXBuffer[2] = 0xd5; 
	tpsRXBuffer[3] = 0xfd; 
	tpsRXBuffer[4] = 0x5f; 
	tpsRXBuffer[5] = 0xd5; 
	tpsRXBuffer[6] = 0xf6;	
	tpsRXBuffer[7] = 0x03; 
	tpsRXBuffer[8] = 0xd7; 
	tpsRXBuffer[9] = 0x0d; 
	tpsRXBuffer[10] = 0x00; 
	tpsRXBuffer[11] = 0x43; 
	tpsRXBuffer[12] = 0x02; 
	tpsRXBuffer[13] = 0xe3;
	tpsCRC = CRC8 ( &tpsRXBuffer[7],6);	
	*/
	//-----------------------------
	// Read 10 bytes and then compare 
	// against the required configuration.
	// if they don't match - just write those 10 bytes. 
	//-----------------------------
	address = 0x0000;
	for (j=0;j<61;j++)  //was 62
	{
		RF433ReadEeprom(address,10,&rf433Eeprom[address]) ;  //200); //RF433_MAXEEPROM = 620
		done = 0; 
		i = 0; 
		while ((done==0) &&(i<10))
		{
			if (rf433Eeprom[address+i] != RF433EEPROM_SETTINGS[address+i])
			{
				done = 1; 
			}
			i++;
		}
		if (done != 0)
		{
			RF433WriteEeprom(address,10,&RF433EEPROM_SETTINGS[address]) ;  
		}
		address += 10; 
	}
	//-------------------------------------------
	// check the integrity for status update test
	address = 0x0000;
	RF433ReadEeprom(address,10,&rf433Eeprom[address]) ;  //200); //RF433_MAXEEPROM = 620
	done = 0;
	i = 0;
	while ((done==0) &&(i<10))
	{
		if (rf433Eeprom[address+i] != RF433EEPROM_SETTINGS[address+i])
		{
			done = 1;
		}
		i++;
	}
	if (done != 0)
	{
		//-----------------------------
		// no good 
		AppStatusUpdate(INTERFACE_TIRERADIO,STATUS_PARTTALKING,0);
	}
	else
	{
		//-----------------------------
		// good
		AppStatusUpdate(INTERFACE_TIRERADIO,STATUS_PARTTALKING,1);
	}	
 	
	
	
	RF433ResetROM();
	RF433SetSystemMode();
	RF433ReadEvents();
//	port_pin_set_output_level(TPS_POWERON, false); 	
	RF433SetSystemMode();	
	RF433ReadEvents();
		
//BETH 09 13 15	RF433StartRSSI();
	//---------------------------
	// enable the rf433 interrupt
	configure_extint_channel();
	configure_extint_callbacks();
//
//	system_interrupt_enable_global();


/*	
	while (1)
	{
	port_pin_set_output_level(TPS_POWERON, true); 	
	RF433ReadEvents();		
	port_pin_set_output_level(TPS_POWERON, false); 	
	RF433ReadEvents();		
	}
*/	
/*
	while (1)
	{
		tpsIRQ = port_pin_get_input_level(TPS_IRQ);		

	}
*/	
} 

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function
//============================================================================== 
void configure_extint_channel(void)
{
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin = PIN_PB31A_EIC_EXTINT15;
	config_extint_chan.gpio_pin_mux =  MUX_PB31A_EIC_EXTINT15;
	config_extint_chan.gpio_pin_pull = EXTINT_PULL_UP;
	config_extint_chan.detection_criteria = EXTINT_DETECT_BOTH;
	extint_chan_set_config(15, &config_extint_chan);  //15
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function
//============================================================================== 
void configure_extint_callbacks(void)
{
	extint_register_callback(extint_detection_callback,15,EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(15,EXTINT_CALLBACK_TYPE_DETECT);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function
//============================================================================== 
void extint_detection_callback(void)
{
//	bool pin_state = port_pin_get_input_level(TPS_IRQ);
	schedByte |= SCHEDBYTE_RF433;
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function
//============================================================================== 
void RF433ProcessPacket(uint8_t length)
{
	uint8_t done,i,j;
	//----------------------------
	// add the bytes to the end of the 
	// current build Buffer. 	
	if ((rf433BuildOffset+length)>=MAXRF433BUILDBUFFER)
	{
		rf433BuildOffset = 0;
	}
	for (i=0;i<length;i++)
	{
		rf433BuildBuffer[rf433BuildOffset++] = tpsRXBuffer[i];;
	}
	//-----------------------
	// look for a valid packet.
	// that is 7 bytes - where 6 have same CRC
	while (rf433BuildOffset >= 7)
	{
		tpsCRC = CRC8 (&rf433BuildBuffer[0],6);
		if (tpsCRC == rf433BuildBuffer[6])
		{
			AppStatusUpdate(INTERFACE_TIRERADIO,STATUS_RXPACKET,1);
			AppStatusUpdate(INTERFACE_TIRERADIO,STATUS_COMMGOOD,1);
			//-------------------------
			// look at the ID of the packet to see if in the sensor list.
			//-------------------------
			i = 0;
			done = 0; 
			while ((i<MAXSENSORS)&&(done==0))
			{
				if ((sensorDynamic[i].ID[1] == rf433BuildBuffer[1])&&(sensorDynamic[i].ID[0] == rf433BuildBuffer[0])&&
					(sensorDynamic[i].ID[2] == rf433BuildBuffer[2]))
				{
					//-----------------------
					// load the packet in
					// mark if a value has changed 
					//-----------------------
					for (j=0;j<7;j++)
					{
						if (sensorDynamic[i].LastPacket[j] != rf433BuildBuffer[j])
						{
							sensorDynamic[i].NewValue =1; 
						}
						sensorDynamic[i].LastPacket[j] = rf433BuildBuffer[j];
					}
					sensorDynamic[i].Change = 1; 
					schedByte |= SCHEDBYTE_UPDATEPRESSURE;
					done = 1; 
				}
				i++;
			}
			if (done == 0)
			{
				//------add the sensor to the list until configuration is in place.
				//------------------------------------------------------
				i = 0;
				while ((i<MAXSENSORS)&&(done==0))
				{
					if ((sensorDynamic[i].ID[0] == 0)&&(sensorDynamic[i].ID[1]==0))
					{
						//-----------------------
						// load the packet in
						//-----------------------
						for (j=0;j<7;j++)
						{
							sensorDynamic[i].LastPacket[j] = rf433BuildBuffer[j];
						}
						sensorDynamic[i].Change = 1;
						sensorDynamic[i].ID[0] = rf433BuildBuffer[0]; 
						sensorDynamic[i].ID[1] = rf433BuildBuffer[1]; 
						sensorDynamic[i].ID[2] = rf433BuildBuffer[2]; 												
						schedByte |= SCHEDBYTE_UPDATEPRESSURE;
						done = 1; 
					}
					i++;
				}				 
				
			}
			rf433BuildOffset = rf433BuildOffset-7;
		}	
		else
		{
			rf433BuildOffset--;
			for (i=0;i<rf433BuildOffset;i++)
			{
				rf433BuildBuffer[i]= rf433BuildBuffer[i+1];
			}	
		}		
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void SPITConfigure(void)
{
 
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;
 
	//-----------------------------------
	// Configure and initialize software device 
	// instance of peripheral slave 
	//-----------------------------------
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = TSLAVE_SELECT_PIN;
	spi_attach_slave(&slavet, &slave_dev_config);
	//------------------------------------
	// Configure, initialize and enable SERCOM SPI module 
	//------------------------------------
	spi_get_config_defaults(&config_spi_master);
	config_spi_master.mux_setting = EXT3_SPI_SERCOM_MUX_SETTING;

	/* Configure pad 0 for data in */
	config_spi_master.pinmux_pad0 = EXT3_SPI_SERCOM_PINMUX_PAD0;
	/* Configure pad 1 as unused */
	config_spi_master.pinmux_pad1 = EXT3_SPI_SERCOM_PINMUX_PAD1;
	/* Configure pad 2 for data out */
	config_spi_master.pinmux_pad2 = PINMUX_UNUSED;
	/* Configure pad 3 for SCK */
	config_spi_master.pinmux_pad3 = EXT3_SPI_SERCOM_PINMUX_PAD3;
	spi_init(&spit_master_instance, EXT3_SPI_MODULE, &config_spi_master);
	spi_enable(&spit_master_instance);
 
}
 

 //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
 // FUNCTION:
 //------------------------------------------------------------------------------
 // This function
 //==============================================================================
 int SPITInOut(uint8_t *buffer,uint8_t size)
 {
	 uint8_t i,*ptr; 
	 ptr = buffer; 
	 int success;
	 
	 success = 0; 
	 
	 if (size < (MAX_SPIT_BUFFER_SIZE-1))
	 {
		 success = 1; 
		 spi_select_slave(&spit_master_instance, &slavet, true);
		 for (i=0;i<size;i++)
		 {
			txBuffT[i] = *ptr++;
		 }
		 spi_transceive_buffer_wait(&spit_master_instance,&txBuffT[0], &rxBuffT[0],size);
		 ptr = buffer; 
		 for (i=0;i<size;i++)
		 {
			 *ptr++ = rxBuffT[i]; 
		 }
		 
		 spi_select_slave(&spit_master_instance, &slavet, false);
	 } 
	 return success; 
 }
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  
//------------------------------------------------------------------------------
// This function
//============================================================================== 
 uint8_t CRC8(uint8_t *data, uint8_t len) {
	 uint16_t tempI,extract,sum;
	 uint16_t crc = 0x00;
	 while (len--) {
		 extract = *data++;
		 for (tempI = 8; tempI; tempI--) {
			 sum = (crc ^ extract) & 0x80; //01;
			 crc <<=1;  //>>= 1;
			 if (sum) {
				 crc ^= 0x31;  //8C;
			 }
			 extract <<=1; //>>= 1;
		 }
	 }
	 return crc;
 }
 
 //xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void RF433ReadRssiBuffer(uint8_t *buffer,uint8_t length)
{
	uint8_t i; 
	
	for (i=0;i<length;i++)
	{
		buffer[i] = 0x00;
	}
	buffer[0] = 0x05;
	buffer[1] = length;
	SPITInOut(buffer,length+3);
}  
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void RF433ReadRXBuffer(uint8_t *buffer,uint8_t length)
{
	uint8_t i; 
	
	for (i=0;i<length;i++)
	{
		buffer[i] = 0x00;
	}
	buffer[0] = 0x06;
	buffer[1] = length;
	SPITInOut(buffer,length+3);
	//--------------------------
	// data is in the 4th byte on
	//--------------------------
	for (i=0;i<length;i++)
	{
		tpsRXBuffer[i] = buffer[i+3];
	}  
}  
 
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void RF433ReadRXLevel(void)
{
	uint8_t commandBuffer[3];
	
	commandBuffer[0] = 0x01;
	commandBuffer[1] = 0x00;
	commandBuffer[2] = 0x00;
	SPITInOut(commandBuffer,3);
	//--------------------------
	// data is in the 5th byte
	//--------------------------
 	tpsRXLevel = commandBuffer[2];
} 
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   
//------------------------------------------------------------------------------
// This function
//==============================================================================
void RF433ReadEvents(void)
{
	uint8_t commandBuffer[5];
	uint8_t i; 
	
	commandBuffer[0] = 0x04;
	commandBuffer[1] = 0x00;  	
	commandBuffer[2] = 0x00; 
	commandBuffer[3] = 0x00;
	commandBuffer[4] = 0x00; 
	SPITInOut(commandBuffer,4);
	//--------------------------
	// data is in the 5th byte 
	//--------------------------
	for (i=0;i<4;i++)
	{
		tpsEvents[i] = commandBuffer[i];
	}
}
 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   
//------------------------------------------------------------------------------
// This function
//==============================================================================
void RF433ReadEeprom(uint16_t address,uint8_t length,uint8_t *buffer)
{
	uint8_t commandBuffer[5],temp;
	uint16_t itemp,newaddress;  
	uint8_t i; 
	
	newaddress = address; 
	for (i=0;i<length;i++)
	{
		commandBuffer[0] = SigmaX_eepromread;
		itemp = newaddress>>8;
		temp = itemp;  
		commandBuffer[1] = temp;  	
		itemp = newaddress & 0x00ff; 
		temp = itemp; 
		commandBuffer[2] = temp; 
		commandBuffer[3] = 0x00;
		commandBuffer[4] = 0x00; 
		SPITInOut(commandBuffer,5);
		//--------------------------
		// data is in the 5th byte 
		//--------------------------
		if ((i+address)<RF433_MAXEEPROM)
		{
			rf433Eeprom[i+address] = commandBuffer[4];
		}
		newaddress++;
	}	
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void RF433WriteEeprom(uint16_t address,uint8_t length,const uint8_t *buffer)
{
	uint8_t commandBuffer[5],temp,*ptr;
	uint16_t itemp,newaddress,delay;
	uint8_t i;
	
	ptr = (uint8_t *)buffer;
	newaddress = address;
	for (i=0;i<length;i++)
	{
		commandBuffer[0] = SigmaX_eepromwrite;
		itemp = newaddress>>8;
		temp = itemp;
		commandBuffer[1] = temp;
		itemp = newaddress & 0x00ff;
		temp = itemp;
		commandBuffer[2] = temp;
		commandBuffer[3] = *ptr++;
		SPITInOut(commandBuffer,4);
		newaddress++;
		for(delay=0;delay<0x7fff;delay++);
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: RF433ResetROM
//------------------------------------------------------------------------------
// This function
//==============================================================================
void RF433ResetROM(void)
{
	uint8_t commandBuffer[2];	
 
	commandBuffer[0] = 0x10;
	commandBuffer[1] = 0x00;
 	SPITInOut(commandBuffer,2);
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: RF433SetSystemMode  
//------------------------------------------------------------------------------
// This function
//==============================================================================
void RF433SetSystemMode(void)
{
	uint8_t commandBuffer[3];	
 
	commandBuffer[0] = 0x0D;
	commandBuffer[1] = 0xF2;
	commandBuffer[2] = 0x40;	
 	SPITInOut(commandBuffer,3);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION: RF433SetSystemMode
//------------------------------------------------------------------------------
// This function
//==============================================================================
void RF433StartRSSI(void)
{
	uint8_t commandBuffer[3];
	
	commandBuffer[0] = 0x1B;
	commandBuffer[1] = 0x40;
	SPITInOut(commandBuffer,2);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void RF433GetRSSI(void)
{
	uint8_t commandBuffer[4];
	
	commandBuffer[0] = 0x1C;
	commandBuffer[1] = 0x00;
	commandBuffer[2] = 0x00;
	commandBuffer[3] = 0x00;
	SPITInOut(commandBuffer,4);
	//--------------------------
	// data is in the 5th byte
	//--------------------------
	tpsRSSIAvg = commandBuffer[2];
	tpsRSSIPeak = commandBuffer[3];
}





