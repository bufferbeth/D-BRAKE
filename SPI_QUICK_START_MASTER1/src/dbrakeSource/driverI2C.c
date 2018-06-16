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
#include "driveri2c.h"
#include "config.h"

#define SLAVE_ADDRESS 0x50  //0x12
#define TIMEOUT 1000

//---------------------GLOBAL VARIABLES-----------------------------------


//---------------------LOCAL VARIABLES------------------------------------
struct i2c_master_module i2c_master_instance;

#define DATA_LENGTH 20
uint8_t write_buffer[DATA_LENGTH];
uint8_t read_buffer[DATA_LENGTH];
uint8_t i2cError; 


uint8_t eepromManDevSerial[6]; 

	//--------------master packet
struct i2c_master_packet myI2Cpacket;
	
//---------------------LOCAL FUNCTION PROTOTYPES--------------------------   
void configure_i2c_master(void);

void RecoverI2C(void)
{
	uint8_t i; 
	struct port_config pin_conf;
 
	i2c_master_disable(&i2c_master_instance);
 
	port_get_config_defaults(&pin_conf);
	
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA12, &pin_conf);
	port_pin_set_output_level(PIN_PA12, FALSE);
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA13, &pin_conf);
	port_pin_set_output_level(PIN_PA13, FALSE);
		
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA13, &pin_conf);
	port_pin_set_output_level(PIN_PA13, TRUE);		
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA12, &pin_conf);
	port_pin_set_output_level(PIN_PA12, TRUE);		
	
	for (i=0;i<16;i++)
	{
		HardDelay();
		pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
		port_pin_set_config(PIN_PA13, &pin_conf);
		port_pin_set_output_level(PIN_PA13, FALSE);		
		HardDelay();
		pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
		port_pin_set_config(PIN_PA13, &pin_conf);
		port_pin_set_output_level(PIN_PA13, TRUE);		
	}
	
	configure_i2c_master();
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
uint8_t I2CAccelBufferRead(uint8_t *buffer, uint8_t setting, uint8_t count)
{
	uint8_t status,tries,done; 
	
	status = 0; 
	 
	//---------------------------------------
	// Set-up packet for transmitting
	//---------------------------------------
	myI2Cpacket.address = 0x19;
	myI2Cpacket.data = write_buffer;
	myI2Cpacket.ten_bit_address = false;
	myI2Cpacket.high_speed = false;
	myI2Cpacket.hs_master_code  = 0x00;	
	myI2Cpacket.data_length = 1;
	
	write_buffer[0] = setting;
	tries = 0; 
	done = 0; 
	while ((done ==0) &&(tries<200))
	{
		if (i2c_master_write_packet_wait(&i2c_master_instance, &myI2Cpacket)==STATUS_OK) 
		{
			done = 1; 
		}
		else
		{
			tries++;
		}
	}
	if (done == 1)
	{
		//-------------------------------
		// Read from slave until success. 
		myI2Cpacket.data_length = count;
		myI2Cpacket.data = buffer;
		tries = 0;
		done = 0;
		while ((done ==0) &&(tries<200))
		{
			if (i2c_master_read_packet_wait(&i2c_master_instance, &myI2Cpacket)==STATUS_OK)
			{
				done = 1;
			}
			else
			{
				tries++;
			}
		}
		if (done == 1)
		{
			status = 1; 
		}
	}
	if (status == 0)
	{
		i2cError = 1; 
		RecoverI2C();
	}
	return status; 	
}; 

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
uint8_t I2CAccelBufferWrite(uint8_t *buffer,uint8_t setting, uint8_t count)
{
	uint8_t status,tries,done,*ptr,i;
	
	status = 0;
	ptr = buffer; 
	//---------------------------------------
	// Set-up packet for transmitting
	//---------------------------------------
	myI2Cpacket.address = 0x19;
	myI2Cpacket.data = write_buffer;
	myI2Cpacket.ten_bit_address = false;
	myI2Cpacket.high_speed = false;
	myI2Cpacket.hs_master_code  = 0x00;
	myI2Cpacket.data_length = count+1;
	
	write_buffer[0] = setting;
	for (i=0;i<count;i++)
	{
		write_buffer[1+i] = *ptr++;
	}
	tries = 0;
	done = 0;
	while ((done ==0) &&(tries<200))
	{
		if (i2c_master_write_packet_wait(&i2c_master_instance, &myI2Cpacket)==STATUS_OK)
		{
			done = 1;
		}
		else
		{
			tries++;
		}
	}
	if (done == 1)
	{
		status = 1;
	}
	if (status == 0)
	{
		i2cError = 1; 
		RecoverI2C();
	}	
	return status;
};

 




//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
uint8_t I2CEEPROMBufferRead(uint8_t *buffer, uint8_t setting, uint8_t count)
{
	uint8_t status,tries,done; 
	
	status = 0; 
	 
	//---------------------------------------
	// Set-up packet for transmitting
	//---------------------------------------
	myI2Cpacket.address = SLAVE_ADDRESS;
	myI2Cpacket.data = write_buffer;
	myI2Cpacket.ten_bit_address = false;
	myI2Cpacket.high_speed = false;
	myI2Cpacket.hs_master_code  = 0x00;	
	myI2Cpacket.data_length = 1;
	
	write_buffer[0] = setting;
	tries = 0; 
	done = 0; 
	while ((done ==0) &&(tries<200))
	{
		if (i2c_master_write_packet_wait(&i2c_master_instance, &myI2Cpacket)==STATUS_OK) 
		{
			done = 1; 
		}
		else
		{
			tries++;
		}
	}
	if (done == 1)
	{
		//-------------------------------
		// Read from slave until success. 
		myI2Cpacket.data_length = count;
		myI2Cpacket.data = buffer;
		tries = 0;
		done = 0;
		while ((done ==0) &&(tries<200))
		{
			if (i2c_master_read_packet_wait(&i2c_master_instance, &myI2Cpacket)==STATUS_OK)
			{
				done = 1;
			}
			else
			{
				tries++;
			}
		}
		if (done == 1)
		{
			status = 1; 
		}
	}
	if (status == 0)
	{
		i2cError = 1; 
		RecoverI2C();
	}
	return status; 	
}; 

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
uint8_t I2CEEPROMBufferWrite(uint8_t *buffer,uint8_t setting, uint8_t count)
{
	uint8_t status,tries,done,*ptr,i;
	
	status = 0;
	ptr = buffer; 
	//---------------------------------------
	// Set-up packet for transmitting
	//---------------------------------------
	myI2Cpacket.address = SLAVE_ADDRESS;
	myI2Cpacket.data = write_buffer;
	myI2Cpacket.ten_bit_address = false;
	myI2Cpacket.high_speed = false;
	myI2Cpacket.hs_master_code  = 0x00;
	myI2Cpacket.data_length = count+1;
	
	write_buffer[0] = setting;
	for (i=0;i<count;i++)
	{
		write_buffer[1+i] = *ptr++;
	}
	tries = 0;
	done = 0;
	while ((done ==0) &&(tries<200))
	{
		if (i2c_master_write_packet_wait(&i2c_master_instance, &myI2Cpacket)==STATUS_OK)
		{
			done = 1;
		}
		else
		{
			tries++;
		}
	}
	if (done == 1)
	{
		status = 1;
	}
	if (status == 0)
	{
		i2cError = 1; 
		RecoverI2C();
	}	
	return status;
};

 

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void configure_i2c_master(void)
{
	struct i2c_master_config config_i2c_master;
	
	//---------------------------------------
	// Initialize config structure and software module.
	//---------------------------------------	
	i2c_master_get_config_defaults(&config_i2c_master);
	
	//--------------------------------------- 
	// Change buffer timeout to something longer. 
	//---------------------------------------
	config_i2c_master.buffer_timeout = 100;  //was 10000
	
	//---------------------------------------
	//  
	//---------------------------------------
	config_i2c_master.pinmux_pad0 = EXT2_I2C_SERCOM_PINMUX_PAD0;
	config_i2c_master.pinmux_pad1 = EXT2_I2C_SERCOM_PINMUX_PAD1;
	//---------------------------------------
	// Initialize and enable device with config
	//---------------------------------------	
	i2c_master_init(&i2c_master_instance, SERCOM4, &config_i2c_master);
	//---------------------------------------
	//
	//---------------------------------------
	i2c_master_enable(&i2c_master_instance);
	 
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void I2Cmain(void)
{
	uint8_t testBuffer[10],n; 
	//---------------------------------------
	// Configure device and enable.
	//---------------------------------------
	configure_i2c_master();
	//--------------------------------------
	// read in the EEPROM header and see if CREED 
	AppStatusUpdate(INTERFACE_EEPROM,STATUS_RW,0);	
	AppStatusUpdate(INTERFACE_EEPROM,STATUS_PARTTALKING,0);
	AppStatusUpdate(INTERFACE_EEPROM,STATUS_GOODHEADER,0);		
	//------------------------------
	// READ in the manufacturer code and device code 
	// and serial number 32 bits. 
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
	if (I2CEEPROMBufferRead(testBuffer,TableHeader1_Setting,10)!= 0)
	{
		AppStatusUpdate(INTERFACE_EEPROM,STATUS_PARTTALKING,1);
		//------------------------
		// check the HEADER
		if ((testBuffer[0] == 'C')&&(testBuffer[1] == 'R')&&(testBuffer[2]=='E'))
		{
			AppStatusUpdate(INTERFACE_EEPROM,STATUS_RW,1);	
			AppStatusUpdate(INTERFACE_EEPROM,STATUS_GOODHEADER,1);	
		}	
		else
		{
			//-----------------------
			// do a simple test. 
			testBuffer[0] = 0x46;
			testBuffer[1] = 0x72; 
			if (I2CEEPROMBufferWrite(testBuffer,0, 2)!= 0) 
			{
				testBuffer[0] = 0;
				testBuffer[1] = 0; 
				if (I2CEEPROMBufferRead(testBuffer,0, 2)!= 0)
				{
					//-----------------------------
					// it worked!!
					AppStatusUpdate(INTERFACE_EEPROM,STATUS_RW,1);					
				}
			}			
		}
	}
	else
	{
		AppStatusUpdate(INTERFACE_EEPROM,STATUS_PARTTALKING,1);
		//-----------------------
		// do a simple test. 
		testBuffer[0] = 0x46;
		testBuffer[1] = 0x72; 
		if (I2CEEPROMBufferWrite(testBuffer,0, 2)!= 0) 
		{
			testBuffer[0] = 0;
			testBuffer[1] = 0; 
			if (I2CEEPROMBufferRead(testBuffer,0, 2)!= 0)
			{
				//-----------------------------
				// it worked!!
				AppStatusUpdate(INTERFACE_EEPROM,STATUS_RW,1);
			}
		}
		else
		{
			//----------real failure	
			AppStatusUpdate(INTERFACE_EEPROM,STATUS_PARTTALKING,0);
		}
	}
}
