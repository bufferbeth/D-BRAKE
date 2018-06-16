/*
 * GccApplication1.c
 *
 * Created: 7/26/2014 6:32:17 PM
 *  Author: ehorton
 */ 
#include <asf.h>
//#include "samd20.h"
#include "dbrakeControlBoard.h"

int LoarMain( void );
int SPImain(void);
int I2Cmain(void); 

int wdogTimer;

bool pinState; 
/** Updates the board LED to the current button state. */
static void update_led_state(void)
{
	bool pin_state = port_pin_get_input_level(BUTTON_0_PIN);
	pinState = pin_state;
	port_pin_set_output_level(LED_RED_PIN, pin_state);
}


/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);

	//--------------------------------
	// Set all LED pins as outputs
	// PA15, PB15, PB4, PB5
	// ** turn them off as initial setting
	//--------------------------------
  //----------PA15 RED LED
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_RED_PIN, &pin_conf);
	port_pin_set_output_level(LED_RED_PIN, LED_RED_INACTIVE);
  //----------PB15 BLUE LED
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_BLUE_PIN, &pin_conf);
	port_pin_set_output_level(LED_BLUE_PIN, LED_BLUE_INACTIVE);	
  //----------PB5 RED LED
    pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(LED_BIRED_PIN, &pin_conf);
    port_pin_set_output_level(LED_BIRED_PIN, LED_BIRED_INACTIVE);	
  //----------PB4 RED LED
    pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(LED_BIGREEN_PIN, &pin_conf);
    port_pin_set_output_level(LED_BIGREEN_PIN, LED_BIGREEN_INACTIVE);
  
 	port_pin_set_output_level(LED_RED_PIN, LED_RED_ACTIVE);	 
 	port_pin_set_output_level(LED_BLUE_PIN, LED_BLUE_INACTIVE);
 	port_pin_set_output_level(LED_BIRED_PIN, LED_BIRED_ACTIVE);
 	port_pin_set_output_level(LED_BIGREEN_PIN, LED_BIGREEN_ACTIVE); 
  
	/* Set buttons as inputs */
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	pin_conf.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(BUTTON_0_PIN, &pin_conf);	

  //----------PA08 SCLK
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA09, &pin_conf);
	port_pin_set_config(PIN_PA10, &pin_conf);
	port_pin_set_config(PIN_PA08, &pin_conf);
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	port_pin_set_config(PIN_PA11, &pin_conf);
	
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(PIN_PA08, &pin_conf);
	port_pin_set_config(PIN_PA06, &pin_conf);
	port_pin_set_config(PIN_PA07, &pin_conf);
	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	port_pin_set_config(PIN_PA05, &pin_conf);	

//----------------------
// LoRA radio
//-----------------------	
	//--------------------------------
	// X-RESET = output = PA3
	// X-NSSS = output = PA6
	// X-MOSI = output = PA4
	// X-MISO = input = PA7
	// X-SCK = output = PA5
	// X-DIO0 = input ? = PB9
	//--------------------------------
	//----------PA15 RED LED
	pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LORA_XRESET, &pin_conf);
	port_pin_set_output_level(LORA_XRESET, 1);	
	
	port_pin_set_config(LORA_XNSSS, &pin_conf);
	port_pin_set_config(LORA_XMOSI, &pin_conf);		
	port_pin_set_config(LORA_XSCK, &pin_conf);	

	pin_conf.direction  = PORT_PIN_DIR_INPUT;
	port_pin_set_config(LORA_XMISO, &pin_conf);
	port_pin_set_config(LORA_XDIO0, &pin_conf);

				
//	port_pin_set_output_level(PINLED_RED_PIN, LED_RED_INACTIVE);
	
    /* Initialize the SAM system */
    SystemInit();

	SPImain();
//	LoarMain();
		
//	I2Cmain();

	SPIXConfigure();
	SPIXMain();

    while (1) 
    {
        //TODO:: Please write your application code 
		
		update_led_state();
    }
}
