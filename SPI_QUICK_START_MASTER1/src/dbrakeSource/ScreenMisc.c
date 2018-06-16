#include <asf.h>
//#include "samd20.h"
#include "dbrakeDefs.h"
#include "driverLCD.h"

const uint8_t ScreenMisc[1024]=
{
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x48,0x48,0x48,0x30,0x00,0x3F,0x48,0x48,
0x48,0x3F,0x00,0x41,0x41,0x7F,0x41,0x41,0x00,0x7F,0x4C,0x4A,0x4B,0x31,0x00,0x00,
0x00,0x33,0x33,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x60,0x38,0x60,0x7F,
0x00,0x7F,0x48,0x48,0x48,0x40,0x00,0x7F,0x4C,0x4A,0x4B,0x31,0x00,0x3E,0x41,0x41,
0x41,0x22,0x00,0x7F,0x49,0x49,0x49,0x41,0x00,0x00,0x33,0x33,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x7F,0x40,0x40,0x00,0x7F,0x49,0x49,
0x49,0x41,0x00,0x7F,0x60,0x30,0x60,0x7F,0x00,0x7F,0x48,0x48,0x48,0x30,0x00,0x00,
0x00,0x33,0x33,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x48,0x48,0x48,0x3F,
0x00,0x3F,0x49,0x49,0x49,0x36,0x00,0x7F,0x4C,0x4A,0x4B,0x31,0x00,0x7F,0x08,0x1C,
0x77,0x41,0x00,0x7F,0x49,0x49,0x41,0x41,0x00,0x00,0x33,0x33,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x49,0x49,0x49,0x36,0x00,0x7F,0x01,0x01,
0x01,0x01,0x00,0x40,0x40,0x7F,0x40,0x40,0x00,0x7F,0x49,0x49,0x49,0x41,0x00,0x00,
0x00,0x33,0x33,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x49,0x49,0x49,0x41,
0x00,0x40,0x40,0x7F,0x40,0x40,0x00,0x7F,0x48,0x48,0x48,0x30,0x00,0x7F,0x60,0x30,
0x60,0x7F,0x00,0x31,0x49,0x49,0x49,0x46,0x00,0x00,0x33,0x33,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x31,0x49,0x49,0x49,0x46,0x00,0x3E,0x41,0x41,
0x41,0x41,0x00,0x7F,0x01,0x01,0x01,0x01,0x00,0x7F,0x48,0x4C,0x4A,0x31,0x00,0x01,
0x33,0x33,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x31,0x49,0x49,0x49,0x46,0x00,0x7F,0x48,0x48,
0x48,0x30,0x00,0x7F,0x08,0x1C,0x77,0x41,0x00,0x7F,0x49,0x49,0x41,0x41,0x00,0x7F,
0x48,0x4C,0x4A,0x31,0x00,0x00,0x33,0x33,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,

0xFF,0x00,0x10,0x30,0x70,0x7F,0x7F,0x7F,0x70,0x30,0x10,0x00,0xFF,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x7F,0x08,0x08,0x08,0x7F,0x00,0x3E,0x41,0x41,
0x41,0x3E,0x00,0x7F,0x20,0x38,0x20,0x7F,0x00,0x7F,0x49,0x49,0x41,0x41,0x00,0x03,
0x06,0x0C,0x38,0x70,0x00,0x7F,0x4C,0x4E,0x4A,0x7B,0x00,0x3E,0x41,0x41,0x41,0x3E,
0x00,0x40,0x40,0x7F,0x40,0x40,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0xFF,0x00,0x1D,0x15,0x17,0x00,0x1F,0x15,0x11,0x00,0x1F,0x01,0x01,0x00,0xFF
	
	
};
