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
 * \file       main.c
 * \brief      Ping-Pong example application on how to use Semtech's Radio
 *             drivers.
 *
 * \version    2.0
 * \date       Nov 21 2012
 * \author     Miguel Luis
 */
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "platform.h"
#include "sx1272.h"
 

int LoarMain( void );
void OnMaster( void );
void OnSlave( void );

#include "radio.h"

#if 0
#define BUFFER_SIZE                                 9 // Define the payload size here


uint16_t BufferSize = BUFFER_SIZE;			// RF buffer size
uint8_t Buffer[BUFFER_SIZE];					// RF buffer

static uint8_t EnableMaster = true; 				// Master/Slave selection

tRadioDriver *Radio = NULL;

const uint8_t PingMsg[] = "PING";
const uint8_t PongMsg[] = "PONG";



/*
 * Main application entry point.
 */
int LoraMain( void )
{
//	uint8_t done; 
	
//    BoardInit( );
    
    SX1272Init();
    
//    SX1272StartRx( );
 
 /*   
    while( 1 )
    {
//		EnableMaster = false;
        if( EnableMaster == true )
        {
            OnMaster( );

			
			
        }
        else
        {
            OnSlave( );
        }    
    }
 */
 return 1;
}
#endif
 
 

