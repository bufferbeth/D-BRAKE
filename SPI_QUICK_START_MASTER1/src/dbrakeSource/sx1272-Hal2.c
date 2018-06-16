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

#include "dbrakeDefs.h"
#include "ioe.h"
#include "spi.h"
 
#include "driverXSPI.h"
#include "sx1272-fsk.h"
  
 

 

 

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

void SX1272WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
	SPIXInOut(addr|0x80,buffer,size);
}

void SX1272ReadBuffer2( uint8_t addr, uint8_t *buffer, uint8_t size )
{   
	SPIXInOut2(addr & 0x7f,buffer,size);
}

void SX1272ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
     
	SPIXInOut(addr & 0x7f,buffer,size);
}

void SX1272WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1272WriteBuffer( 0, buffer, size );
}

void SX1272ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1272ReadBuffer2( 0, buffer, size );
}

 

 
 
 
