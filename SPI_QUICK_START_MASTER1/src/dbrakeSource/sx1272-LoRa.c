//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: sx1272-LoRa.c
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// Processor: STM32F103R
// TOOLS: IAR Workbench
// DATE:
// CONTENTS: This file contains
//------------------------------------------------------------------------------
// HISTORY: This file
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
/*! 
 * \file       sx1272-LoRa.c
 * \brief      SX1272 RF chip driver mode LoRa
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include <string.h>
 
#include "radio.h"
#include "driverXSPI.h"
#include "sx1272.h"
#include "sx1272-LoRaMisc.h"
#include "sx1272-LoRa.h"
#include "appProtocol.h"
#include "appLCD.h"



const uint8_t LORA_DEFAULT[112] = {0x00,0x01,0x1a,0x0b,0x00,0x52,0xe4,0xc0,0x00,0x0f,
								   0x19,0x2b,0x20,0x00,0x80,0x00,0x00,0x00,0x00,0x00, //0x0D - 0x13
								   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,//x1D
								   0x74,0x64,0x00,0x08,0x01,0xFF,0x00,0x00,0x00,0x00,//x27
								   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//x31
								   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//x3b
								   0x00,0x00,0x00,0x00,0x00,0x00,0x22,0x13,0x0e,0x5b,
								   0xdb,0x24,0x0D,0xFD,0x3A,0x2e,0x00,0x03,0x00,0x00,
								   0x00,0x00,0x04,0x23,0x3F,0xFE,0x3F,0xD4,0x09,0x05,
								   0x84,0x0b,0xd0,0x0b,0xd0,0x32,0x2b,0x14,0x00,0x00,
								   0x12,0x00,0x00,0x00,0x0f,0xe0,0x00,0x0c,0xf2,0x14,
								   0x25,0x06
								   };

/*!
 * Constant values need to compute the RSSI value
 */
#define RSSI_OFFSET                                 -137.0
#define NOISE_ABSOLUTE_ZERO                         -174.0
#define NOISE_FIGURE                                6.0

/*!
 * Precomputed signal bandwidth log values
 * Used to compute the Packet RSSI value.
 */
const double SignalBwLog[] =
{
    5.0969100130080564143587833158265,
    5.397940008672037609572522210551,
    5.6989700043360188047862611052755
};

const double RssiOffset[] =
{
    -137.0,
    -133.0,
    -127.0,
};

/*!
 * Frequency hopping frequencies table
 */
const int32_t HoppingFrequencies[] =
{
    916500000,
    923500000,
    906500000,
    917500000,
    917500000,
    909000000,
    903000000,
    916000000,
    912500000,
    926000000,
    925000000,
    909500000,
    913000000,
    918500000,
    918500000,
    902500000,
    911500000,
    926500000,
    902500000,
    922000000,
    924000000,
    903500000,
    913000000,
    922000000,
    926000000,
    910000000,
    920000000,
    922500000,
    911000000,
    922000000,
    909500000,
    926000000,
    922000000,
    918000000,
    925500000,
    908000000,
    917500000,
    926500000,
    908500000,
    916000000,
    905500000,
    916000000,
    903000000,
    905000000,
    915000000,
    913000000,
    907000000,
    910000000,
    926500000,
    925500000,
    911000000,
};

// Default settings
tLoRaSettings LoRaSettings =
{
    922330000,        // was 915 RFFrequency
    13,  //+6 13,//10,  //20,               // Power
    2,                // SignalBw [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved] 
    7,                // SpreadingFactor [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
    2,                // ErrorCoding [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
    true,             // CrcOn [0: OFF, 1: ON]
    false,            // ImplicitHeaderOn [0: OFF, 1: ON]
    0,                // RxSingleOn [0: Continuous, 1 Single] ...... was 1
    0,                // FreqHopOn [0: OFF, 1: ON]
    4,                // HopPeriod Hops every frequency hopping period symbols
    100,              // TxPacketTimeout
    100,              // RxPacketTimeout
    128,              // PayloadLength (used for implicit header mode)
};

	//-----------------------------
	// SX1272 LoRa registers variable
	//-----------------------------
tSX1272LR* SX1272LR;
	//------------------------
	// Local RF buffer for communication support
	//-------------------------------
uint8_t RFBufferLora[RF_BUFFER_SIZE];

/*!
 * RF state machine variable
 */
uint8_t RFLRState = RFLR_STATE_IDLE;

 
static uint16_t RxPacketSize = 0;
static int8_t RxPacketSnrEstimate;
static double RxPacketRssiValue;
static uint8_t RxGain = 1;
 
static uint16_t TxPacketSize = 0;
 

//------------------counts for testing
uint16_t loraTransmitCount = 0;
uint16_t loraReceiveCount = 0;

//---------------------LOCAL FUNCTION PROTOTYPES--------------------------
void extintLORA_detection_callback(void);
void configure_extintLORA_channel(void);
void configure_extintLORA_callbacks(void);
void SX1272LoRaReset( void );

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//
//==============================================================================
void SX1272LoRaInit( void )
{
    RFLRState = RFLR_STATE_IDLE;

	//------------------------
	// read the base buffer from the radio
	//------------------------
	SX1272ReadBuffer2( REG_LR_OPMODE, SX1272Regs+1, 0x70 - 1 );
	//beth     SX1272WriteBuffer( REG_OPMODE, &FSK_DEFAULT[1], 0x70 - 1 );
	SX1272ReadBuffer2( REG_LR_OPMODE, SX1272Regs+1, 0x70 - 1 );
 
 	//----------------------------------------------
    // Set the device in FSK mode and Sleep Mode
	//----------------------------------------------
	//horton boc
    SX1272LoRaSetOpMode( RFLR_OPMODE_SLEEP );
    SX1272LR->RegOpMode = ( SX1272LR->RegOpMode & RFLR_OPMODE_LONGRANGEMODE_MASK ) | RFLR_OPMODE_LONGRANGEMODE_ON;
    SX1272Write( REG_LR_OPMODE, SX1272LR->RegOpMode );    
	SX1272ReadBuffer2( REG_LR_OPMODE, SX1272Regs+1, 0x70 - 1 );  	
	//horton eoc
    //--------------------------------------
    // Then we initialize the device register
    // structure with the value of our setup
    //--------------------------------------
    SX1272LR->RegPaConfig = ( SX1272LR->RegPaConfig & ~RFLR_PACONFIG_PASELECT_PABOOST ) |
                            RFLR_PACONFIG_PASELECT_PABOOST;
    SX1272LR->RegLna = RFLR_LNA_GAIN_G1 | RFLR_LNA_BOOST_ON;

    SX1272WriteBuffer( REG_LR_OPMODE, SX1272Regs+1, 0x70 - 1 );
	
    // set the RF settings 
    SX1272LoRaSetRFFrequency( LoRaSettings.RFFrequency );
    SX1272LoRaSetPa20dBm( true );
    SX1272LoRaSetRFPower( LoRaSettings.Power );
    SX1272LoRaSetSpreadingFactor( LoRaSettings.SpreadingFactor ); // SF6 only operates in implicit header mode.
    SX1272LoRaSetErrorCoding( LoRaSettings.ErrorCoding );
    SX1272LoRaSetPacketCrcOn( LoRaSettings.CrcOn );
    SX1272LoRaSetSignalBandwidth( LoRaSettings.SignalBw );
    
    SX1272LoRaSetImplicitHeaderOn( LoRaSettings.ImplicitHeaderOn );
    SX1272LoRaSetSymbTimeout( 0x3FF );
    SX1272LoRaSetPayloadLength( LoRaSettings.PayloadLength );
    SX1272LoRaSetLowDatarateOptimize( true );
	
 
	if (setCW != 0)
	{
		SX1272LoRaSetContinuousTx(RFLR_MODEMCONFIG2_TXCONTINUOUSMODE_ON);
	}
	else
	{
		SX1272LoRaSetContinuousTx(RFLR_MODEMCONFIG2_TXCONTINUOUSMODE_OFF);
	}

    SX1272LoRaSetOpMode( RFLR_OPMODE_STANDBY );
	SX1272ReadBuffer2( REG_LR_OPMODE, SX1272Regs+1, 0x70 - 1 );		
	
	if (setCW != 0)
	{
		SX1272LoRaSetOpMode( RFLR_OPMODE_TRANSMITTER);
	}
}

/*
void SX1272LoRaSetDefaults( void )
{
    // REMARK: See SX1272 datasheet for modified default values.

    // Sets IF frequency selection manual
    SX1272LR->RegTestReserved31 = 0x43; // default value 0xC3
    SX1272Write( 0x31, SX1272LR->RegTestReserved31 );

    SX1272Read( REG_LR_VERSION, &SX1272LR->RegVersion );
}
*/

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void SX1272LoRaReset( void )
{
    SX1272Reset();  
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void SX1272LoRaSetOpMode( uint8_t opMode )
{
   
    SX1272LR->RegOpMode = ( SX1272LR->RegOpMode & RFLR_OPMODE_MASK ) | opMode;
    SX1272Write( REG_LR_OPMODE, SX1272LR->RegOpMode );        
}

uint8_t SX1272LoRaGetOpMode( void )
{
    SX1272Read( REG_LR_OPMODE, &SX1272LR->RegOpMode );
    
    return SX1272LR->RegOpMode & ~RFLR_OPMODE_MASK;
}

uint8_t SX1272LoRaReadRxGain( void )
{
    SX1272Read( REG_LR_LNA, &SX1272LR->RegLna );
    return( SX1272LR->RegLna >> 5 ) & 0x07;
}

double SX1272LoRaReadRssi( void )
{
    // Reads the RSSI value
    SX1272Read( REG_LR_RSSIVALUE, &SX1272LR->RegRssiValue );

    return RssiOffset[LoRaSettings.SignalBw] + ( double )SX1272LR->RegRssiValue;
}

uint8_t SX1272LoRaGetPacketRxGain( void )
{
    return RxGain;
}

int8_t SX1272LoRaGetPacketSnr( void )
{
    return RxPacketSnrEstimate;
}

double SX1272LoRaGetPacketRssi( void )
{
    return RxPacketRssiValue;
}

void SX1272LoRaStartRx( void )
{
    SX1272LoRaSetRFState( RFLR_STATE_RX_INIT );
}

void SX1272LoRaGetRxPacket( void *buffer, uint16_t *size )
{
    *size = RxPacketSize;
    RxPacketSize = 0;
    memcpy( ( void * )buffer, ( void * )RFBufferLora, ( size_t )*size );
}

void SX1272LoRaSetTxPacket( const void *buffer, uint16_t size )
{
    if( LoRaSettings.FreqHopOn == false )
    {
        TxPacketSize = size;
    }
    else
    {
        TxPacketSize = 255;
    }
    memcpy( ( void * )RFBufferLora, buffer, ( size_t )TxPacketSize ); 

    RFLRState = RFLR_STATE_TX_INIT;
}

uint8_t SX1272LoRaGetRFState( void )
{
    return RFLRState;
}

void SX1272LoRaSetRFState( uint8_t state )
{
    RFLRState = state;
}




//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void LORAGetCounts(uint16_t *txCount,uint16_t *rxCount)
{
	*txCount = loraTransmitCount;
	*rxCount = loraReceiveCount; 
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void configure_extintLORA_channel(void)
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
void configure_extintLORA_callbacks(void)
{
	extint_register_callback(extintLORA_detection_callback,9,EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(9,EXTINT_CALLBACK_TYPE_DETECT);
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void extintLORA_detection_callback(void)
{
	//	bool pin_state = port_pin_get_input_level(TPS_IRQ);
	schedByte |= SCHEDBYTE_RFLORA;
	schedByte |= SCHEDBYTE_RFFSK;
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void AppLoraTask(void)
{
		
	if ((setCW == 0)&&(setRXContinuous==0)&&(whichRadio == WHICHRADIO_LORA))
	{	 
		if (whichRadio == WHICHRADIO_LORA)
		{	
			SX1272Read(REG_LR_IRQFLAGS, &SX1272LR->RegIrqFlags);
			//----------------------------
			/// see if RX DONE for a receive event 
			//----------------------------
 			if (((SX1272LR->RegIrqFlags & 0x40)!= 0) && (setTXContinuous==0))
 			{ 		
				SX1272Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE  );
				RFLRState = RFLR_STATE_RX_DONE;		
		
				loraReceiveCount++;
				SX1272Read( REG_LR_FIFORXCURRENTADDR, &SX1272LR->RegFifoRxCurrentAddr );

 
				SX1272Read( REG_LR_NBRXBYTES, &SX1272LR->RegNbRxBytes );
				RxPacketSize = SX1272LR->RegNbRxBytes;
				RFBufferLora[0] = RxPacketSize; 
				SX1272LR->RegFifoAddrPtr = SX1272LR->RegFifoRxCurrentAddr;
				SX1272Write( REG_LR_FIFOADDRPTR, SX1272LR->RegFifoAddrPtr );
				SX1272ReadFifo( RFBufferLora+1, SX1272LR->RegNbRxBytes );
 
				if (testRFBufferOffset >= MAXPACKETS2)
				{
					testRFBufferOffset = 0; 
				}
				testRFBuffer[testRFBufferOffset].Length = RFBufferLora[1];  //0];
				testRFBuffer[testRFBufferOffset].Command[0] = RFBufferLora[3]; //2]; 
				testRFBuffer[testRFBufferOffset].Command[1] = RFBufferLora[4]; //3]; 
		#if REMOTEBOARD		
				AppScreenFSKReadingIn(&RFBufferLora[1],testRFBufferOffset);
		#endif		
				testRFBufferOffset++;	
				if (testRFBufferOffset >= MAXPACKETS2)
				{
					testRFBufferOffset = 0;
				}	
		#if BRAKEBOARD		
				AppProtocolBrake(&RFBufferLora[1]);
		#endif		
		#if REMOTEBOARD
				AppProtocolRemote(&RFBufferLora[1]);
		//		SX1272FskSetOpMode( RF_OPMODE_STANDBY);		
		#endif						
			}
			//----------------------------
			/// see if packetSent
			//----------------------------
 			if ((SX1272LR->RegIrqFlags & 0x08)!= 0)  
			{
				SX1272LoRaSetOpMode( RFLR_OPMODE_STANDBY );
				SX1272Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE  );	
				//--------------------------------
				// set interrupt pin to processor
				SX1272LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00; //| RFLR_DIOMAPPING1_DIO0_01;
				SX1272LR->RegDioMapping2 = 0;
				SX1272WriteBuffer( REG_LR_DIOMAPPING1, &SX1272LR->RegDioMapping1, 2 );		
				SX1272LoRaSetOpMode( RFLR_OPMODE_RECEIVER );	
		#if REMOTEBOARD
				commSupTimer = COMM_SUP_TIME_REMOTE; 
		#endif		
				if (switchOnTransmit != 0)
				{
					whichRadio = switchOnTransmit; 
					CommInit();
					switchOnTransmit = 0; 
				}
				if (setTXContinuous!=0)
				{
					SendOneMessage();
				}
			}
		}
	}
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
void AppLoraReceiveStart(void)
{
	 
 
 	//------------------------
	// set up receive.
	//------------------------
	SX1272Read(REG_LR_IRQFLAGS, &SX1272LR->RegIrqFlags);
    memset( RFBufferLora, 0, ( size_t )RF_BUFFER_SIZE );

	//---------------------------
	// enable the rf433 interrupt
	configure_extintLORA_channel();
	configure_extintLORA_callbacks();

    SX1272LoRaSetOpMode( RFLR_OPMODE_STANDBY );
 
	SX1272LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
        //RFLR_IRQFLAGS_RXDONE |
        RFLR_IRQFLAGS_PAYLOADCRCERROR |
        RFLR_IRQFLAGS_VALIDHEADER |
        //RFLR_IRQFLAGS_TXDONE |
        RFLR_IRQFLAGS_CADDONE |
        RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
        RFLR_IRQFLAGS_CADDETECTED;
	SX1272Write( REG_LR_IRQFLAGSMASK, SX1272LR->RegIrqFlagsMask );	
    // Clear Irq
    SX1272Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE  );
    
	if( LoRaSettings.FreqHopOn == true )
    {
	    SX1272LR->RegHopPeriod = LoRaSettings.HopPeriod;
		SX1272Read( REG_LR_HOPCHANNEL, &SX1272LR->RegHopChannel );
		SX1272LoRaSetRFFrequency( HoppingFrequencies[SX1272LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
    }
    else
    {
		SX1272LR->RegHopPeriod = 255;   //or 0 with transmit
    }
    SX1272Write( REG_LR_HOPPERIOD, SX1272LR->RegHopPeriod );	
	//--------------------------------
	// set interrupt pin to processor
	SX1272LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00; //| RFLR_DIOMAPPING1_DIO0_01;
	SX1272LR->RegDioMapping2 = 0;
	SX1272WriteBuffer( REG_LR_DIOMAPPING1, &SX1272LR->RegDioMapping1, 2 );	

    SX1272LR->RegFifoAddrPtr = SX1272LR->RegFifoRxBaseAddr;
    SX1272Write( REG_LR_FIFOADDRPTR, SX1272LR->RegFifoAddrPtr );
    
    SX1272LoRaSetOpMode( RFLR_OPMODE_RECEIVER );	


}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// FUNCTION:
//------------------------------------------------------------------------------
// This function
//==============================================================================
uint8_t SX1272LoraTransmit(uint8_t *txBuffer,uint16_t length)
{
	uint16_t i;
	uint8_t *ptr; 
    uint8_t status; 
	status = 1; 
 		  
	loraTransmitCount++;	   
		         
    SX1272Read( REG_LR_OPMODE, &SX1272LR->RegOpMode );   		
	if ((SX1272LR->RegOpMode & ~RFLR_OPMODE_MASK)!= 0x01)
	{	 
		SX1272LoRaSetOpMode( RFLR_OPMODE_STANDBY );  		
	}
	//-----------------------
	// check if the radio is ready to transmit.
	//-----------------------
	ptr = txBuffer; 
	RFBufferLora[0] = length;
	for (i=0;i<length;i++)
	{
		RFBufferLora[i+1] = *ptr++;
	}
	 
    TxPacketSize = length+1; 
        // Initializes the payload size
        SX1272LR->RegPayloadLength = TxPacketSize;
        SX1272Write( REG_LR_PAYLOADLENGTH, SX1272LR->RegPayloadLength );
        
        SX1272LR->RegFifoTxBaseAddr = 0x00; // Full buffer used for Tx
        SX1272Write( REG_LR_FIFOTXBASEADDR, SX1272LR->RegFifoTxBaseAddr );

        SX1272LR->RegFifoAddrPtr = SX1272LR->RegFifoTxBaseAddr;
        SX1272Write( REG_LR_FIFOADDRPTR, SX1272LR->RegFifoAddrPtr );
        
        // Write payload buffer to LORA modem
        SX1272WriteFifo( RFBufferLora, TxPacketSize);  //SX1272LR->RegPayloadLength );
		
	//--------------------------------
	// set interrupt pin to processor
	SX1272LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00| RFLR_DIOMAPPING1_DIO0_01;
	SX1272LR->RegDioMapping2 = 0;
	SX1272WriteBuffer( REG_LR_DIOMAPPING1, &SX1272LR->RegDioMapping1, 2 );		
	//------------------
	// turn on transmitter
    SX1272LoRaSetOpMode( RFLR_OPMODE_TRANSMITTER );     		
  
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
 

#if 0
/*Process the LoRa modem Rx and Tx state machines depending on the
 *        SX1272 operating mode.
 *
 * \retval rfState Current RF state [RF_IDLE, RF_BUSY, 
 *                                   RF_RX_DONE, RF_RX_TIMEOUT,
 *                                   RF_TX_DONE, RF_TX_TIMEOUT]
 */
#define BUFFER_SIZE                                 9 // Define the payload size here
extern uint16_t BufferSize;			// RF buffer size
extern uint8_t Buffer[BUFFER_SIZE];					// RF buffer
uint32_t SX1272LoRaProcess( void )
{
    uint32_t result = RF_BUSY;
	uint8_t done,i; 
	uint16_t j;
	
#if RADIO_TRANSMIT	
        Buffer[0] = 'P';
        Buffer[1] = 'I';
        Buffer[2] = 'N';
        Buffer[3] = 'G';
        for( i = 4; i < BUFFER_SIZE; i++ )
        {
	        Buffer[i] = i - 4;
        }
        SX1272SetTxPacket( Buffer, BUFFER_SIZE );
        
        SX1272Read( REG_LR_IRQFLAGS, &SX1272LR->RegIrqFlags );
		
        SX1272LoRaSetOpMode( RFLR_OPMODE_STANDBY );

        if( LoRaSettings.FreqHopOn == true )
        {
	        SX1272LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
	        RFLR_IRQFLAGS_RXDONE |
	        RFLR_IRQFLAGS_PAYLOADCRCERROR |
	        RFLR_IRQFLAGS_VALIDHEADER |
	    //    RFLR_IRQFLAGS_TXDONE |
	        RFLR_IRQFLAGS_CADDONE |
	        //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
	        RFLR_IRQFLAGS_CADDETECTED;
	        SX1272LR->RegHopPeriod = LoRaSettings.HopPeriod;

	        SX1272Read( REG_LR_HOPCHANNEL, &SX1272LR->RegHopChannel );
	        SX1272LoRaSetRFFrequency( HoppingFrequencies[SX1272LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
        }
        else
        {
	        SX1272LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
	        RFLR_IRQFLAGS_RXDONE |
	        RFLR_IRQFLAGS_PAYLOADCRCERROR |
	        RFLR_IRQFLAGS_VALIDHEADER |
	  //      RFLR_IRQFLAGS_TXDONE |
	        RFLR_IRQFLAGS_CADDONE |
	        RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
	        RFLR_IRQFLAGS_CADDETECTED;
	        SX1272LR->RegHopPeriod = 0;
        }
        SX1272Write( REG_LR_HOPPERIOD, SX1272LR->RegHopPeriod );
        SX1272Write( REG_LR_IRQFLAGSMASK, SX1272LR->RegIrqFlagsMask );

        // Initializes the payload size
        SX1272LR->RegPayloadLength = TxPacketSize;
        SX1272Write( REG_LR_PAYLOADLENGTH, SX1272LR->RegPayloadLength );
        
        SX1272LR->RegFifoTxBaseAddr = 0x00; // Full buffer used for Tx
        SX1272Write( REG_LR_FIFOTXBASEADDR, SX1272LR->RegFifoTxBaseAddr );

        SX1272LR->RegFifoAddrPtr = SX1272LR->RegFifoTxBaseAddr;
        SX1272Write( REG_LR_FIFOADDRPTR, SX1272LR->RegFifoAddrPtr );
        
        // Write payload buffer to LORA modem
        SX1272WriteFifo( RFBufferLora, SX1272LR->RegPayloadLength );
        // TxDone               RxTimeout                   FhssChangeChannel          ValidHeader
 //       SX1272LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_01;
        // PllLock              Mode Ready
 //       SX1272LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_01 | RFLR_DIOMAPPING2_DIO5_00;
 //       SX1272WriteBuffer( REG_LR_DIOMAPPING1, &SX1272LR->RegDioMapping1, 2 );

        SX1272LoRaSetOpMode( RFLR_OPMODE_TRANSMITTER );

        RFLRState = RFLR_STATE_TX_RUNNING;
//        break;
//        case RFLR_STATE_TX_RUNNING:

		//------------------------------
		// wait for tx ready.
		//-----------------------------
		for (j=0;j<0xff;j++)
		{
			done++;
		};
		done = 0;
		while (done == 0)
		{
			for(j=0;j<0xff;j++){ done = 0;};
			SX1272Read( REG_LR_IRQFLAGS, &SX1272LR->RegIrqFlags );
			if ((SX1272LR->RegIrqFlags & 0x08)!= 0)  //checkfor TXDONE
			{
				done = 1;
				// Clear Irq
				SX1272Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE  );
				RFLRState = RFLR_STATE_TX_DONE;				
			}
			if ((SX1272LR->RegIrqFlags & 0x02)!= 0)  // FHSS Changed Channel
			{
				if( LoRaSettings.FreqHopOn == true )
				{
					SX1272Read( REG_LR_HOPCHANNEL, &SX1272LR->RegHopChannel );
					SX1272LoRaSetRFFrequency( HoppingFrequencies[SX1272LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
				}
				// Clear Irq
				SX1272Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL );
			}			
		}
//   case RFLR_STATE_TX_DONE:
        // optimize the power consumption by switching off the transmitter as soon as the packet has been sent
        SX1272LoRaSetOpMode( RFLR_OPMODE_STANDBY );

        RFLRState = RFLR_STATE_IDLE;
        result = RF_TX_DONE;        
 #else
   
        SX1272LoRaSetOpMode( RFLR_OPMODE_STANDBY );

        SX1272LR->RegIrqFlagsMask =
								//bb	RFLR_IRQFLAGS_RXTIMEOUT |
                                    //RFLR_IRQFLAGS_RXDONE |
                                    //RFLR_IRQFLAGS_PAYLOADCRCERROR |
                             //bb       RFLR_IRQFLAGS_VALIDHEADER |
                                    RFLR_IRQFLAGS_TXDONE |
                                    RFLR_IRQFLAGS_CADDONE |
                                    //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                    RFLR_IRQFLAGS_CADDETECTED;
        SX1272Write( REG_LR_IRQFLAGSMASK, SX1272LR->RegIrqFlagsMask );

        if( LoRaSettings.FreqHopOn == true )
        {
            SX1272LR->RegHopPeriod = LoRaSettings.HopPeriod;

            SX1272Read( REG_LR_HOPCHANNEL, &SX1272LR->RegHopChannel );
            SX1272LoRaSetRFFrequency( HoppingFrequencies[SX1272LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
        }
        else
        {
            SX1272LR->RegHopPeriod = 255;
        }
        
        SX1272Write( REG_LR_HOPPERIOD, SX1272LR->RegHopPeriod );
                
                                    // RxDone                    RxTimeout                   FhssChangeChannel           CadDone
//        SX1272LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
                                    // CadDetected               ModeReady
//        SX1272LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
//        SX1272WriteBuffer( REG_LR_DIOMAPPING1, &SX1272LR->RegDioMapping1, 2 );
    
        if( LoRaSettings.RxSingleOn == true ) // Rx single mode
        {

            SX1272LoRaSetOpMode( RFLR_OPMODE_RECEIVER_SINGLE );
        }
        else // Rx continuous mode
        {
            SX1272LR->RegFifoAddrPtr = SX1272LR->RegFifoRxBaseAddr;
            SX1272Write( REG_LR_FIFOADDRPTR, SX1272LR->RegFifoAddrPtr );
            
            SX1272LoRaSetOpMode( RFLR_OPMODE_RECEIVER );
        }
        
        memset( RFBufferLora, 0, ( size_t )RF_BUFFER_SIZE );

        PacketTimeout = LoRaSettings.RxPacketTimeout;
        RxTimeoutTimer = GET_TICK_COUNT( );
        RFLRState = RFLR_STATE_RX_RUNNING;
 
 //   case RFLR_STATE_RX_RUNNING:
		//------------------------------
		//  
		//-----------------------------
		done = 0;
		while (done == 0)
		{
			SX1272Read( REG_LR_IRQFLAGS, &SX1272LR->RegIrqFlags );
			if ((SX1272LR->RegIrqFlags & 0x40)!= 0)   // RxDone
			{
				done = 1;
				RxTimeoutTimer = GET_TICK_COUNT( );
				if( LoRaSettings.FreqHopOn == true )
				{
					SX1272Read( REG_LR_HOPCHANNEL, &SX1272LR->RegHopChannel );
					SX1272LoRaSetRFFrequency( HoppingFrequencies[SX1272LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
				}
				// Clear Irq
				SX1272Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE  );
				RFLRState = RFLR_STATE_RX_DONE;
			}
			if ((SX1272LR->RegIrqFlags & 0x02)!= 0)  // FHSS Changed Channel
			{
				RxTimeoutTimer = GET_TICK_COUNT( );
				if( LoRaSettings.FreqHopOn == true )
				{
					SX1272Read( REG_LR_HOPCHANNEL, &SX1272LR->RegHopChannel );
					SX1272LoRaSetRFFrequency( HoppingFrequencies[SX1272LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
				}
				// Clear Irq
				SX1272Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL );
				// Debug
				RxGain = SX1272LoRaReadRxGain( );
			}
		}

        if( LoRaSettings.RxSingleOn == true ) // Rx single mode
        {
            if( ( GET_TICK_COUNT( ) - RxTimeoutTimer ) > PacketTimeout )
            {
                RFLRState = RFLR_STATE_RX_TIMEOUT;
            }
        }
        
//    case RFLR_STATE_RX_DONE:

        SX1272Read( REG_LR_IRQFLAGS, &SX1272LR->RegIrqFlags );
        if( ( SX1272LR->RegIrqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR ) == RFLR_IRQFLAGS_PAYLOADCRCERROR )
        {
            // Clear Irq
            SX1272Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_PAYLOADCRCERROR  );
//-------------------------ERROR PROCESSING ADD BACK IN BETH             
/*			
            if( LoRaSettings.RxSingleOn == true ) // Rx single mode
            {
                RFLRState = RFLR_STATE_RX_INIT;
            }
            else
            {
                RFLRState = RFLR_STATE_RX_RUNNING;
            }
			break;
*/			
        }
        
        {
            uint8_t rxSnrEstimate;
            SX1272Read( REG_LR_PKTSNRVALUE, &rxSnrEstimate );
            if( rxSnrEstimate & 0x80 ) // The SNR sign bit is 1
            {
                // Invert and divide by 4
                RxPacketSnrEstimate = ( ( ~rxSnrEstimate + 1 ) & 0xFF ) >> 2;
                RxPacketSnrEstimate = -RxPacketSnrEstimate;
            }
            else
            {
                // Divide by 4
                RxPacketSnrEstimate = ( rxSnrEstimate & 0xFF ) >> 2;
            }
        }
        
        if( RxPacketSnrEstimate < 0 )
        {
            RxPacketRssiValue = NOISE_ABSOLUTE_ZERO + 10.0 * SignalBwLog[LoRaSettings.SignalBw] + NOISE_FIGURE + ( double )RxPacketSnrEstimate;
        }
        else
        {    
            SX1272Read( REG_LR_PKTRSSIVALUE, &SX1272LR->RegPktRssiValue );
            RxPacketRssiValue = RssiOffset[LoRaSettings.SignalBw] + ( double )SX1272LR->RegPktRssiValue;
        }

        if( LoRaSettings.RxSingleOn == true ) // Rx single mode
        {
            SX1272LR->RegFifoAddrPtr = SX1272LR->RegFifoRxBaseAddr;
            SX1272Write( REG_LR_FIFOADDRPTR, SX1272LR->RegFifoAddrPtr );

            if( LoRaSettings.ImplicitHeaderOn == true )
            {
                RxPacketSize = SX1272LR->RegPayloadLength;
                SX1272ReadFifo( RFBufferLora, SX1272LR->RegPayloadLength );
            }
            else
            {
                SX1272Read( REG_LR_NBRXBYTES, &SX1272LR->RegNbRxBytes );
                RxPacketSize = SX1272LR->RegNbRxBytes;
                SX1272ReadFifo( RFBufferLora, SX1272LR->RegNbRxBytes );
            }
        }
        else // Rx continuous mode
        {
            SX1272Read( REG_LR_FIFORXCURRENTADDR, &SX1272LR->RegFifoRxCurrentAddr );

            if( LoRaSettings.ImplicitHeaderOn == true )
            {
                RxPacketSize = SX1272LR->RegPayloadLength;
                SX1272LR->RegFifoAddrPtr = SX1272LR->RegFifoRxCurrentAddr;
                SX1272Write( REG_LR_FIFOADDRPTR, SX1272LR->RegFifoAddrPtr );
                SX1272ReadFifo( RFBufferLora, SX1272LR->RegPayloadLength );
            }
            else
            {
                SX1272Read( REG_LR_NBRXBYTES, &SX1272LR->RegNbRxBytes );
                RxPacketSize = SX1272LR->RegNbRxBytes;
                SX1272LR->RegFifoAddrPtr = SX1272LR->RegFifoRxCurrentAddr;
                SX1272Write( REG_LR_FIFOADDRPTR, SX1272LR->RegFifoAddrPtr );
                SX1272ReadFifo( RFBufferLora, SX1272LR->RegNbRxBytes );
            }
        }
        
        if( LoRaSettings.RxSingleOn == true ) // Rx single mode
        {
            RFLRState = RFLR_STATE_RX_INIT;
        }
        else // Rx continuous mode
        {
            RFLRState = RFLR_STATE_RX_RUNNING;
        }
        result = RF_RX_DONE;
#endif		
#if 0				
        break;
    case RFLR_STATE_RX_TIMEOUT:
        RFLRState = RFLR_STATE_RX_INIT;
        result = RF_RX_TIMEOUT;
        break;
    case RFLR_STATE_TX_INIT:

        SX1272LoRaSetOpMode( RFLR_OPMODE_STANDBY );

        if( LoRaSettings.FreqHopOn == true )
        {
            SX1272LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                        RFLR_IRQFLAGS_RXDONE |
                                        RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                        RFLR_IRQFLAGS_VALIDHEADER |
                                        //RFLR_IRQFLAGS_TXDONE |
                                        RFLR_IRQFLAGS_CADDONE |
                                        //RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                        RFLR_IRQFLAGS_CADDETECTED;
            SX1272LR->RegHopPeriod = LoRaSettings.HopPeriod;

            SX1272Read( REG_LR_HOPCHANNEL, &SX1272LR->RegHopChannel );
            SX1272LoRaSetRFFrequency( HoppingFrequencies[SX1272LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
        }
        else
        {
            SX1272LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                        RFLR_IRQFLAGS_RXDONE |
                                        RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                        RFLR_IRQFLAGS_VALIDHEADER |
                                        //RFLR_IRQFLAGS_TXDONE |
                                        RFLR_IRQFLAGS_CADDONE |
                                        RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
                                        RFLR_IRQFLAGS_CADDETECTED;
            SX1272LR->RegHopPeriod = 0;
        }
        SX1272Write( REG_LR_HOPPERIOD, SX1272LR->RegHopPeriod );
        SX1272Write( REG_LR_IRQFLAGSMASK, SX1272LR->RegIrqFlagsMask );

        // Initializes the payload size
        SX1272LR->RegPayloadLength = TxPacketSize;
        SX1272Write( REG_LR_PAYLOADLENGTH, SX1272LR->RegPayloadLength );
        
        SX1272LR->RegFifoTxBaseAddr = 0x00; // Full buffer used for Tx
        SX1272Write( REG_LR_FIFOTXBASEADDR, SX1272LR->RegFifoTxBaseAddr );

        SX1272LR->RegFifoAddrPtr = SX1272LR->RegFifoTxBaseAddr;
        SX1272Write( REG_LR_FIFOADDRPTR, SX1272LR->RegFifoAddrPtr );
        
        // Write payload buffer to LORA modem
        SX1272WriteFifo( RFBuffer, SX1272LR->RegPayloadLength );
                                        // TxDone               RxTimeout                   FhssChangeChannel          ValidHeader         
        SX1272LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_01;
                                        // PllLock              Mode Ready
        SX1272LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_01 | RFLR_DIOMAPPING2_DIO5_00;
        SX1272WriteBuffer( REG_LR_DIOMAPPING1, &SX1272LR->RegDioMapping1, 2 );

        SX1272LoRaSetOpMode( RFLR_OPMODE_TRANSMITTER );

        RFLRState = RFLR_STATE_TX_RUNNING;
        break;
    case RFLR_STATE_TX_RUNNING:
        if( DIO0 == 1 ) // TxDone
        {
            // Clear Irq
            SX1272Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE  );
            RFLRState = RFLR_STATE_TX_DONE;   
        }
		if (DIO0 ==1)
		//AA       if( DIO2 == 1 ) // FHSS Changed Channel
        {
            if( LoRaSettings.FreqHopOn == true )
            {
                SX1272Read( REG_LR_HOPCHANNEL, &SX1272LR->RegHopChannel );
                SX1272LoRaSetRFFrequency( HoppingFrequencies[SX1272LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
            }
            // Clear Irq
            SX1272Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL );
        }
        break;
    case RFLR_STATE_TX_DONE:
        // optimize the power consumption by switching off the transmitter as soon as the packet has been sent
        SX1272LoRaSetOpMode( RFLR_OPMODE_STANDBY );

        RFLRState = RFLR_STATE_IDLE;
        result = RF_TX_DONE;
        break;
    case RFLR_STATE_CAD_INIT:    
        SX1272LoRaSetOpMode( RFLR_OPMODE_STANDBY );
    
        SX1272LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
                                    RFLR_IRQFLAGS_RXDONE |
                                    RFLR_IRQFLAGS_PAYLOADCRCERROR |
                                    RFLR_IRQFLAGS_VALIDHEADER |
                                    RFLR_IRQFLAGS_TXDONE |
                                    //RFLR_IRQFLAGS_CADDONE |
                                    RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL; // |
                                    //RFLR_IRQFLAGS_CADDETECTED;
        SX1272Write( REG_LR_IRQFLAGSMASK, SX1272LR->RegIrqFlagsMask );
           
                                    // RxDone                   RxTimeout                   FhssChangeChannel           CadDone
        SX1272LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
                                    // CAD Detected              ModeReady
        SX1272LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
        SX1272WriteBuffer( REG_LR_DIOMAPPING1, &SX1272LR->RegDioMapping1, 2 );
            
        SX1272LoRaSetOpMode( RFLR_OPMODE_CAD );
        RFLRState = RFLR_STATE_CAD_RUNNING;
        break;
    case RFLR_STATE_CAD_RUNNING:
		if (DIO0 ==1)
        //AA if( DIO3 == 1 ) //CAD Done interrupt
        { 
            // Clear Irq
            SX1272Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDONE  );
		if (DIO0 ==1)
		//AA   if( DIO4 == 1 ) // CAD Detected interrupt
            {
                // Clear Irq
                SX1272Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDETECTED  );
                // CAD detected, we have a LoRa preamble
                RFLRState = RFLR_STATE_RX_INIT;
                result = RF_CHANNEL_ACTIVITY_DETECTED;
            } 
            else
            {    
                // The device goes in Standby Mode automatically    
                RFLRState = RFLR_STATE_IDLE;
                result = RF_CHANNEL_EMPTY;
            }
        }   
        break;
    
    default:
        break;
    } 
#endif	
    return result;
}
#endif

 
