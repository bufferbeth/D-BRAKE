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
 * \file       radio.h
 * \brief      Generic radio driver ( radio abstraction )
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Gregory Cristian on Apr 25 2013
 */
#ifndef __RADIO_H__
#define __RADIO_H__
#include <asf.h>
#include "dBrakeDefs.h"

typedef struct
{
	uint8_t Length;
	uint8_t Command[2];
}testRFDATA;

 
#if REMOTEBOARD
#define LORA                                  0  //1           // [0: OFF, 1: ON]
#define RADIO_TRANSMIT						  1 //		1  
#endif
#if BRAKEBOARD
#define LORA                                  0  //1             // [0: OFF, 1: ON]
#define RADIO_TRANSMIT						  0 //		1
#endif

 

#endif // __RADIO_H__
