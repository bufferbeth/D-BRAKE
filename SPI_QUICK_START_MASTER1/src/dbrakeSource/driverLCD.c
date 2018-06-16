//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: driverLCD.c
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor: STM32F103R
// TOOLS: IAR Workbench
// DATE:
// CONTENTS: This file contains
//------------------------------------------------------------------------------
// HISTORY: This file
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#include <asf.h>
#include "dbrakeDefs.h"
#if REMOTEBOARD
#include "driverLCD.h"

//---------------------GLOBAL VARIABLES-----------------------------------

//---------------------LOCAL VARIABLES------------------------------------
//--------------------------

//---------------------LOCAL FUNCTION PROTOTYPES--------------------------
void writeDataBus(uint8_t what, uint8_t inverse);

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:  delayUs
//------------------------------------------------------------------------------
// This function 
// input: 0~65535
//==============================================================================
void delayUs(uint16_t us) 
{
//	uint16_t i=0;
//	uint16_t j=0;
//	for(i=0; i<=us; i++) {
//DDDD		for(j=0; j<=16; j++);
//		for(j=0; j<=2; j++);		
//	}
}

// Delay us
// input: 0~65535
void delayMs(uint16_t ms) {
	uint16_t i=0;
	for(i=0; i<=ms; i++) {
		delayUs(5);     //0);
	}
}

 

// Set DB7~DB0 PIN as input or ourput
// direction=1 -> input£»   direction=0(or any other value non-1) -> output;
void lcdDdataPinsDirectionSet(uint8_t direction) {
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);
	
	if(direction==1) 
	{
//		FIO0DIR2 = 0x00;      // Set P0.23~P0.16 as input
		pin_conf.direction  = PORT_PIN_DIR_INPUT;
	}
	else 
	{
//		FIO0DIR2 = 0xFF;      // Set P0.23~P0.16 as output
		pin_conf.direction  = PORT_PIN_DIR_OUTPUT;	
	}
		port_pin_set_config(DB0, &pin_conf);
		port_pin_set_config(DB1, &pin_conf);
		port_pin_set_config(DB2, &pin_conf);
		port_pin_set_config(DB3, &pin_conf);
		port_pin_set_config(DB4, &pin_conf);
		port_pin_set_config(DB5, &pin_conf);
		port_pin_set_config(DB6, &pin_conf);
		port_pin_set_config(DB7, &pin_conf);	
}

// Initialize LCD control PINs as output; RST, A0, RW, E;
void lcdPinInit(void) 
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);
	
 	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(RST, &pin_conf);	
	port_pin_set_config(A0, &pin_conf);		
	port_pin_set_config(RW, &pin_conf);	
	port_pin_set_config(E, &pin_conf);		
	port_pin_set_config(CSB, &pin_conf);		
		port_pin_set_output_level(CSB,false);	
					
	lcdDdataPinsDirectionSet(0);    // set data pins as output
}

// Set RST level
// Input:   0->Low;  1->High
void setRST(uint8_t RST_value) 
{
	if(RST_value == 0) 
	{
//		FIO0CLR = RST;	     // RST = 0
		port_pin_set_output_level(RST,false);		
	}
	else 
	{
//		FIO0SET = RST;		 // RST = 1
		port_pin_set_output_level(RST,true);			
	}
}

// Set A0 level
// Input:   0->Low;  1->High
void setA0(uint8_t A0_value) 
{
	if(A0_value == 0) 
	{
		port_pin_set_output_level(A0,false);
	}
	else
	{
		//		FIO0SET = RST;		 // RST = 1
		port_pin_set_output_level(A0,true);
	}
}

// Set RW level
// Input:   0->Low;  1->High
void setRW(uint8_t RW_value) 
{
	if(RW_value == 0) 
	{
		port_pin_set_output_level(RW,false);
	}
	else
	{
		//		FIO0SET = RST;		 // RST = 1
		port_pin_set_output_level(RW,true);
	}
}

// Set E level
// Input:   0->Low;  1->High
void setE(uint8_t E_value) {
	if(E_value == 0) {
		port_pin_set_output_level(E,false);
	}
	else
	{
		//		FIO0SET = RST;		 // RST = 1
		port_pin_set_output_level(E,true);
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void writeDataBus(uint8_t what, uint8_t inverse)
{
	uint32_t itemp;
	itemp = what; 
	itemp = itemp<<16; 
	
	if (inverse == 0)
	{
		port_group_set_output_level(&PORTA,0x00FF0000,itemp);
 
	}
	else
	{
			
			itemp = 0; 
			if ((what & 0x80)!=0)
			{
				itemp |= 0x00010000;
			}
			if ((what & 0x40)!=0)
			{
				itemp |= 0x00020000;
			}
			if ((what & 0x20)!=0)
			{
				itemp |= 0x00040000;
			}
			if ((what & 0x10)!=0)
			{
				itemp |= 0x00080000;		 
			}
			if ((what & 0x08)!=0)
			{
				itemp |= 0x00100000;			 
			}
			if ((what & 0x04)!=0)
			{
				itemp |= 0x00200000;			 
			}
			if ((what & 0x02)!=0)
			{
				itemp |= 0x00400000;			 
			}
			if ((what & 0x01)!=0)
			{
				itemp |= 0x00800000;			 
			}
				
			port_group_set_output_level(&PORTA,0x00FF0000,itemp);					
/*			
			if ((what & 0x80)==0)
			{
				port_pin_set_output_level(DB0,false);				
			}
			else
			{
				port_pin_set_output_level(DB0,true);				
			}
			if ((what & 0x40)==0)
			{
				port_pin_set_output_level(DB1,false);
			}
			else
			{
				port_pin_set_output_level(DB1,true);
			}
			if ((what & 0x20)==0)
			{
				port_pin_set_output_level(DB2,false);
			}
			else
			{
				port_pin_set_output_level(DB2,true);
			}
			if ((what & 0x10)==0)
			{
				port_pin_set_output_level(DB3,false);
			}
			else
			{
				port_pin_set_output_level(DB3,true);
			}		
			if ((what & 0x08)==0)
			{
				port_pin_set_output_level(DB4,false);
			}
			else
			{
				port_pin_set_output_level(DB4,true);
			}
			if ((what & 0x04)==0)
			{
				port_pin_set_output_level(DB5,false);
			}
			else
			{
				port_pin_set_output_level(DB5,true);
			}
			if ((what & 0x02)==0)
			{
				port_pin_set_output_level(DB6,false);
			}
			else
			{
				port_pin_set_output_level(DB6,true);
			}
			if ((what & 0x01)==0)
			{
				port_pin_set_output_level(DB7,false);
			}
			else
			{
				port_pin_set_output_level(DB7,true);
			}				
*/			
		}	
		
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
// Write command
// Input:  Refer to Command Set	of Data Sheet
void writeComd(uint8_t comd) {

	lcdDdataPinsDirectionSet(0);	      // DB7~DB0 as output
	
	setRW(0);  setA0(0);				  // Write instruction register

//	FIO0PIN2 = comd;  // output command on P0.23~P0.16, as a 8bits bus
	writeDataBus(comd,0);
	
//	delayUs(10);

	setE(1);   
	//delayUs(5);
	setE(0);   
	//delayUs(10);
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
// Wirte data,
// Input:  Refer to Data Sheet
void writeData(uint8_t data,uint8_t inverse) 
{
	
	lcdDdataPinsDirectionSet(0);	      // DB7~DB0 as output
	setRW(0);  
	setA0(1);				  // Write data register

//	FIO0PIN2 = data;  // output data on P0.23~P0.16, as a 8bits bus
	writeDataBus(data,inverse);	
//	delayUs(10);
	setE(1);   
//	delayUs(5);
	setE(0);   
//	delayUs(10);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
// Initialize LCM controller IC, ST7567
void lcdInit(void) {

	setRST(0);
	delayMs(10);
	setRST(1);
	delayMs(10);		 // Reset ST7567

	writeComd(0xa2);    delayUs(100);   // bias=1/9, duty=1/65
	writeComd(0xa0);    delayUs(100);   // normal SEG direction
	writeComd(0xc0);    delayUs(100);	// normal COM direction
	writeComd(0xf8);    delayUs(100);	// set booster_(1)
	writeComd(0x01);    delayUs(100);	// set booster_(2)	*5

	writeComd(0x27);    delayMs(100);	// Regulation Ratio
	// V0=9.0  RR=6.5 [RR2,RR1,RR0]=111 Ratio=0x27 EV=0x08
	// V0=8.7  RR=6   RR2RR1RR0=110 Ratio=0x26 EV=0x0c
	// V0=9.3  RR=6.5 RR2RR1RR0=111 Ratio=0x27 EV=0x0b

	writeComd(0x81);	delayUs(100);	// set EV_(1)
	writeComd(0x08);	delayUs(100);	// set EV_(2) : EV5~EV0=001000 (0x08)

	writeComd(0x2f);	delayUs(100);	// power control
	writeComd(0xaf);	delayUs(100);	// display on
	
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
// **** fillScreen ****
// left -> right, up -> down, to fill screen
// this function just fill screen with simple data
void fillScreen(uint8_t data) {
	uint16_t i,j;
	for(j=0;j<8;j++) {
		writeComd(0xb0+j);   //set page address D0~D7
		writeComd(0x40);     //set start line
		writeComd(0x10);     //set column Address
		writeComd(0x00);
		
		for(i=0;i<132;i++){
			writeData(data,0);
		}
	}
}

// **** cleanScreen ****
void FillScreen3(void)	{
	fillScreen(0x33);
}

// **** cleanScreen ****
void cleanScreen(void)	{
	fillScreen(0x00);
}

// **** placeDot ****
// Place a dot at (x,y) on screen. (0,0) at the up left corner
// input:  x position (0~127)
//         y position (0~63)
// output: void
void placeDot(uint16_t x, uint16_t y) {
	if ( (x<=127) && (y<=63) ) {	    // protect overflow
		writeComd(0x10|(x>>4));	        // set x  (1)
		writeComd(0x00|(x&0x0f));		// set x  (2)
		writeComd(0xb0|(y/8));		    // set y; actually set current page
		writeData(1<<(y%8),0);            // write converted data
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void LCDPlacePage(uint8_t page,const uint8_t *gData,uint8_t inverse) 
{
	uint16_t i,temp;
	if (page<8)
	{
		writeComd(0xb0+page);   //set page address D0~D7

		writeComd(0x10);     //set column Address
		writeComd(0x00);
		
		for(i=0;i<128;i++)
		{
			temp = gData[i];
			writeData(temp,inverse);
		}

	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void LCDPlaceData(uint8_t x,uint8_t y, const uint8_t *data,uint8_t size)
{
	uint8_t *ptr,i; 
	ptr = (uint8_t *)data; 
	//-------------------------------
	// PAGE is y/8   	
	if ( (x<=127) && (y<=63) ) 
	{	    // protect overflow
		writeComd(0x10|(x>>4));	        // set x  (1)
		writeComd(0x00|(x&0x0f));		// set x  (2)
		writeComd(0xb0|(y/8));		    // set y; actually set current page
		for (i=0;i<size;i++)
		{
			writeData(*ptr++,1);            // write converted data	
		}
 			
	}	
	
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void LCDClearArea(uint8_t x,uint8_t y, uint8_t horizontalLength, uint8_t verticalLength)
{
	 
	//-------------------------------
	// PAGE is y/8   	
	if ( (x<=127) && (y<=63) ) 
	{	    // protect overflow
		writeComd(0x10|(x>>4));	        // set x  (1)
		writeComd(0x00|(x&0x0f));		// set x  (2)
		writeComd(0xb0|(y/8));		    // set y; actually set current page
		writeData(0x00,1);            // write converted data
		writeData(0x01,1);            // write converted data
		writeData(0x01,1);            // write converted data				
		writeData(0x01,1);            // write converted data
		writeData(0x01,1);            // write converted data
		writeData(0xff,1);            // write converted data					
		writeData(0xff,1);            // write converted data
		writeData(0x01,1);            // write converted data
		writeData(0x01,1);            // write converted data					
		writeData(0x01,1);            // write converted data
		writeData(0x00,1);            // write converted data
 			
	}	
	
}


 
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:   
//------------------------------------------------------------------------------
// This function
//==============================================================================
void refreshScreen(const uint8_t *gData,uint8_t inverse) {
	uint16_t i,j,temp;
	for(j=0;j<8;j++) {
		writeComd(0xb0+j);   //set page address D0~D7

		writeComd(0x10);     //set column Address
		writeComd(0x00);
		
		for(i=0;i<128;i++)
		{
			temp = gData[i+(j*128)];
			writeData(temp,inverse); 
		}

	}
}

#endif 


