//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//                FILE: appProtocol
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
#include "appProtocol.h"
#include "sx1272-fsk.h"
#include "config.h"
#include "appAccel.h"
#include "driverTSPI.h"
#include "radio.h"
#include "sx1272-LoRa.h"
#include "appLCD.h"
#include "appLCDConfigMisc.h"
#include "driverI2C.h"
#include "driverButtons.h"
#include "appmotor.h"
#include "appBluetooth.h"

 
//---------------------GLOBAL VARIABLES-----------------------------------
#define TEMP_APP_ADDRESS 0x001C000

 


//---------------------LOCAL VARIABLES------------------------------------
//--------------------------
//button check histories

 

//---------------------LOCAL FUNCTION PROTOTYPES--------------------------

 

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ---------------------------GLOBAL FUNCTIONS ----------------------------------
 

 
 
 
/**
 * \brief Function for programming data to Flash
 *
 * This function will check whether the data is greater than Flash page size.
 * If it is greater, it splits and writes pagewise.
 *
 * \param address address of the Flash page to be programmed
 * \param buffer  pointer to the buffer containing data to be programmed
 * \param len     length of the data to be programmed to Flash
 */
static void program_memory(uint32_t address, uint8_t *buffer, uint16_t len)
{
	/* Check if length is greater than Flash page size */
	if (len > NVMCTRL_PAGE_SIZE) {
		uint32_t offset = 0;

		while (len > NVMCTRL_PAGE_SIZE) {
			/* Check if it is first page of a row */
			if ((address & 0xFF) == 0) {
				/* Erase row */
				nvm_erase_row(address);
			}
			/* Write one page data to flash */
			nvm_write_buffer(address, buffer + offset, NVMCTRL_PAGE_SIZE);
			/* Increment the address to be programmed */
			address += NVMCTRL_PAGE_SIZE;
			/* Increment the offset of the buffer containing data */
			offset += NVMCTRL_PAGE_SIZE;
			/* Decrement the length */
			len -= NVMCTRL_PAGE_SIZE;
		}

		/* Check if there is data remaining to be programmed */
		if (len > 0) {
			/* Write the data to flash */
			nvm_write_buffer(address, buffer + offset, len);
		}
	} else {
		/* Check if it is first page of a row) */
		if ((address & 0xFF) == 0) {
			/* Erase row */
			nvm_erase_row(address);
		}
		/* Write the data to flash */
		nvm_write_buffer(address, buffer, len);
	}
}

 
 

 
int main2(void)
{
	uint32_t len;
	uint32_t curr_prog_addr;
	uint32_t tmp_len;
	uint8_t buff[NVMCTRL_PAGE_SIZE];
	struct nvm_config config;

	/* Check switch state to enter boot mode or application mode */
	uint32_t app_check_address;
	uint32_t *app_check_address_ptr;

	/* Check if WDT is locked */
	if (WDT->CTRL.reg & WDT_CTRL_ALWAYSON) {
		/* Watchdog always enabled, unsafe to program */
		while (1);
	}

 
 
		app_check_address = TEMP_APP_ADDRESS;
		app_check_address_ptr = (uint32_t *) app_check_address;

		/*
		 * Read the first location of application section
		 * which contains the address of stack pointer.
		 * If it is 0xFFFFFFFF then the application section is empty.
		 */
		if (*app_check_address_ptr == 0xFFFFFFFF) {
			while (1) {
				/* Wait indefinitely */
			}
		}
		/* Pointer to the Application Section */
		void (*application_code_entry)(void);

		/* Rebase the Stack Pointer */
		__set_MSP(*(uint32_t *) APP_START_ADDRESS);

		/* Rebase the vector table base address */
		SCB->VTOR = ((uint32_t) APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);

		/* Load the Reset Handler address of the application */
		application_code_entry = (void (*)(void))(unsigned *)(*(unsigned *)
				(APP_START_ADDRESS + 4));

		/* Jump to user Reset Handler in the application */
		application_code_entry();
	 
	/*
	 * Application to be programmed from APP_START_ADDRESS defined in
	 * conf_bootloader.h
	 */
	curr_prog_addr = APP_START_ADDRESS;

	/* Initialize system */
	system_init();


	/* Configure the SPI slave module */
//	configure_spi();

	/* Get NVM default configuration and load the same */
	nvm_get_config_defaults(&config);
	nvm_set_config(&config);

	/* Turn on LED */
//	port_pin_set_output_level(BOOT_LED, false);
	/* Get the length to be programmed */
//	len = get_length();

while (1)
{
	
}
	do {
		/* Get remaining or NVMCTRL_PAGE_SIZE as block length */
		tmp_len = min(NVMCTRL_PAGE_SIZE, len);

		/* Acknowledge last received data */
//		send_ack();

		/* Read data from SPI master */
//		fetch_data(buff, tmp_len);

		/* Program the data into Flash */
		program_memory(curr_prog_addr, buff, tmp_len);

		/* Increment the current programming address */
		curr_prog_addr += tmp_len;

		/* Update the length to remaining length to be programmed */
		len -= tmp_len;

		/* Do this for entire length */
	} while (len != 0);

	/* Acknowledge last block */
//	send_ack();

	/* Reset module and boot into application */
	NVIC_SystemReset();

	while (1) {
		/* Inf loop */
	}

}
