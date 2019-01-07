/****************************************************************************** 
TeensyView.h
Header file for the TeensyView Library

Marshall Taylor @ SparkFun Electronics, December 6, 2016
https://github.com/sparkfun/SparkFun_TeensyView_Arduino_Library

This has been modified from the SFE_MicroOLED library, Original contributors:

Jim Lindblom @ SparkFun Electronics, October 26, 2014
https://github.com/sparkfun/Micro_OLED_Breakout/tree/master/Firmware/Arduino/libraries/SFE_MicroOLED

Emil Varughese @ Edwin Robotics Pvt. Ltd. July 27, 2015
https://github.com/emil01/SparkFun_Micro_OLED_Arduino_Library/

GeekAmmo, (https://github.com/geekammo/MicroView-Arduino-Library)

Released under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

*****This file defines the hardware interface(s) for the TeensyView.*****

Development environment specifics:
Arduino IDE 1.6.12 w/ Teensyduino 1.31
Arduino IDE 1.8.1 w/ Teensyduino 1.35
TeensyView v1.0

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef TEENSYVIEW_H
#define TEENSYVIEW_H

#include <stdio.h>

#ifdef __cplusplus
#include "Arduino.h"
#include <SPI.h>
#if defined(KINETISK) || defined(KINESISL)
#include <DMAChannel.h>
#endif

#ifndef SPI_INTERFACES_COUNT
#if defined(__MKL26Z64__)
#define SPI_INTERFACES_COUNT 2
#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define SPI_INTERFACES_COUNT 3
#else
#define SPI_INTERFACES_COUNT 1
#endif
#endif

#endif

#define swap(a, b) { uint8_t t = a; a = b; b = t; }

#define BLACK 0
#define WHITE 1

// Define this if you wish to support the larger displays
#define LCDMAXHEIGHT		64
#define TEENSYVIEW_USE_MALLOC		// Define this if you wish for memory to be allocated by malloc to size of display

#define LCDWIDTH			128
#define LCDHEIGHT			32
#ifndef LCDMAXHEIGHT
#define LCDMAXHEIGHT		LCDHEIGHT
#endif

#define LCDMAXMEMORYSIZE	( LCDWIDTH * LCDMAXHEIGHT / 8 )
//#define LCDMEMORYSIZE		( LCDWIDTH * _height / 8 )

#define FONTHEADERSIZE		6
// The font header size is the first bytes of the static const unsigned char space that are not data.
// They are defined in the font files.
//   first row defines - FONTWIDTH, FONTHEIGHT, ASCII START CHAR, TOTAL CHARACTERS, FONT MAP WIDTH HIGH, FONT MAP WIDTH LOW (2,56 meaning 256)

#define NORM				0
#define XOR					1

#define PAGE				0x1
#define HARDWARE_MEM		0x02
#define ALL					0x3

#define SETCONTRAST 		0x81
#define DISPLAYALLONRESUME 	0xA4
#define DISPLAYALLON 		0xA5
#define NORMALDISPLAY 		0xA6
#define INVERTDISPLAY 		0xA7
#define DISPLAYOFF 			0xAE
#define DISPLAYON 			0xAF
#define SETDISPLAYOFFSET 	0xD3
#define SETCOMPINS 			0xDA
#define SETVCOMDESELECT		0xDB
#define SETDISPLAYCLOCKDIV 	0xD5
#define SETPRECHARGE 		0xD9
#define SETMULTIPLEX 		0xA8
#define SETLOWCOLUMN 		0x00
#define SETHIGHCOLUMN 		0x10
#define SETSTARTLINE 		0x40
#define MEMORYMODE 			0x20
#define COMSCANINC 			0xC0
#define COMSCANDEC 			0xC8
#define SEGREMAP 			0xA0
#define CHARGEPUMP 			0x8D
#define EXTERNALVCC 		0x01
#define SWITCHCAPVCC 		0x02
//////////////////////////////////
#define COLUMNADDR 			0x21
#define PAGEADDR   			0x22
#define DISPLAYALLON_RESUME 0xA4


// Scroll
#define ACTIVATESCROLL 					0x2F
#define DEACTIVATESCROLL 				0x2E
#define SETVERTICALSCROLLAREA 			0xA3
#define RIGHT_HORIZONTALSCROLL 			0x26
#define LEFT_HORIZONTALSCROLL 			0x27
#define VERTICALRIGHTHORIZONTALSCROLL	0x29
#define VERTICALLEFTHORIZONTALSCROLL	0x2A

typedef enum CMD {
	CMD_CLEAR,			//0
	CMD_INVERT,			//1
	CMD_CONTRAST,		//2
	CMD_DISPLAY,		//3
	CMD_SETCURSOR,		//4
	CMD_PIXEL,			//5
	CMD_LINE,			//6
	CMD_LINEH,			//7
	CMD_LINEV,			//8
	CMD_RECT,			//9
	CMD_RECTFILL,		//10
	CMD_CIRCLE,			//11
	CMD_CIRCLEFILL,		//12
	CMD_DRAWCHAR,		//13
	CMD_DRAWBITMAP,		//14
	CMD_GETLCDWIDTH,	//15
	CMD_GETLCDHEIGHT,	//16
	CMD_SETCOLOR,		//17
	CMD_SETDRAWMODE		//18
} commCommand_t;

class TeensyView : public Print{
public:
	// Constructor(s)
	TeensyView(uint8_t rst, uint8_t dc, uint8_t cs, uint8_t sck, uint8_t mosi, uint8_t height=32, SPIClass *spi=&SPI);
	
	void begin(void);
	void setClockRate( uint32_t );
	virtual size_t write(uint8_t);

	// RAW LCD functions
	void command(uint8_t c, boolean last=1);
	void data(uint8_t c, boolean last=0);
	void setColumnAddress(uint8_t add);
	void setPageAddress(uint8_t add);
	
	// LCD Draw functions
	void clear(uint8_t mode);
	void clear(uint8_t mode, uint8_t c);
	void invert(boolean inv);
	void contrast(uint8_t contrast);
	void display(void);

#ifdef SPI_HAS_TRANSFER_ASYNC
	EventResponder _event_responder; 
	static void asyncEventResponder(EventResponderRef event_responder);
	void displayAsyncCallBack(void);
	bool displayAsync(void);
	bool displayAsyncActive();
#endif

	bool outputCommandString(uint8_t count, bool do_async);

	// Kludge - static call backs for each SPI buss...
	void setCursor(uint8_t x, uint8_t y);
	void pixel(uint8_t x, uint8_t y);
	void pixel(uint8_t x, uint8_t y, uint8_t color, uint8_t mode);
	void line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
	void line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color, uint8_t mode);
	void lineH(uint8_t x, uint8_t y, uint8_t width);
	void lineH(uint8_t x, uint8_t y, uint8_t width, uint8_t color, uint8_t mode);
	void lineV(uint8_t x, uint8_t y, uint8_t height);
	void lineV(uint8_t x, uint8_t y, uint8_t height, uint8_t color, uint8_t mode);
	void rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
	void rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color , uint8_t mode);
	void rectFill(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
	void rectFill(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t color , uint8_t mode);
	void circle(uint8_t x, uint8_t y, uint8_t radius);
	void circle(uint8_t x, uint8_t y, uint8_t radius, uint8_t color, uint8_t mode);
	void circleFill(uint8_t x0, uint8_t y0, uint8_t radius);
	void circleFill(uint8_t x0, uint8_t y0, uint8_t radius, uint8_t color, uint8_t mode);
	void drawChar(uint8_t x, uint8_t y, uint8_t c);
	void drawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t color, uint8_t mode);
	void drawBitmap(const uint8_t * bitArray, uint16_t cbArray);
	uint8_t getLCDWidth(void);
	uint8_t getLCDHeight(void);
	void setColor(uint8_t color);
	void setDrawMode(uint8_t mode);
	uint8_t *getScreenBuffer(void);
	uint16_t getScreenBufferSize(void);

	// Font functions
	uint8_t getFontWidth(void);
	uint8_t getFontHeight(void);
	uint8_t getTotalFonts(void);
	uint8_t getFontType(void);
	uint8_t setFontType(uint8_t type);
	uint8_t getFontStartChar(void);
	uint8_t getFontTotalChar(void);

	// LCD Rotate Scroll functions	
	void scrollRight(uint8_t start, uint8_t stop, bool do_async = false);
	void scrollLeft(uint8_t start, uint8_t stop, bool do_async = false);
	void scrollVertRight(uint8_t start, uint8_t stop, bool do_async = false);
	void scrollVertLeft(uint8_t start, uint8_t stop, bool do_async = false);
	void scrollStop(void);
	void flipVertical(boolean flip);
	void flipHorizontal(boolean flip);
	
 	// Screen memory as part of class...
 	// Question should we fix size it or malloc it depending on size of 
 	SPIClass *_spi;		// Which spi buss to use. 
 	

protected: 	// Screen? ... 
#ifdef TEENSYVIEW_USE_MALLOC		// Define this if you wish for memory to be allocated by malloc to size of display
	uint8_t *screenmemory; 
#else 	
	uint8_t screenmemory [LCDMAXMEMORYSIZE]; 
#endif

private:
	uint8_t csPin, dcPin, rstPin, sckPin, mosiPin;
//	volatile uint8_t *wrport, *wrreg, *rdport, *rdreg;
//	uint8_t wrpinmask, rdpinmask;
//	volatile uint8_t *ssport, *dcport, *ssreg, *dcreg;	// use volatile because these are fixed location port address
//	uint8_t mosipinmask, sckpinmask, sspinmask, dcpinmask;
	uint8_t foreColor,drawMode,fontWidth, fontHeight, fontType, fontStartChar, fontTotalChar, cursorX, cursorY;
	uint16_t fontMapWidth;
	static const unsigned char *fontsPointer[];
	uint32_t clockRateSetting;
	
	// Communication (Defined in hardware.cpp)
	void spiSetup();

	// 
#if defined(__IMXRT1052__) || defined(__IMXRT1062__)
    volatile uint32_t *_csport, *_dcport;
    uint8_t _cspinmask, _dcpinmask;
#elif defined(__OPENCR__)	
#else
    volatile uint8_t *_csport, *_dcport;
    uint8_t _cspinmask, _dcpinmask;
#endif    
    uint8_t _height;

    // ASYNC support, need to save stuff to be used in callback
    volatile uint8_t _display_async_state; 	// What state we are in.  Probably even numbers output page description, Odd output page data.
	uint8_t _set_column_row_address[6];
	uint8_t _command_buffer[12];

 	//SPINClass *_pspin;
 	uint8_t	 _spi_bus;	// which bus are we on?
	uint16_t _screenmemory_size;
 	// Inline helper functions
	void beginSPITransaction() __attribute__((always_inline)) {
		_spi->beginTransaction(SPISettings(clockRateSetting, MSBFIRST, SPI_MODE0));
#ifdef __OPENCR__
		digitalWrite(csPin, LOW);
#else		
		if (_csport)
			*_csport  &= ~_cspinmask;
#endif
	}
	void endSPITransaction() __attribute__((always_inline)) {
#ifdef __OPENCR__
		digitalWrite(csPin, HIGH);
#else		
		if (_csport)
			*_csport |= _cspinmask;
#endif		
		_spi->endTransaction();
	}
	// Always use on TLC, only use on T3.x if DC pin is not on hardware CS pin
	volatile uint8_t _dcpinAsserted;
	void setCommandMode() __attribute__((always_inline)) {
		if (!_dcpinAsserted) {
#ifdef __OPENCR__
			digitalWrite(dcPin, LOW);
#else		
			*_dcport  &= ~_dcpinmask;
#endif
			_dcpinAsserted = 1;
		}
	}

	void setDataMode() __attribute__((always_inline)) {
		if (_dcpinAsserted) {
#ifdef __OPENCR__
			digitalWrite(dcPin, HIGH);
#else		
			*_dcport  |= _dcpinmask;
#endif			
			_dcpinAsserted = 0;
		}
	}


};

class TeensyView64 : public TeensyView {
public:
	TeensyView64 (uint8_t rst, uint8_t dc, uint8_t cs, uint8_t sck, uint8_t mosi, SPIClass *spi=&SPI)
			: TeensyView(rst, dc, cs, sck, mosi, 64, spi) {
		screenmemory = _screen_memory_64_line;

	}

protected:
	uint8_t _screen_memory_64_line[LCDWIDTH*64/8];

};

class TeensyView32 : public TeensyView {
public:
	TeensyView32 (uint8_t rst, uint8_t dc, uint8_t cs, uint8_t sck, uint8_t mosi, SPIClass *spi=&SPI)
			: TeensyView(rst, dc, cs, sck, mosi, 32, spi) {
		screenmemory = _screen_memory_32_line;

	}

protected:
	uint8_t _screen_memory_32_line[LCDWIDTH*32/8];

};
#endif
