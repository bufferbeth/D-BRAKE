//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                              driverXSPI.c
//================================================================================
// SPI - SERCOM0 on pins 
//		X-MISO	- PA07
//      X-NSS	- PA06
//		X-SCK	- PA05
//		X-MOSI	- PA04
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#include <asf.h>
#include "dbrakeDefs.h"
#include "driverASPI.h"

#if REMOTEBOARD
#define ASLAVE_SELECT_PIN PIN_PA10
 
#define MAX_SPIA_BUFFER_SIZE 15
uint8_t rxBuffA[MAX_SPIA_BUFFER_SIZE];
uint8_t txBuffA[MAX_SPIA_BUFFER_SIZE]; 

struct spi_module spia_master_instance;
struct spi_slave_inst slavea;
 
//----------------------------------------------------------------------------
//		SPIXConfigure
//---------------------------------------------------------------------------- 
void SPIAConfigure(void)
{
 
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;
 
	//-----------------------------------
	// Configure and initialize software device 
	// instance of peripheral slave 
	//-----------------------------------
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = ASLAVE_SELECT_PIN;
	spi_attach_slave(&slavea, &slave_dev_config);
	//------------------------------------
	// Configure, initialize and enable SERCOM SPI module 
	//------------------------------------
	spi_get_config_defaults(&config_spi_master);
	config_spi_master.mux_setting = EXT1_SPI_SERCOM_MUX_SETTING;

	/* Configure pad 0 for data in */
	config_spi_master.pinmux_pad0 = EXT1_SPI_SERCOM_PINMUX_PAD0;
	/* Configure pad 1 as unused */
	config_spi_master.pinmux_pad1 = EXT1_SPI_SERCOM_PINMUX_PAD1;
	/* Configure pad 2 for data out */
	config_spi_master.pinmux_pad2 = PINMUX_UNUSED;
	/* Configure pad 3 for SCK */
	config_spi_master.pinmux_pad3 = EXT1_SPI_SERCOM_PINMUX_PAD3;
	spi_init(&spia_master_instance, EXT1_SPI_MODULE, &config_spi_master);
	spi_enable(&spia_master_instance);
 
}

 //----------------------------------------------------------------------------
 //		SPIAMain
 //----------------------------------------------------------------------------

int SPIAMain(void)
{
	uint8_t tempBuffer[3]; 
	

	SPIAConfigure();
	//-------------------------
	// ask WHO AM I 
 	AppStatusUpdate(INTERFACE_ACCELEROMETER,STATUS_PARTTALKING,0);
	 	
	tempBuffer[0] = 0; 
	if (SPIAInOut(0x8f,tempBuffer,1)!= 0)
	{
		if (tempBuffer[0] == 0x33)	
		{
			AppStatusUpdate(INTERFACE_ACCELEROMETER,STATUS_PARTTALKING,1);			
		}
	}

	/* ------------  hard coded	
	spi_select_slave(&spia_master_instance, &slavea, true);
	txBuffA[0] = 0x8f;
	txBuffA[1] = 0x00;
	txBuffA[2] = 0x00;
	spi_transceive_buffer_wait(&spia_master_instance,&txBuffA[0], &rxBuffA[0],2);
	spi_select_slave(&spia_master_instance, &slavea, false);
	*/
	
	return 1; 
 
}

 //----------------------------------------------------------------------------
 //		SPIXInOUt
 //----------------------------------------------------------------------------
 int SPIAInOut(uint8_t addr,uint8_t *buffer,uint8_t size)
 {
	 uint8_t i,*ptr; 
	 ptr = buffer; 
	 int success;
	 
	 success = 0; 
	 
	 if (size < (MAX_SPIA_BUFFER_SIZE-1))
	 {
		 success = 1; 
		 spi_select_slave(&spia_master_instance, &slavea, true);
		 for (i=0;i<size;i++)
		 {
			txBuffA[i+1] = *ptr++;
		 }
		 txBuffA[0] = addr; 
		 spi_transceive_buffer_wait(&spia_master_instance,&txBuffA[0], &rxBuffA[0],size+1);
		 spi_select_slave(&spia_master_instance, &slavea, false);
		 
		 ptr = buffer; 
		 for (i=0;i<size;i++)
		 {
			*ptr++ = rxBuffA[i+1];
		 }		 
	 } 
	 return success; 
 }
#endif 



