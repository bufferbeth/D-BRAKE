/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND 
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, SEMTECH SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) SEMTECH S.A.
 */
/*! 
 * \file       sx1272.c
 * \brief      SX1272 RF chip driver
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include <asf.h>
#include "dbrakeDefs.h"
#include "radio.h"
#include "driverXSPI.h" 
#include "sx1272.h"
#include "sx1272-Fsk.h"
#include "sx1272-LoRa.h"

 
uint8_t SX1272Regs[0x75];


 

testRFDATA testRFBuffer[MAXPACKETS2];
uint8_t testRFBufferOffset = 0;

bool LoRaOn = false;
bool LoRaOnState = false;


void SX1272Write( uint8_t addr, uint8_t data )
{
	SPIXInOut(addr|0x80,&data,1);
}

void SX1272Read( uint8_t addr, uint8_t *data )
{
	uint8_t tempBuffer[2];
	SPIXInOut2(addr & 0x7f,tempBuffer,1);
	*data = tempBuffer[0];
}

void SX1272WriteBuffer( uint8_t addr, uint8_t *buffer, uint16_t size )
{
	SPIXInOut(addr|0x80,buffer,size);
}

void SX1272ReadBuffer2( uint8_t addr, uint8_t *buffer, uint16_t size )
{
	SPIXInOut2(addr & 0x7f,buffer,size);
}

 
void SX1272WriteFifo( uint8_t *buffer, uint16_t size )
{
//	SX1272WriteBuffer( 0, buffer, size );
	SPIXInOut(0x80,buffer,size);
}

void SX1272ReadFifo( uint8_t *buffer, uint16_t size )
{
//	SX1272ReadBuffer2( 0, buffer, size );
	SPIXInOut2(0,buffer,size);	
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void SX1272Init(uint8_t radiowhich )
{
    // Initialize FSK and LoRa registers structure
	if (radiowhich == WHICHRADIO_LORA)
	{
		SX1272LR = ( tSX1272LR* )SX1272Regs;
		SX1272Reset( );
		LoRaOn = true;
		SX1272SetLoRaOn( LoRaOn );
		// Initialize LoRa modem
		SX1272LoRaInit( );		
	}
	else
	{
		SX1272 = ( tSX1272* )SX1272Regs;
		SX1272Reset( );
		LoRaOn = false;
		SX1272SetLoRaOn( LoRaOn );
		// Initialize FSK modem
		SX1272FskInit( );		
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void SX1272Reset( void )
{
	uint32_t i; 
  	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);
	//--------------------------
	// Set RESET pin to 1
	port_pin_set_output_level(LORA_XRESET, 1);
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LORA_XRESET, &pin_conf);
    // Wait 1 msec
	for (i=0;i<0x00007000;i++)
	{
		
	}
  	port_pin_set_output_level(LORA_XRESET, 0);
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;		
    port_pin_set_config(LORA_XRESET, &pin_conf);   
    // Wait 1 msec
	for (i=0;i<0x00007000;i++)
	{
		
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void SX1272SetLoRaOn( bool enable )
{
 
    LoRaOnState = enable;

    if( LoRaOn == true )
    {
		//---------------------------------------
		// set up the LORA RADIO
		//---------------------------------------
	    SX1272ReadBuffer2( REG_LR_OPMODE, SX1272Regs, 0x70 - 1 );
	    SX1272LoRaSetOpMode( RFLR_OPMODE_SLEEP );
	    
	    SX1272LR->RegOpMode = ( SX1272LR->RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_ON;
	    SX1272Write( REG_LR_OPMODE, SX1272LR->RegOpMode );
	    
	    SX1272LoRaSetOpMode( RFLR_OPMODE_STANDBY );
	    // RxDone               RxTimeout                   FhssChangeChannel           CadDone
	    //        SX1272LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
	    // CadDetected          ModeReady
	    //        SX1272LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
	    //        SX1272WriteBuffer( REG_LR_DIOMAPPING1, &SX1272LR->RegDioMapping1, 2 );
	    
	    SX1272ReadBuffer2( REG_LR_OPMODE, SX1272Regs, 0x70 - 1 );
    }
    else
    {
		//-----------------------------
		// set up FSK RADIO
		//-----------------------------
	    SX1272ReadBuffer2( REG_OPMODE, SX1272Regs+1, 0x70 - 1 );
	    SX1272FskSetOpMode( RF_OPMODE_SLEEP );
	    
	    SX1272->RegOpMode = ( SX1272->RegOpMode & RF_OPMODE_LONGRANGEMODE_MASK ) | RF_OPMODE_LONGRANGEMODE_OFF;
	    SX1272Write( REG_OPMODE, SX1272->RegOpMode );
	    
	    SX1272FskSetOpMode( RFLR_OPMODE_STANDBY );
	    SX1272ReadBuffer2( REG_OPMODE, SX1272Regs+1, 0x70 - 1 );
    }
}


 
