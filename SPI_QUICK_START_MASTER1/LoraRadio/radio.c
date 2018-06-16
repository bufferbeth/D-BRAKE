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
 * \file       radio.c
 * \brief      Generic radio driver ( radio abstraction )
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Gregory Cristian on Apr 25 2013
 */

#include "platform.h"
#include "DbrakeControlBoard.h"
#include "radio.h"
#include "sx1272.h"
 

tRadioDriver RadioDriver;

tRadioDriver* RadioDriverInit( void )
{
 
    RadioDriver.Init = SX1272Init;
    RadioDriver.Reset = SX1272Reset;
    RadioDriver.StartRx = SX1272StartRx;
    RadioDriver.GetRxPacket = SX1272GetRxPacket;
    RadioDriver.SetTxPacket = SX1272SetTxPacket;
    RadioDriver.Process = SX1272Process;
 

    return &RadioDriver;
}
