/**
 * \file
 *
 * \brief SAM D20/D21/R21 SPI Quick Start
 *
 * Copyright (C) 2012-2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#include <asf.h>
#include "DbrakeControlBoard.h"

#define ACCEL 0

int SPImain(void);

 
#define BUF_LENGTH 20
#if ACCEL
	#define SLAVE_SELECT_PIN PIN_PA10   //EXT1_PIN_SPI_SS_0
#else
#define SLAVE_SELECT_PIN PIN_PA06   //EXT1_PIN_SPI_SS_0
#endif
 
static const uint8_t buffer[BUF_LENGTH] = {
		0x8f, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
		 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13
};
 
struct spi_module spi_master_instance;
struct spi_slave_inst slave;
 

void configure_spi_master(void);

 
void configure_spi_master(void)
{
 
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;
 
	/* Configure and initialize software device instance of peripheral slave */
 
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = SLAVE_SELECT_PIN;
 
	spi_attach_slave(&slave, &slave_dev_config);
 
	/* Configure, initialize and enable SERCOM SPI module */
 
	spi_get_config_defaults(&config_spi_master);
#if ACCEL	
	config_spi_master.mux_setting = EXT1_SPI_SERCOM_MUX_SETTING;

	/* Configure pad 0 for data in */
	config_spi_master.pinmux_pad0 = EXT1_SPI_SERCOM_PINMUX_PAD0;
	/* Configure pad 1 as unused */
	config_spi_master.pinmux_pad1 = EXT1_SPI_SERCOM_PINMUX_PAD1;
	/* Configure pad 2 for data out */
	config_spi_master.pinmux_pad2 = PINMUX_UNUSED;
	/* Configure pad 3 for SCK */
	config_spi_master.pinmux_pad3 = EXT1_SPI_SERCOM_PINMUX_PAD3;
	spi_init(&spi_master_instance, EXT1_SPI_MODULE, &config_spi_master);	
#else
	config_spi_master.mux_setting = EXT2_SPI_SERCOM_MUX_SETTING;

	/* Configure pad 0 for data in */
	config_spi_master.pinmux_pad0 = EXT2_SPI_SERCOM_PINMUX_PAD0;
	/* Configure pad 1 as unused */
	config_spi_master.pinmux_pad1 = EXT2_SPI_SERCOM_PINMUX_PAD1;
	/* Configure pad 2 for data out */
	config_spi_master.pinmux_pad2 = PINMUX_UNUSED;
	/* Configure pad 3 for SCK */
	config_spi_master.pinmux_pad3 = EXT2_SPI_SERCOM_PINMUX_PAD3;
	spi_init(&spi_master_instance, EXT2_SPI_MODULE, &config_spi_master);

#endif



//! [enable]
	spi_enable(&spi_master_instance);
//! [enable]

}
 
uint8_t rxBuff[3];
uint8_t txBuff[3];
int SPImain(void)
{
 
	system_init();
 
	configure_spi_master();
 while (true)
 {
	spi_select_slave(&spi_master_instance, &slave, true);
	
//	spi_write(&spi_master_instance,0x5a);
//	spi_transceive_wait(&spi_master_instance,0x18f, &rxBuff);
#if ACCEL
	txBuff[0] = 0x8f;
#else
	txBuff[0] = 0x42;
#endif
	txBuff[1] = 0x00;
	txBuff[2] = 0x00;
	spi_transceive_buffer_wait(&spi_master_instance,&txBuff[0], &rxBuff[0],2);
	
 
//	spi_write_buffer_wait(&spi_master_instance, buffer, BUF_LENGTH);
 
	spi_select_slave(&spi_master_instance, &slave, false);
 }
	while (true) {
		/* Infinite loop */
	}
 
}
