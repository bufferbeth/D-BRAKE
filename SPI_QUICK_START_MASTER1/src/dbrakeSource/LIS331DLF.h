/******************************************************************************
* <LIS331DLF.h>
*
* Project:      Alarm-It-All
*
* File Name:	  LIC33DE.h
*
* Description:  This header file contains the definitions for the ST Micro Accelerometer.
*
* Copyright (c) 2010 Randolph Cook.  All rights reserved.  Modified by Jeff Bokor 2011
*
* This work contains valuable, confidential and proprietary information
* of Randolph Cook.  Disclosure, use or reproduction without the prior
* written authorization of Randolph Cook is strictly prohibited.
*
*==============================================================================
* Author:
*    Randolph Cook
*    8800 Bonica Pl
*    Land O Lakes, FL 34637
*    (813) 995-9761
*    randy@authenticationtechnology.com
*
* Licensed to:
*     dbTechnologies, Inc.
*     9921 Kenda Drive
*     Riverview, FL  33569
*     USA
*     (813)376-6895
*     dbtech@tampabay.rr.com
*
*==============================================================================
* Revision History                                                   
*
*   Date     Version   Author   Description
* ---------- --------- ------   ------------------------------------------
* 03/24/2010 01.00.00	 RVC		  Created
* 06/29/2011 01.01.00    JPB              Modified for LIS33DE 
*
******************************************************************************/

#ifndef __LIS331DLF_H__
#define __LIS331DLF_H__


//-------------------------------------------
//	Global Definitions
//-------------------------------------------



//-------------------------------------------
//	External Global Variables
//-------------------------------------------



//-------------------------------------------
//	Global Function Prototypes
//-------------------------------------------



//-------------------------------------------
//	Local Definitions
//-------------------------------------------

//===========================================================================
// LIS33DE Register Map
//===========================================================================


#define MEMS_CTRL_REG1 			0x20		// rw

#define MEMS_DR_400     		((uint8_t)0x80)		// Data Rate = 400 Hz
#define MEMS_POWER_ACTIVE		((uint8_t)0x40)		// Active Mode
#define MEMS_FULL_SCALE 		((uint8_t)0x20)		// Full Scale Sensing
#define MEMS_SELF_TEST_P		((uint8_t)0x10)		// Self Test Plus
#define MEMS_SELF_TEST_M		((uint8_t)0x08)		// Self Test Minus
//
#define	MEMS_Z_ENABLE				((uint8_t)0x04)		// Z-axis enable
#define	MEMS_Y_ENABLE				((uint8_t)0x02)		// Y-axis enable
#define	MEMS_X_ENABLE				((uint8_t)0x01)		// X-axis enable

//----------------------------------------------
#define MEMS_CTRL_REG2		 	0x21		// rw

#define	MEMS_BOOT								((uint8_t)0x80)	// Reboot memory content
#define MEMS_HPFILTER_NORMAL		((uint8_t)0x00)	// High pass filter: normal
#define MEMS_HPFILTER_REFERENCE	((uint8_t)0x20)	// High pass filter: reference
//
#define MEMS_HPFILTER_BYPASS		((uint8_t)0x00)	// High pass filter bypassed
#define MEMS_HPFILTER_ENABLE		((uint8_t)0x10)	// High pass filter enabled
//
#define MEMS_HPFILTER_INT2_ENABLE	((uint8_t)0x08)	// High Pass filter enabled for INT2
#define MEMS_HPFILTER_INT1_ENABLE	((uint8_t)0x04)	// High Pass filter enabled for INT1
//

#define MEMS_HP_CUTOFF_1				((uint8_t)0x00)	// High Pass Cutoff:
//							1  Hz@ODR50,   2 Hz@ODR100, 8Hz@ODR400
//					     approx:    1  ft@50Hz,    2 ft@100Hz,  8ft@400Hz

#define MEMS_HP_CUTOFF_2				((uint8_t)0x01)	// High Pass Cutoff:
//							0.5  Hz@ODR50,   1 Hz@ODR100, 4Hz@ODR400
//					     approx:   .5  ft@50Hz,    1 ft@100Hz,  4ft@400Hz

#define MEMS_HP_CUTOFF_3				((uint8_t)0x02)	// High Pass Cutoff:
//							0.25 Hz@ODR50, 0.5 Hz@ODR100, 2Hz@ODR400
//					     approx:   .25 ft@50Hz,   .5 ft@100Hz,  2ft@400Hz

#define MEMS_HP_CUTOFF_4				((uint8_t)0x03)	// High Pass Cutoff:
//							0.125Hz@ODR50, 0.25Hz@ODR100, 1Hz@ODR400
//					     approx:   .125ft@50Hz,   .25 ft@100Hz, 1ft@400Hz

//----------------------------------------------
#define MEMS_CTRL_REG3	 		0x22		// rw

#define	MEMS_INT_HIGH			((uint8_t)0x00)	// Interrupt active high
#define	MEMS_INT_LOW			((uint8_t)0x80)	// Interrupt active low

#define	MEMS_ICFG_GND	                ((uint8_t)0x00)	// INT1 PAD has GND Source signal
#define	MEMS_ICFG_FF_WU   	        ((uint8_t)0x01)	// INT1 PAD has FF_WU source signal
#define	MEMS_ICFG_DATA_RDY		((uint8_t)0x04)	// INT1 PAD has DATA READY signal

//----------------------------------------------
#define MEMS_STATUS_REG			0x27		// r

#define MEMS_ZYX_OVERRUN	((uint8_t)0x80)	// Data Overrun (any axis)
#define	MEMS_Z_OVERRUN		((uint8_t)0x40) // Z-axis data overrun
#define	MEMS_Y_OVERRUN		((uint8_t)0x20) // Y-axis data overrun
#define	MEMS_X_OVERRUN		((uint8_t)0x10) // X-axis data overrun

#define	MEMS_ZYX_DATA_AVAIL	((uint8_t)0x08) // Data Available (all axis)
#define	MEMS_Z_DATA_AVAIL	((uint8_t)0x04)	// Z-axis Data Available
#define	MEMS_Y_DATA_AVAIL	((uint8_t)0x02)	// Y-axis Data Available
#define	MEMS_X_DATA_AVAIL	((uint8_t)0x01)	// X-axis Data Available

//----------------------------------------------
#define MEMS_OUT_X 			0x29	// r - X-axis data (2's complement)

//----------------------------------------------
#define MEMS_OUT_Y			0x2B	// r - Y-axis data (2's complement)

//----------------------------------------------
#define MEMS_OUT_Z 			0x2D	// r - Z-axis data (2's complement)

//----------------------------------------------
#define MEMS_FF_WU_CFG	 		0x30	// rw - Interrupt 1 Configuration

#define	MEMS_INT_OR		((uint8_t)0x00)	// OR Interrupt events (movement)
#define	MEMS_INT_AND		((uint8_t)0x80)	// AND Interrupt events (position)

#define	MEMS_LIR		((uint8_t)0x40)	// Latch the Interrupt Req

#define	MEMS_ZHIGH_IE		((uint8_t)0x20)	// Interrupt on Z High
#define	MEMS_ZLOW_IE		((uint8_t)0x10)	// Interrupt on Z Low

#define	MEMS_YHIGH_IE		((uint8_t)0x08)	// Interrupt on Y High
#define	MEMS_YLOW_IE		((uint8_t)0x04)	// Interrupt on Y Low

#define	MEMS_XHIGH_IE		((uint8_t)0x02)	// Interrupt on X High
#define	MEMS_XLOW_IE		((uint8_t)0x01)	// Interrupt on X Low

#define	MEMS_ALL_IE		((uint8_t)0x3F)	// Interrupt on ANY event
 


//----------------------------------------------
#define MEMS_FF_WU_SRC			0x31	// r - Interrupt 1 Source
//																// Reading this reg clears register if latched

#define	MEMS_INT_ACTIVE		((uint8_t)0x40)	// Interrupt active (one or more interrupts have been generated)

#define	MEMS_ZHIGH_INT		((uint8_t)0x20) // Z High event has occurred
#define	MEMS_ZLOW_INT		((uint8_t)0x10) // Z Low event has occurred

#define	MEMS_YHIGH_INT		((uint8_t)0x08) // Y High event has occurred
#define	MEMS_YLOW_INT		((uint8_t)0x04) // Y Low event has occurred

#define	MEMS_XHIGH_INT		((uint8_t)0x02) // X High event has occurred
#define	MEMS_XLOW_INT		((uint8_t)0x01) // X Low event has occurred


//----------------------------------------------
#define MEMS_FF_WU_THS			0x32	// rw - Interrupt 1 Threshold

#define MEMS_DCRM               ((uint8_t)0x7F)	// Reset mode. Counter dec
#define	MEMS_THS_MASK		((uint8_t)0x7F)	// Threshold value is 7 bits

//----------------------------------------------
#define MEMS_FF_WU_DURATION		0x33	// rw - Interrupt 1 Duration

#define	MEMS_DURATION_MASK	((uint8_t)0xFF)	// Duration value is 8 bits
#define	MEMS_DURATION_NONE	((uint8_t)0x00)	// Duration value is 8 bits

//			Duration steps and maximum values depend on the ODR selected
//						// ODR=400Hz: 2.5msec steps, 637.5msec max
//						// ODR=100Hz: 10.0msec steps, 2.55sec max
//


#define MEMS_REGISTER_MAX		0x37



#endif //__LIS331DLF_H__



