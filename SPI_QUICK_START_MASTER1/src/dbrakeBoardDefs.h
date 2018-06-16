


#ifndef DBRAKE_BOARD_DEFS
#define DBRAKE_BOARD_DEFS

#include <asf.h>

//#define EXT1_SPI_MODULE              SERCOM0
//#define EXT1_SPI_SERCOM_MUX_SETTING  SPI_SIGNAL_MUX_SETTING_E
//#define EXT1_SPI_SERCOM_PINMUX_PAD0  PINMUX_PA04D_SERCOM0_PAD0
//#define EXT1_SPI_SERCOM_PINMUX_PAD1  PINMUX_PA05D_SERCOM0_PAD1
//#define EXT1_SPI_SERCOM_PINMUX_PAD2  PINMUX_PA06D_SERCOM0_PAD2
//#define EXT1_SPI_SERCOM_PINMUX_PAD3  PINMUX_PA07D_SERCOM0_PAD3

	//---------------------------------
	// LEDs on the board 
	// PA15 - RED LED
	// PB15 - Blue LED
	// PB4 - Bi-color Green LED
	// PB5 - Bi-color Red LED 
	//---------------------------------
#define LED_RED_PIN                  PIN_PA15
#define LED_RED_ACTIVE               false
#define LED_RED_INACTIVE             !LED_RED_ACTIVE

#define LED_BLUE_PIN                  PIN_PB15
#define LED_BLUE_ACTIVE               false
#define LED_BLUE_INACTIVE             !LED_BLUE_ACTIVE

#define LED_BIRED_PIN                  PIN_PB05
#define LED_BIRED_ACTIVE               false
#define LED_BIRED_INACTIVE             !LED_BIRED_ACTIVE

#define LED_BIGREEN_PIN                PIN_PB04
#define LED_BIGREEN_ACTIVE             false
#define LED_BIGREEN_INACTIVE           !LED_BIGREEN_ACTIVE



/** @} */

/** \name SW0 definitions
 *  @{ */
#define SW0_PIN                   PIN_PB13
#define SW0_ACTIVE                false
#define SW0_INACTIVE              !SW0_ACTIVE
#define SW0_EIC_PIN               PIN_PB13A_EIC_EXTINT13
#define SW0_EIC_MUX               MUX_PB13A_EIC_EXTINT13
#define SW0_EIC_PINMUX            PINMUX_PB13A_EIC_EXTINT13
#define SW0_EIC_LINE              13
/** @} */

 

/** Number of on-board LEDs */
#define LED_COUNT                 1

/**
 * \name Button #0 definitions
 *
 * Wrapper macros for SW0, to ensure common naming across all Xplained Pro
 * boards.
 *
 *  @{ */
#define BUTTON_0_NAME             "SW0"
#define BUTTON_0_PIN              SW0_PIN
#define BUTTON_0_ACTIVE           SW0_ACTIVE
#define BUTTON_0_INACTIVE         SW0_INACTIVE
#define BUTTON_0_EIC_PIN          SW0_EIC_PIN
#define BUTTON_0_EIC_MUX          SW0_EIC_MUX
#define BUTTON_0_EIC_PINMUX       SW0_EIC_PINMUX
#define BUTTON_0_EIC_LINE         SW0_EIC_LINE




#endif