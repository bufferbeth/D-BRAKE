//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: screenNumbers.c
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor: STM32F103R
// TOOLS: IAR Workbench
// DATE:
// CONTENTS: This file contains
//------------------------------------------------------------------------------
// HISTORY: This file
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#include <asf.h>
 
#include "screenNumbers.h"

const uint8_t MISSING[32] = {0x00,0x00,0x00,0xF8,0x60,0xF8,0x00,0x88,0xF8,0x88,0x00,0xE8,0xA8,0x98,0x00,0xE8,
0xA8,0x98,0x00,0x88,0xF8,0x88,0x00,0x78,0x20,0x38,0x00,0xE8,0xA8,0xF8,0x00,0x00};
const uint8_t READY[32] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0xB0,0xE8,0x00,0xF8,0xA8,0xA8,0x00,0xF8,
0xA0,0xF8,0x00,0xF8,0x88,0x70,0x00,0xE0,0x38,0xE0,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t BREAKAWAY[32] = {0xF8,0xA8,0xF8,0x00,0xF8,0xB0,0xE8,0x00,0xF8,0x20,0xD8,0x00,0x18,0x70,0xC0,0x00,
0xF8,0xA0,0xF8,0x00,0xF8,0x30,0xF8,0x00,0xF8,0xA0,0xF8,0x00,0xE0,0x38,0xE0,0x00};
const uint8_t CLEARSECTION[32] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};


const uint8_t FONTSMALL_HYPEN[6] =   {0x18,0x18,0x18,0x18,0x18,0x00};
const uint8_t FONTSMALL_FULL[6] =   {0x7f,0x7f,0x7f,0x7f,0x7f,0x00};	
const uint8_t FONTSMALL_DOT[6] =   {0x00,0x00,0x18,0x18,0x00,0x00};	
const uint8_t FONTSMALL_BLANK[6] =   {0x00,0x00,0x00,0x00,0x00,0x00};	
const uint8_t FONTSMALL_START[6] =   {0x42,0x24,0x18,0x24,0x42,0x00};	
const uint8_t FONTSMALL_DEGREE[6] =   {0x00,0x00,0x60,0x60,0x00,0x00};		
	
const uint8_t FONTSMALL_ONE[5] =   {0x00,0x00,0x7F,0x00,0x00};
const uint8_t FONTSMALL_TWO[5] =   {0x4F,0x49,0x49,0x49,0x79};
const uint8_t FONTSMALL_THREE[5] = {0x41,0x41,0x49,0x49,0x7F};
const uint8_t FONTSMALL_FOUR[5] =  {0x78,0x08,0x08,0x08,0x7F};	
const uint8_t FONTSMALL_FIVE[5] =  {0x79,0x49,0x49,0x49,0x4F};
const uint8_t FONTSMALL_SIX[5] =   {0x7F,0x49,0x49,0x49,0x4F};	
const uint8_t FONTSMALL_SEVEN[5] = {0x40,0x40,0x40,0x40,0x7F};
const uint8_t FONTSMALL_EIGHT[5] = {0x7F,0x49,0x49,0x49,0x7F};	
const uint8_t FONTSMALL_NINE[5] =  {0x78,0x48,0x48,0x48,0x7F};
const uint8_t FONTSMALL_ZERO[5] =  {0x7F,0x41,0x41,0x41,0x7F};	 
const uint8_t FONTSMALL_A[5] =	   {0x3F,0x48,0x48,0x48,0x3F};
const uint8_t FONTSMALL_B[5] =     {0x7F,0x49,0x49,0x49,0x36};
const uint8_t FONTSMALL_C[5] =     {0x3E,0x41,0x41,0x41,0x22};//
const uint8_t FONTSMALL_D[5] =     {0x3F,0x41,0x41,0x41,0x3E};
const uint8_t FONTSMALL_E[5] =     {0x7F,0x49,0x49,0x49,0x41};//
const uint8_t FONTSMALL_F[5] =     {0x7F,0x48,0x48,0x48,0x40};
const uint8_t FONTSMALL_G[5] =	   {0x3E,0x41,0x49,0x49,0x4E};//
const uint8_t FONTSMALL_H[5] =     {0x7F,0x08,0x08,0x08,0x7f};//
const uint8_t FONTSMALL_I[5] =     {0x41,0x41,0x7F,0x41,0x41};//
const uint8_t FONTSMALL_J[5] =     {0x3F,0x41,0x41,0x41,0x3E};
const uint8_t FONTSMALL_K[5] =     {0x7F,0x08,0x14,0x22,0x41};//
const uint8_t FONTSMALL_L[5] =     {0x7F,0x01,0x01,0x01,0x01};//
const uint8_t FONTSMALL_M[5] =	   {0x7F,0x40,0x30,0x40,0x7F};//
const uint8_t FONTSMALL_N[5] =     {0x7F,0x60,0x1c,0x03,0x7f};//
const uint8_t FONTSMALL_O[5] =     {0x7e,0x41,0x41,0x41,0x7e};//done
const uint8_t FONTSMALL_P[5] =     {0x3F,0x48,0x48,0x48,0x30};//
const uint8_t FONTSMALL_Q[5] =     {0x3E,0x41,0x45,0x42,0x3D};//
const uint8_t FONTSMALL_R[5] =     {0x3F,0x48,0x4C,0x4A,0x31};//
const uint8_t FONTSMALL_S[5] =	   {0x31,0x49,0x49,0x49,0x46};//
const uint8_t FONTSMALL_T[5] =     {0x40,0x40,0x7f,0x40,0x40};//
const uint8_t FONTSMALL_U[5] =     {0x7E,0x01,0x01,0x01,0x7e};//
const uint8_t FONTSMALL_V[5] =     {0x70,0x0e,0x01,0x0e,0x70};
const uint8_t FONTSMALL_W[5] =     {0x7F,0x01,0x0e,0x01,0x7f};
const uint8_t FONTSMALL_X[5] =     {0x41,0x36,0x1c,0x36,0x41};//
const uint8_t FONTSMALL_Y[5] =     {0x60,0x10,0x0F,0x10,0x60};//
const uint8_t FONTSMALL_Z[5] =     {0x7F,0x48,0x48,0x48,0x40};

const uint8_t *FONTSMALL[36]= {FONTSMALL_ZERO,FONTSMALL_ONE,FONTSMALL_TWO,
							   FONTSMALL_THREE,FONTSMALL_FOUR,FONTSMALL_FIVE,
							   FONTSMALL_SIX,FONTSMALL_SEVEN,FONTSMALL_EIGHT,
							   FONTSMALL_NINE,FONTSMALL_A,FONTSMALL_B,
							   FONTSMALL_C,FONTSMALL_D,FONTSMALL_E,FONTSMALL_F,
							   FONTSMALL_G,FONTSMALL_H,FONTSMALL_I,FONTSMALL_J,
							   FONTSMALL_K,FONTSMALL_L,FONTSMALL_M,FONTSMALL_N,
							   FONTSMALL_O,FONTSMALL_P,FONTSMALL_Q,FONTSMALL_R,
							   FONTSMALL_S,FONTSMALL_T,FONTSMALL_U,FONTSMALL_V,
							   FONTSMALL_W,FONTSMALL_X,FONTSMALL_Y,FONTSMALL_Z,							   							   							   							   
							   };
const uint8_t FONTMID_HTOP[9] = {0x00,0x1F,0x1F,0x00,0x00,0x00,0x1F,0x1F,0x00};							   	
const uint8_t FONTMID_HMID[9] = {0x00,0xFE,0xFE,0xC0,0xC0,0xC0,0xFE,0xFE,0x00};
	
const uint8_t FONTMID_ONETOP[9] = {0x00,0x02,0x06,0x0F,0x1F,0x1F,0x00,0x00,0x00};
const uint8_t FONTMID_TWOTOP[9] = {0x00,0x0E,0x1E,0x1C,0x18,0x18,0x1F,0x0F,0x00};
const uint8_t FONTMID_THREETOP[9] = {0x00,0x18,0x18,0x18,0x18,0x1C,0x1F,0x0F,0x00};
const uint8_t FONTMID_FOURTOP[9] = {0x00,0x1F,0x1F,0x00,0x00,0x1F,0x1F,0x00,0x00};	
const uint8_t FONTMID_FIVETOP[9] = {0x00,0x1F,0x1F,0x18,0x18,0x18,0x18,0x18,0x00};
const uint8_t FONTMID_SIXTOP[9] = {0x00,0x07,0x0F,0x1C,0x18,0x18,0x00,0x00,0x00};	
const uint8_t FONTMID_SEVENTOP[9] = {0x00,0x18,0x18,0x18,0x18,0x19,0x1F,0x1F,0x00};
const uint8_t FONTMID_EIGHTTOP[9] = {0x00,0x0F,0x1F,0x18,0x18,0x18,0x1F,0x0F,0x00};	
const uint8_t FONTMID_NINETOP[9] = {0x00,0x0F,0x1F,0x19,0x19,0x19,0x1F,0x0F,0x00};
const uint8_t FONTMID_ZEROTOP[9] = {0x00,0x0F,0x1F,0x1C,0x18,0x1C,0x1F,0x0F,0x00};	

const uint8_t FONTMID_ONEMID[9] = {0x00,0x06,0x06,0xFE,0xFE,0xFE,0x06,0x06,0x00};
const uint8_t FONTMID_TWOMID[9] = {0x00,0x0E,0x1E,0x3E,0x76,0xE6,0xC6,0x86,0x00};
const uint8_t FONTMID_THREEMID[9] = {0x00,0x06,0x06,0xC6,0xC6,0xCE,0xFE,0x3C,0x00};
const uint8_t FONTMID_FOURMID[9] = {0x00,0xC0,0xC0,0xC0,0xC0,0xFE,0xFE,0xC0,0x00};	
const uint8_t FONTMID_FIVEMID[9] = {0x00,0x86,0xC6,0xC6,0xC6,0xC6,0xFE,0x7C,0x00};
const uint8_t FONTMID_SIXMID[9] = {0x00,0xFC,0xFE,0xC6,0xC6,0xC6,0xFE,0x7C,0x00};	
const uint8_t FONTMID_SEVENMID[9] = {0x00,0x00,0x00,0x0E,0x3E,0xF8,0xE0,0x00,0x00};
const uint8_t FONTMID_EIGHTMID[9] = {0x00,0xFC,0xFE,0xC6,0xC6,0xC6,0xFE,0xFC,0x00};	
const uint8_t FONTMID_NINEMID[9] = {0x00,0x00,0x80,0x80,0x86,0x9E,0xFC,0xF0,0x00};
const uint8_t FONTMID_ZEROMID[9] = {0x00,0xFC,0xFE,0x0E,0x06,0x0E,0xFE,0xFC,0x00};	

const uint8_t FONTMID_ONEBOT[9] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t FONTMID_TWOBOT[9] = {0x00,0xE0,0xE0,0xE0,0x60,0x60,0x60,0x60,0x00};
const uint8_t FONTMID_THREEBOT[9] = {0x00,0x60,0x60,0x60,0x60,0xE0,0xE0,0xC0,0x00};
const uint8_t FONTMID_FOURBOT[9] = {0x00,0x00,0x00,0x00,0x00,0xE0,0xE0,0x00,0x00};	
const uint8_t FONTMID_FIVEBOT[9] = {0x00,0x60,0x60,0x60,0x60,0x60,0xE0,0xC0,0x00};	
const uint8_t FONTMID_SIXBOT[9] = {0x00,0xC0,0xE0,0x60,0x60,0x60,0xE0,0xC0,0x00};	
const uint8_t FONTMID_SEVENBOT[9] = {0x00,0x00,0x00,0xE0,0xE0,0x80,0x00,0x00,0x00};	
const uint8_t FONTMID_EIGHTBOT[9] = {0x00,0xC0,0xE0,0x60,0x60,0x60,0xE0,0xC0,0x00};	
const uint8_t FONTMID_NINEBOT[9] = {0x00,0x00,0x00,0x00,0x60,0xE0,0xC0,0x00,0x00};
const uint8_t FONTMID_ZEROBOT[9] = {0x00,0xC0,0xE0,0xE0,0x60,0xE0,0xE0,0xC0,0x00};	
	
const uint8_t *FONTMID[10][3]= {	
									{FONTMID_ZEROMID,FONTMID_ZEROBOT,FONTMID_ZEROTOP},
									{FONTMID_ONEMID,FONTMID_ONEBOT,FONTMID_ONETOP},	
									{FONTMID_TWOMID,FONTMID_TWOBOT,FONTMID_TWOTOP},
									{FONTMID_THREEMID,FONTMID_THREEBOT,FONTMID_THREETOP},
									{FONTMID_FOURMID,FONTMID_FOURBOT,FONTMID_FOURTOP},
									{FONTMID_FIVEMID,FONTMID_FIVEBOT,FONTMID_FIVETOP},
									{FONTMID_SIXMID,FONTMID_SIXBOT,FONTMID_SIXTOP},
									{FONTMID_SEVENMID,FONTMID_SEVENBOT,FONTMID_SEVENTOP},
									{FONTMID_EIGHTMID,FONTMID_EIGHTBOT,FONTMID_EIGHTTOP},
									{FONTMID_NINEMID,FONTMID_NINEBOT,FONTMID_NINETOP}																																								
								};	
	
const uint8_t FONTLARGE_HMID[11] = {0x00,0x00,0xFF,0xFF,0xC0,0xC0,0xC0,0xFF,0xFF,0x00,0x00};
const uint8_t FONTLARGE_HBOT[11] = {0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x80,0x80,0x00,0x00};
const uint8_t FONTLARGE_HTOP[11] = {0x00,0x00,0x3F,0x3F,0x03,0x03,0x03,0x3F,0x3F,0x00,0x00};	

const uint8_t FONTLARGE_ONEMID[11] = {0x00,0x01,0x01,0x01,0x01,0xFF,0xFF,0x01,0x01,0x01,0x00};
const uint8_t FONTLARGE_ONEBOT[11] = {0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00};
const uint8_t FONTLARGE_ONETOP[11] = {0x00,0x00,0x04,0x0C,0x1C,0x3F,0x3F,0x00,0x00,0x00,0x00};

const uint8_t FONTLARGE_TWOMID[11] = {0x00,0x03,0x07,0x0F,0x1D,0x39,0x71,0xE1,0xC1,0x81,0x00};
const uint8_t FONTLARGE_TWOBOT[11] = {0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00};
const uint8_t FONTLARGE_TWOTOP[11] = {0x00,0x0C,0x1C,0x38,0x30,0x30,0x30,0x38,0x1F,0x0F,0x00};

const uint8_t FONTLARGE_THREEMID[11] = {0x00,0x01,0x01,0x01,0xC1,0xC1,0xC1,0xC3,0xFF,0xFF,0x00};
const uint8_t FONTLARGE_THREEBOT[11] = {0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00};
const uint8_t FONTLARGE_THREETOP[11] = {0x00,0x30,0x30,0x30,0x30,0x30,0x30,0x38,0x3F,0x1F,0x00};

const uint8_t FONTLARGE_FOURMID[11] = {0x00,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xFF,0xFF,0xC0,0x00};
const uint8_t FONTLARGE_FOURBOT[11] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00};
const uint8_t FONTLARGE_FOURTOP[11] = {0x00,0x3F,0x3F,0x00,0x00,0x00,0x00,0x3F,0x3F,0x00,0x00};

const uint8_t FONTLARGE_FIVEMID[11] = {0x00,0xC1,0xC1,0xC1,0xC1,0xC1,0xC1,0xC1,0xFF,0x7F,0x00};
const uint8_t FONTLARGE_FIVEBOT[11] = {0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00};
const uint8_t FONTLARGE_FIVETOP[11] = {0x00,0x3F,0x3F,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x00};

const uint8_t FONTLARGE_SIXMID[11] = {0x00,0xFF,0xFF,0xC1,0xC1,0xC1,0xC1,0xC1,0xFF,0x7F,0x00};
const uint8_t FONTLARGE_SIXBOT[11] = {0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00};
const uint8_t FONTLARGE_SIXTOP[11] = {0x00,0x07,0x0F,0x1C,0x38,0x38,0x30,0x30,0x30,0x00,0x00};

const uint8_t FONTLARGE_SEVENMID[11] = {0x00,0x00,0x00,0x00,0x00,0x0F,0x3F,0xF8,0xE0,0x00,0x00};
const uint8_t FONTLARGE_SEVENBOT[11] = {0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00,0x00};
const uint8_t FONTLARGE_SEVENTOP[11] = {0x00,0x30,0x30,0x30,0x30,0x30,0x30,0x31,0x3F,0x3F,0x00};

const uint8_t FONTLARGE_EIGHTMID[11] = {0x00,0xFF,0xFF,0xC1,0xC1,0xC1,0xC1,0xC1,0xFF,0xFF,0x00};
const uint8_t FONTLARGE_EIGHTBOT[11] = {0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00};
const uint8_t FONTLARGE_EIGHTTOP[11] = {0x00,0x1F,0x3F,0x30,0x30,0x30,0x30,0x30,0x3F,0x1F,0x00};

const uint8_t FONTLARGE_NINEMID[11] = {0x00,0x80,0xC0,0xC0,0xC0,0xC0,0xCF,0xFF,0xF8,0xE0,0x00};
const uint8_t FONTLARGE_NINEBOT[11] = {0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x00,0x00,0x00};
const uint8_t FONTLARGE_NINETOP[11] = {0x00,0x1F,0x3F,0x30,0x30,0x30,0x30,0x30,0x3F,0x1F,0x00};

const uint8_t FONTLARGE_ZEROMID[11] = {0x00,0xFF,0xFF,0x01,0x01,0x01,0x01,0x01,0xFF,0xFF,0x00};
const uint8_t FONTLARGE_ZEROBOT[11] = {0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x00};
const uint8_t FONTLARGE_ZEROTOP[11] = {0x00,0x1F,0x3F,0x30,0x30,0x30,0x30,0x30,0x3F,0x1F,0x00};



const uint8_t *FONTLARGE[10][3]= {	
									{FONTLARGE_ZEROMID,FONTLARGE_ZEROBOT,FONTLARGE_ZEROTOP},
									{FONTLARGE_ONEMID,FONTLARGE_ONEBOT,FONTLARGE_ONETOP},	
									{FONTLARGE_TWOMID,FONTLARGE_TWOBOT,FONTLARGE_TWOTOP},
									{FONTLARGE_THREEMID,FONTLARGE_THREEBOT,FONTLARGE_THREETOP},
									{FONTLARGE_FOURMID,FONTLARGE_FOURBOT,FONTLARGE_FOURTOP},
									{FONTLARGE_FIVEMID,FONTLARGE_FIVEBOT,FONTLARGE_FIVETOP},
									{FONTLARGE_SIXMID,FONTLARGE_SIXBOT,FONTLARGE_SIXTOP},
									{FONTLARGE_SEVENMID,FONTLARGE_SEVENBOT,FONTLARGE_SEVENTOP},
									{FONTLARGE_EIGHTMID,FONTLARGE_EIGHTBOT,FONTLARGE_EIGHTTOP},
									{FONTLARGE_NINEMID,FONTLARGE_NINEBOT,FONTLARGE_NINETOP}																																								
								};

 

 