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
 * \file       sx1272-Hal.c
 * \brief      SX1272 Hardware Abstraction Layer
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include <stdint.h>
#include <stdbool.h> 

#include "DbrakeControlBoard.h"
#include "ioe.h"
#include "spi.h"
#include "sx1272-Hal.h"
#include "driverXSPI.h"
  
 

void SX1272InitIo( void )
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);


	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	 
	port_pin_set_output_level(LORA_XNSSS, 1);
	
	port_pin_set_config(LORA_XNSSS, &pin_conf);
	port_pin_set_config(LORA_XMOSI, &pin_conf);
	port_pin_set_config(LORA_XSCK, &pin_conf);

	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	port_pin_set_config(LORA_XMISO, &pin_conf);
	port_pin_set_config(LORA_XDIO0, &pin_conf);
    
}

void SX1272SetReset( uint8_t state )
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);
	
    if( state == RADIO_RESET_ON )
    {
        // Set RESET pin to 1
		port_pin_set_output_level(LORA_XRESET, 1);
		pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
		port_pin_set_config(LORA_XRESET, &pin_conf);
	 
    }
    else
    {
        // Set RESET pin to 0
        pin_conf.direction  = PORT_PIN_DIR_INPUT;
        port_pin_set_config(LORA_XRESET, &pin_conf);
    }
}

void SX1272Write( uint8_t addr, uint8_t data )
{
    SX1272WriteBuffer( addr, &data, 1 );
}

void SX1272Read( uint8_t addr, uint8_t *data )
{
    SX1272ReadBuffer( addr, data, 1 );
}

void SX1272WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
//    uint8_t i;

    //NSS = 0;
//    port_pin_set_output_level(LORA_XNSSS, 0);

	SPIXInOut(addr|0x80,buffer,size);
//    SpiInOut( addr | 0x80 );
//    for( i = 0; i < size; i++ )
//    {
//        SpiInOut( buffer[i] );
//    }

    //NSS = 1;
//    port_pin_set_output_level(LORA_XNSSS, 1);
}

void SX1272ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;
	SPIXInOut(addr|0x80,buffer,size);
/*

    //NSS = 0;
    port_pin_set_output_level(LORA_XNSSS, 0);

    SpiInOut( addr & 0x7F );

    for( i = 0; i < size; i++ )
    {
        buffer[i] = SpiInOut( 0 );
    }

    //NSS = 1;
    port_pin_set_output_level(LORA_XNSSS, 1);
*/	
}

void SX1272WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1272WriteBuffer( 0, buffer, size );
}

void SX1272ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1272ReadBuffer( 0, buffer, size );
}

uint8_t SX1272ReadDio0( void )
{
    return port_pin_get_input_level(LORA_XDIO0) ;
}

 
inline void SX1272WriteRxTx( uint8_t txEnable )
{
    if( txEnable != 0 )
    {
//AA        IoePinOn( FEM_CTX_PIN );
    }
    else
    {
//AA        IoePinOff( FEM_CTX_PIN );
    }
}

 
