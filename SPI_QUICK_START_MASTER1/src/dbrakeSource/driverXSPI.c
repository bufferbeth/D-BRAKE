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
#include "driverXSPI.h"

#define XSLAVE_SELECT_PIN PIN_PA06   
#define MAX_SPIX_BUFFER_SIZE 150
 
struct spi_module spix_master_instance;
struct spi_slave_inst slavex;
 
//----------------------------------------------------------------------------
//		SPIXConfigure
//---------------------------------------------------------------------------- 
void SPIXConfigure(void)
{
 
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;
 
	//-----------------------------------
	// Configure and initialize software device 
	// instance of peripheral slave 
	//-----------------------------------
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = XSLAVE_SELECT_PIN;
	spi_attach_slave(&slavex, &slave_dev_config);
	//------------------------------------
	// Configure, initialize and enable SERCOM SPI module 
	//------------------------------------
	spi_get_config_defaults(&config_spi_master);
	config_spi_master.mux_setting = EXT2_SPI_SERCOM_MUX_SETTING;

	/* Configure pad 0 for data in */
	config_spi_master.pinmux_pad0 = EXT2_SPI_SERCOM_PINMUX_PAD0;
	/* Configure pad 1 as unused */
	config_spi_master.pinmux_pad1 = EXT2_SPI_SERCOM_PINMUX_PAD1;
	/* Configure pad 2 for data out */
	config_spi_master.pinmux_pad2 = PINMUX_UNUSED;
	/* Configure pad 3 for SCK */
	config_spi_master.pinmux_pad3 = EXT2_SPI_SERCOM_PINMUX_PAD3;
	spi_init(&spix_master_instance, EXT2_SPI_MODULE, &config_spi_master);
	spi_enable(&spix_master_instance);
 
}

 //----------------------------------------------------------------------------
 //		SPIXMain
 //----------------------------------------------------------------------------
uint8_t rxBuffX[MAX_SPIX_BUFFER_SIZE];
uint8_t txBuffX[MAX_SPIX_BUFFER_SIZE];
int SPIXMain(void)
{
 
//	system_init();
 
//	SPIXConfigure();
// while (true)
// {
	spi_select_slave(&spix_master_instance, &slavex, true);
 
	txBuffX[0] = 0x42;
	txBuffX[1] = 0x00;
	txBuffX[2] = 0x00;
	spi_transceive_buffer_wait(&spix_master_instance,&txBuffX[0], &rxBuffX[0],2);
 
	spi_select_slave(&spix_master_instance, &slavex, false);
// }
//	while (true) {
//		/* Infinite loop */
//	}

	spi_select_slave(&spix_master_instance, &slavex, true);
 
	txBuffX[0] = 0x01;
	txBuffX[1] = 0x00;
	txBuffX[2] = 0x00;
	spi_transceive_buffer_wait(&spix_master_instance,&txBuffX[0], &rxBuffX[0],4);
 
	spi_select_slave(&spix_master_instance, &slavex, false);
	return 1; 
 
}

 //----------------------------------------------------------------------------
 //		SPIXInOUt
 //----------------------------------------------------------------------------
 int SPIXInOut(uint8_t addr,uint8_t *buffer,uint16_t size)
 {
	 uint16_t i;
	 uint8_t *ptr; 
	 ptr = buffer; 
	 int success;
	 
	 success = 0; 
	 
	 if (size < (MAX_SPIX_BUFFER_SIZE-1))
	 {
		 success = 1; 
		 spi_select_slave(&spix_master_instance, &slavex, true);
		 for (i=0;i<size;i++)
		 {
			txBuffX[i+1] = *ptr++;
		 }
		 
		 txBuffX[0] = addr; 
 		 spi_transceive_buffer_wait(&spix_master_instance,&txBuffX[0], &rxBuffX[0],size+1);
		 ptr = buffer; 
		 for (i=0;i<(size+1);i++)
		 {
			 *ptr++ = rxBuffX[i]; 
		 } 
		 spi_select_slave(&spix_master_instance, &slavex, false);
	 } 
	 return success; 
 }


 //----------------------------------------------------------------------------
 //		SPIXInOUt
 //----------------------------------------------------------------------------
 int SPIXInOut2(uint8_t addr,uint8_t *buffer,uint16_t size)
 {
	 uint16_t i;
	 uint8_t *ptr; 
	 ptr = buffer; 
	 int success;
	 
	 success = 0; 
	 
	 if (size < (MAX_SPIX_BUFFER_SIZE-1))
	 {
		 success = 1; 
		 spi_select_slave(&spix_master_instance, &slavex, true);
		 for (i=0;i<size;i++)
		 {
			txBuffX[i+1] = *ptr++;
		 }
		 ptr = buffer; 
		 txBuffX[0] = addr; 
		 if ((addr & 0x80)!= 0)
		 {
			 spi_transceive_buffer_wait(&spix_master_instance,&txBuffX[0], &rxBuffX[0],size+1);
		 }
		 else
		 {
			 spi_transceive_buffer_wait(&spix_master_instance,&txBuffX[0],&rxBuffX[0],size+1);
			 ptr = buffer; 
			 for (i=0;i<size;i++)
			 {
				*ptr++ = rxBuffX[i+1];
			 }
		 }
		 spi_select_slave(&spix_master_instance, &slavex, false);
	 } 
	 return success; 
 }


