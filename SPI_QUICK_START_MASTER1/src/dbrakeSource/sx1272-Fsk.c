//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE:  
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor:  
// TOOLS: IAR Workbench
// DATE:
// CONTENTS: This file contains
//------------------------------------------------------------------------------
// HISTORY: This file
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#include <asf.h>
#include "dbrakeDefs.h"

#include <string.h>
#include <math.h>
#include "platform.h"
#include "applcd.h"

#include "radio.h"
 
 
 
#include "sx1272.h"
#include "sx1272-FskMisc.h"
#include "sx1272-Fsk.h"
#include "config.h"
#include "appProtocol.h"

const uint8_t FSK_DEFAULT[112] = {0x00,0x01,0x1a,0x0b,0x00,0x52,0xe4,0xc0,0x00,0x0f,
								   0x19,0x2b,0x20,0x08,0x02,0x0a,0xff,0x00,0x15,0x0b,
								   0x28,0x0c,0x12,0x47,0x32,0x3e,0x00,0x00,0x00,0x00,
								   0x00,0x40,0x00,0x00,0x00,0x00,0x05,0x00,0x03,0x93,
								   0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x90,0x40,
								   0x40,0x00,0x00,0x0f,0x00,0x00,0x00,0xf5,0x20,0x82,
								   0xf2,0x02,0x80,0x40,0x00,0x00,0x22,0x13,0x0e,0x5b,
								   0xdb,0x24,0x0D,0xFD,0x3A,0x2e,0x00,0x03,0x00,0x00,
								   0x00,0x00,0x04,0x23,0x3F,0xFE,0x3F,0xD4,0x09,0x05,
								   0x84,0x0b,0xd0,0x0b,0xd0,0x32,0x2b,0x14,0x00,0x00,
								   0x12,0x00,0x00,0x00,0x0f,0xe0,0x00,0x0c,0xf2,0x14,
								   0x25,0x06
								   };
//---------------------GLOBAL VARIABLES-----------------------------------
 

//---------------------LOCAL VARIABLES------------------------------------

	// Default settings
tFskSettings FskSettings = 
{
    922330000,   //870000000,      // RFFrequency
    9600,           // Bitrate
#if RADIO_CONTINUOUS	
    0,  //BETH test in continuous
#else
	50000,          // Fdev
#endif	
    10,  //20,             // Power
    100000,         // RxBw
    150000,         // RxBwAfc
    true,           // CrcOn
    true,           // AfcOn    
    255             // PayloadLength (set payload size to the maximum for variable mode, else set the exact payload length)
};

	//------------------------------
	// SX1272 FSK registers variable
tSX1272* SX1272;
	//---------------------------
	// Local RF buffer for communication support
uint8_t RFBuffer[RF_BUFFER_SIZE];

//testRFDATA testRFBuffer[MAXPACKETS2];
//uint8_t testRFBufferOffset = 0;
  
uint8_t RFState = RF_STATE_IDLE;
 
uint16_t RxPacketSize = 0;
uint16_t TxPacketSize = 0;

//------------------counts for testing
uint16_t fskTransmitCount = 0;
uint16_t fskReceiveCount = 0; 

//---------------------LOCAL FUNCTION PROTOTYPES--------------------------
void extintFSK_detection_callback(void);
void configure_extintFSK_channel(void);
void configure_extintFSK_callbacks(void);

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void SX1272FskInit( void )
{
    RFState = RF_STATE_IDLE;

	//------------------------
	// read the base buffer from the radio 
	//------------------------
    SX1272ReadBuffer2( REG_OPMODE, SX1272Regs+1, 0x70 - 1 );
//beth     SX1272WriteBuffer( REG_OPMODE, &FSK_DEFAULT[1], 0x70 - 1 );	 
	SX1272ReadBuffer2( REG_OPMODE, SX1272Regs+1, 0x70 - 1 );
	//----------------------------------------------
    // Set the device in FSK mode and Sleep Mode
	//----------------------------------------------
	//horton boc
    SX1272FskSetOpMode( RF_OPMODE_SLEEP );
    SX1272->RegOpMode = ( SX1272->RegOpMode & RF_OPMODE_LONGRANGEMODE_MASK ) | RF_OPMODE_LONGRANGEMODE_OFF;
    SX1272Write( REG_OPMODE, SX1272->RegOpMode );    
	SX1272ReadBuffer2( REG_OPMODE, SX1272Regs+1, 0x70 - 1 );  	
	//horton eoc
    //--------------------------------------
    // Then we initialize the device register 
	// structure with the value of our setup
	//--------------------------------------
    SX1272->RegPaConfig = ( SX1272->RegPaConfig & ~RF_PACONFIG_PASELECT_PABOOST ) | RF_PACONFIG_PASELECT_PABOOST;
    SX1272->RegLna = RF_LNA_GAIN_G1 | RF_LNA_BOOST_ON;

    if( FskSettings.AfcOn == true )
    {
        SX1272->RegRxConfig = RF_RXCONFIG_RESTARTRXONCOLLISION_OFF | RF_RXCONFIG_AFCAUTO_ON |
                              RF_RXCONFIG_AGCAUTO_ON | RF_RXCONFIG_RXTRIGER_PREAMBLEDETECT;
    }
    else
    {
        SX1272->RegRxConfig = RF_RXCONFIG_RESTARTRXONCOLLISION_OFF | RF_RXCONFIG_AFCAUTO_OFF |
                              RF_RXCONFIG_AGCAUTO_ON | RF_RXCONFIG_RXTRIGER_PREAMBLEDETECT;
    }

    SX1272->RegPreambleLsb = 8;
    
    SX1272->RegPreambleDetect = RF_PREAMBLEDETECT_DETECTOR_ON | RF_PREAMBLEDETECT_DETECTORSIZE_2 |
                                RF_PREAMBLEDETECT_DETECTORTOL_10;
    
    SX1272->RegRssiThresh = 0xFF;

    SX1272->RegSyncConfig = RF_SYNCCONFIG_AUTORESTARTRXMODE_WAITPLL_ON | RF_SYNCCONFIG_PREAMBLEPOLARITY_AA |
                            RF_SYNCCONFIG_SYNC_ON | RF_SYNCCONFIG_FIFOFILLCONDITION_AUTO |
                            RF_SYNCCONFIG_SYNCSIZE_4;

    SX1272->RegSyncValue1 = 0x69;
    SX1272->RegSyncValue2 = 0x81;
    SX1272->RegSyncValue3 = 0x7E;
    SX1272->RegSyncValue4 = 0x96;

    SX1272->RegPacketConfig1 = RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE | RF_PACKETCONFIG1_DCFREE_OFF |
                               ( FskSettings.CrcOn << 4 ) | RF_PACKETCONFIG1_CRCAUTOCLEAR_ON |
                               RF_PACKETCONFIG1_ADDRSFILTERING_OFF | RF_PACKETCONFIG1_CRCWHITENINGTYPE_CCITT;
    SX1272FskGetPacketCrcOn( ); 

    SX1272->RegPayloadLength = FskSettings.PayloadLength;
	//------------------------------------------------------
    // we can now update the registers with our configuration
    SX1272WriteBuffer( REG_OPMODE, SX1272Regs + 1, 0x70 - 1 );
	//-------------------------------------
    // then we need to set the RF settings 
    SX1272FskSetRFFrequency( FskSettings.RFFrequency );
    SX1272FskSetBitrate( FskSettings.Bitrate );
    SX1272FskSetFdev( FskSettings.Fdev );
    SX1272FskSetPa20dBm( true );
    SX1272FskSetRFPower( FskSettings.Power );
    SX1272FskSetDccBw( &SX1272->RegRxBw, 0, FskSettings.RxBw );
    SX1272FskSetDccBw( &SX1272->RegAfcBw, 0, FskSettings.RxBwAfc );
    SX1272FskSetRssiOffset( -6 );
	SX1272->RegFifoThresh = RF_FIFOTHRESH_TXSTARTCONDITION_FIFONOTEMPTY | 0x18; // 24 bytes of data
	SX1272Write( REG_FIFOTHRESH, SX1272->RegFifoThresh );	

	if (setCW != 0)
	{
		SX1272FskSetPacketConfig2(1);  //if non-zero, continuous
	}

	SX1272FskSetOpMode( RF_OPMODE_STANDBY );
	SX1272ReadBuffer2( REG_OPMODE, SX1272Regs+1, 0x70 - 1 );	

 
	
	if (setCW != 0)
	{
//		SX1272FskSetOpMode(RF_OPMODE_TRANSMITTER);
	//------------------
	// turn on transmitter
		SX1272->RegOpMode = ( SX1272->RegOpMode & RF_OPMODE_MASK ) | RF_OPMODE_TRANSMITTER;
		SX1272Write( REG_OPMODE, SX1272->RegOpMode );    		
	} 
}

void SX1272FskSetOpMode( uint8_t opMode )
{
  
        SX1272->RegOpMode = ( SX1272->RegOpMode & RF_OPMODE_MASK ) | opMode;

        SX1272Write( REG_OPMODE, SX1272->RegOpMode );        
}

uint8_t SX1272FskGetOpMode( void )
{
    SX1272Read( REG_OPMODE, &SX1272->RegOpMode );
    
    return SX1272->RegOpMode & ~RF_OPMODE_MASK;
}

 
void FSKGetCounts(uint16_t *txCount,uint16_t *rxCount)
{
	*txCount = fskTransmitCount;
	*rxCount = fskReceiveCount; 
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void configure_extintFSK_channel(void)
{
	struct extint_chan_conf config_extint_chan;
	extint_chan_get_config_defaults(&config_extint_chan);
	config_extint_chan.gpio_pin = PIN_PB09A_EIC_EXTINT9;  //PIN_PB31A_EIC_EXTINT15;
	config_extint_chan.gpio_pin_mux = MUX_PB09A_EIC_EXTINT9;  // MUX_PB31A_EIC_EXTINT15;
	config_extint_chan.gpio_pin_pull = EXTINT_PULL_UP;
	config_extint_chan.detection_criteria = EXTINT_DETECT_RISING; //EXTINT_DETECT_BOTH;
	extint_chan_set_config(9, &config_extint_chan);  //15
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void configure_extintFSK_callbacks(void)
{
	extint_register_callback(extintFSK_detection_callback,9,EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(9,EXTINT_CALLBACK_TYPE_DETECT);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void extintFSK_detection_callback(void)
{
	//	bool pin_state = port_pin_get_input_level(TPS_IRQ);
	schedByte |= SCHEDBYTE_RFFSK;
	schedByte |= SCHEDBYTE_RFLORA;
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void AppFskTask(void)
{
	if ((setCW == 0)&&(whichRadio == WHICHRADIO_FSK))
	{	 
	SX1272Read( REG_IRQFLAGS1, &SX1272->RegIrqFlags1 );
	SX1272Read( REG_IRQFLAGS2, &SX1272->RegIrqFlags2 );
	//----------------------------
	/// see if payload ready for a receive event 
	//----------------------------
 	if ((SX1272->RegIrqFlags2 & 0x04)!= 0) // PayloadReady/CrcOk
	{
		fskReceiveCount++;
		SX1272ReadFifo(RFBuffer,1);
		RxPacketSize = RFBuffer[0];				
//				RxPacketSize = SX1272->RegPayloadLength;	 
		SX1272ReadFifo(RFBuffer+1, RxPacketSize+1 );
/*		
		done = 0;
		i = 0;
		while ((done == 0) && (i<10))
		{
			SX1272Read( REG_IRQFLAGS2, &SX1272->RegIrqFlags2 );		
			if ((SX1272->RegIrqFlags2 & 0x04)!= 0)
			{	
				SX1272ReadFifo(&tempBuffer,1);
			}
			else
			{
				done = 1; 
			}
			i++;					
		}
*/		
		if (testRFBufferOffset >= MAXPACKETS2)
		{
			testRFBufferOffset = 0; 
		}
		testRFBuffer[testRFBufferOffset].Length = RFBuffer[0];
		testRFBuffer[testRFBufferOffset].Command[0] = RFBuffer[2]; 
		testRFBuffer[testRFBufferOffset].Command[1] = RFBuffer[3]; 
#if REMOTEBOARD		
		AppScreenFSKReadingIn(RFBuffer,testRFBufferOffset);
#endif		
		testRFBufferOffset++;	
		if (testRFBufferOffset >= MAXPACKETS2)
		{
			testRFBufferOffset = 0;
		}	
#if BRAKEBOARD		
		AppProtocolBrake(RFBuffer);
#endif		
#if REMOTEBOARD
		AppProtocolRemote(RFBuffer);
//		SX1272FskSetOpMode( RF_OPMODE_STANDBY);		
#endif						
	}
	//----------------------------
	/// see if packetSent
	//----------------------------
 	if ((SX1272->RegIrqFlags2 & 0x08)!= 0)  
	{
         SX1272FskSetOpMode( RF_OPMODE_RECEIVER );
#if REMOTEBOARD
		commSupTimer = COMM_SUP_TIME_REMOTE;
#endif		 
		if (switchOnTransmit != 0)
		{
			whichRadio = switchOnTransmit; 
			CommInit();
			switchOnTransmit = 0; 
		}
	}
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void AppFskReceiveStart(void)
{
	uint8_t done; 
 
 
	//------------------------
	// set up receive.
	//------------------------
	SX1272Read( REG_IRQFLAGS1, &SX1272->RegIrqFlags1 );
	SX1272Read( REG_IRQFLAGS2, &SX1272->RegIrqFlags2 );
        
    memset( RFBuffer, 0, ( size_t )RF_BUFFER_SIZE );
	
	//--------------------------------
	// set interrupt pin to processor
    //           PayloadReady
    SX1272->RegDioMapping1 = RF_DIOMAPPING1_DIO0_00 |RF_DIOMAPPING1_DIO1_11 ;
    SX1272->RegDioMapping2 = 0;
    SX1272WriteBuffer( REG_DIOMAPPING1, &SX1272->RegDioMapping1, 2);           	

	//---------------------------
	// enable the rf433 interrupt
	configure_extintFSK_channel();
	configure_extintFSK_callbacks();

    SX1272FskSetOpMode( RF_OPMODE_STANDBY );
	//----------------------
	// the mode was changed to Receiver 
	// - check when ModeReady is set .. bit 7 of IRQ1
	done = 0;
	while (done == 0)
	{
		SX1272Read( REG_IRQFLAGS1, &SX1272->RegIrqFlags1 );
		if ((SX1272->RegIrqFlags1 & 0x80)!= 0) 
		{
			done = 1;	
		}				
	}
  
    SX1272FskSetOpMode( RF_OPMODE_RECEIVER );
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
uint8_t SX1272FskTransmit(uint8_t *txBuffer,uint8_t length)
{
	uint8_t done,i,*ptr; 
    uint8_t status; 
	status = 1; 
 		  
	fskTransmitCount++;	   
		         
    SX1272Read( REG_OPMODE, &SX1272->RegOpMode );   		
	if ((SX1272->RegOpMode & ~RF_OPMODE_MASK)!= 0x001)
	{	 
		SX1272FskSetOpMode( RF_OPMODE_STANDBY );
		//----------------------
		// the mode was changed to Receiver 
		// - check when ModeReady is set .. bit 7 of IRQ1
		done = 0;
		while (done == 0)
		{
			SX1272Read( REG_IRQFLAGS1, &SX1272->RegIrqFlags1 );
			if ((SX1272->RegIrqFlags1 & 0x80)!= 0) 
			{
				done = 1;	
			}				
		}
  		
	}
	//-----------------------
	// check if the radio is ready to transmit.
	//-----------------------
	ptr = txBuffer; 
	RFBuffer[0] = length;
	for (i=0;i<length;i++)
	{
		RFBuffer[i+1] = *ptr++;
	}
    TxPacketSize = length; 
//	SX1272WriteFifo( ( uint8_t* )&TxPacketSize, 1 );
	SX1272WriteFifo( RFBuffer, TxPacketSize+1);		
	//------------------
	// turn on transmitter
	SX1272->RegOpMode = ( SX1272->RegOpMode & RF_OPMODE_MASK ) | RF_OPMODE_TRANSMITTER;
    SX1272Write( REG_OPMODE, SX1272->RegOpMode );       		
	//------------------------------
	// wait for tx ready.
	//-----------------------------
	done = 0;
	wdog = 0x00ff; 
    while ((done == 0)&&(wdog>0))
    {
	       SX1272Read( REG_IRQFLAGS1, &SX1272->RegIrqFlags1 );
	       if ((SX1272->RegIrqFlags1 & 0x20)!= 0)
	        {
		        done = 1;
	        }
    }

    RFState = RF_STATE_TX_RUNNING;
 /*       
        done = 0;
        while (done == 0)
        {
	        SX1272Read( REG_IRQFLAGS1, &SX1272->RegIrqFlags1 );
	        SX1272Read( REG_IRQFLAGS2, &SX1272->RegIrqFlags2 );
	        if ((SX1272->RegIrqFlags2 & 0x08)!= 0)
	        {
		        // PacketSent
		        done = 1;
//		        SX1272FskSetOpMode( RF_OPMODE_STANDBY );
	        }
        }
*/
     return status;
}
 
 