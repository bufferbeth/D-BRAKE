#if REMOTE_TIREON

#include <asf.h>
//#include "samd20.h"
#include "dbrakeDefs.h"
#include "driverLCD.h"

const uint8_t CabMenuDegreeC[1024]=
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x7F,0x70,0x60,
	0x60,0x60,0x60,0x00,0x7F,0x7F,0x60,0x60,0x60,0x7F,0x7F,0x00,0x1F,0x7F,0x61,0x61,
	0x61,0x7F,0x1F,0x00,0x3F,0x7F,0x70,0x60,0x60,0x60,0x60,0x00,0x7F,0x7F,0x03,0x03,
	0x03,0x7F,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF8,0x38,0x18,
	0x18,0x18,0x18,0x00,0xF8,0xF8,0x18,0x18,0x18,0xF8,0xF8,0x00,0xF9,0xF9,0x81,0x81,
	0x81,0xF9,0xF9,0x01,0xF0,0xF8,0x38,0x18,0x18,0x18,0x18,0x00,0xF8,0xF8,0x00,0x00,
	0x00,0xF8,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,
	0x14,0x1C,0x00,0x1F,0x11,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0F,0x70,0xE8,0xC8,0x08,0x08,0x08,
	0x08,0x08,0x08,0xC8,0xE8,0x70,0x0F,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x14,0x1C,0x00,0x1F,
	0x11,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,0x3F,0x40,0x40,0x40,0x40,0x40,
	0x40,0x40,0x40,0x40,0x40,0x3F,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x1F,0xE0,0x1F,0x1F,0x00,0x00,0x00,
	0x00,0x00,0x00,0x1F,0x1F,0xE0,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,
	0x0A,0x0E,0x00,0x0F,0x08,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x7C,0x82,0x02,0x02,0x02,0x02,0x02,
	0x02,0x02,0x02,0x02,0x02,0x82,0x7C,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x0A,0x0E,0x00,0x0F,
	0x08,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x0F,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x09,0x09,0x09,0x09,0x09,0x08,0x08,0x09,
	0x09,0x09,0x08,0x08,0x09,0x09,0x09,0x09,0x08,0x08,0x08,0x00,0x08,0x08,0x08,0x08,
	0x0F,0x00,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
	0x08,0x08,0x08,0x08,0x09,0x08,0x08,0x08,0x09,0x08,0x09,0x09,0x09,0x09,0x09,0x08,
	0x09,0x08,0x08,0x08,0x09,0x08,0x09,0x08,0x08,0x08,0x09,0x08,0x08,0x08,0x08,0x0F,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
	0x08,0x09,0x09,0x09,0x09,0x08,0x08,0x09,0x09,0x09,0x09,0x08,0x08,0x08,0x09,0x89,
	0x89,0x88,0x08,0x09,0x08,0x08,0x08,0x09,0x08,0x09,0x09,0x09,0x09,0x09,0x08,0x0F,

	0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x04,0x04,0x04,0x04,0x00,0xFC,0x10,
	0x10,0x10,0xFC,0x00,0xFC,0x20,0x30,0x28,0xC4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
	0x00,0x00,0x00,0x00,0xFC,0x80,0x60,0x80,0xFC,0x00,0xFC,0x24,0x24,0x24,0x04,0x00,
	0xFC,0xC0,0x30,0x08,0xFC,0x00,0xFC,0x04,0x04,0x04,0xFC,0x00,0x00,0x00,0x00,0xFF,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
	0x00,0xFC,0x24,0x24,0x24,0xD8,0x00,0xFC,0x20,0x30,0x28,0xC4,0x00,0xFC,0x10,0x10,
	0x10,0xFC,0x00,0xFC,0x20,0x50,0x88,0x04,0x00,0xFC,0x24,0x24,0x24,0x04,0x00,0xFF
};

const uint8_t CabMenuPSI[1024]=
{
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0x7F,0x70,0x60,
0x60,0x60,0x60,0x00,0x7F,0x7F,0x60,0x60,0x60,0x7F,0x7F,0x00,0x1F,0x7F,0x61,0x61,
0x61,0x7F,0x1F,0x00,0x3F,0x7F,0x70,0x60,0x60,0x60,0x60,0x00,0x7F,0x7F,0x03,0x03,
0x03,0x7F,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF8,0x38,0x18,
0x18,0x18,0x18,0x00,0xF8,0xF8,0x18,0x18,0x18,0xF8,0xF8,0x00,0xF9,0xF9,0x81,0x81,
0x81,0xF9,0xF9,0x01,0xF0,0xF8,0x38,0x18,0x18,0x18,0x18,0x00,0xF8,0xF8,0x00,0x00,
0x00,0xF8,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,
0x14,0x1C,0x00,0x1D,0x15,0x17,0x00,0x11,0x1F,0x11,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0F,0x70,0xE8,0xC8,0x08,0x08,0x08,
0x08,0x08,0x08,0xC8,0xE8,0x70,0x0F,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x14,0x1C,0x00,0x1D,
0x15,0x17,0x00,0x11,0x1F,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xC0,0x3F,0x40,0x40,0x40,0x40,0x40,
0x40,0x40,0x40,0x40,0x40,0x3F,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x1F,0xE0,0x1F,0x1F,0x00,0x00,0x00,
0x00,0x00,0x00,0x1F,0x1F,0xE0,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
0x0A,0x0E,0x00,0x0E,0x0A,0x0B,0x00,0x08,0x0F,0x08,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x7C,0x82,0x02,0x02,0x02,0x02,0x02,
0x02,0x02,0x02,0x02,0x02,0x82,0x7C,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x0A,0x0E,0x00,0x0E,
0x0A,0x0B,0x00,0x08,0x0F,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x0F,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x09,0x09,0x09,0x09,0x09,0x08,0x08,0x09,
0x09,0x09,0x08,0x08,0x09,0x09,0x09,0x09,0x08,0x08,0x08,0x00,0x08,0x08,0x08,0x88,
0x0F,0x00,0x00,0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x0F,
0x08,0x08,0x08,0x08,0x09,0x08,0x08,0x08,0x09,0x08,0x09,0x09,0x09,0x09,0x09,0x08,
0x09,0x08,0x08,0x08,0x09,0x08,0x09,0x08,0x08,0x08,0x09,0x08,0x08,0x08,0x08,0x0F,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,
0x08,0x09,0x09,0x09,0x09,0x08,0x08,0x09,0x09,0x09,0x09,0x88,0x08,0x08,0x09,0x89,
0x89,0x88,0x08,0x89,0x88,0x88,0x08,0x09,0x08,0x09,0x09,0x09,0x09,0x09,0x08,0x0F,

0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x04,0x04,0x04,0x04,0x00,0xFC,0x10,
0x10,0x10,0xFC,0x00,0xFC,0x20,0x30,0x28,0xC4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0x00,0x00,0x00,0x00,0xFC,0x80,0x60,0x80,0xFC,0x00,0xFC,0x24,0x24,0x24,0x04,0x00,
0xFC,0xC0,0x30,0x08,0xFC,0x00,0xFC,0x04,0x04,0x04,0xFC,0x00,0x00,0x00,0x00,0xFF,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,
0x00,0xFC,0x24,0x24,0x24,0xD8,0x00,0xFC,0x20,0x30,0x28,0xC4,0x00,0xFC,0x10,0x10,
0x10,0xFC,0x00,0xFC,0x20,0x50,0x88,0x04,0x00,0xFC,0x24,0x24,0x24,0x04,0x00,0xFF
};
#endif 