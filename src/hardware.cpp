/****************************************************************************** 
hardware.cpp
TeensyView Arduino Library Hardware Interface

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

#include "TeensyView.h"
#include <SPI.h>

/** \brief Set Up SPI Interface

	Sets up the SPI pins, initializes the Arduino's SPI interface.
**/
void TeensyView::spiSetup()
{
	// Initialize the pins:
	// lets see if we have valid pins to use hardware support through spin. 
//	_pspin = NULL; 	// assume we will not use the hardware...

#if defined(KINETISK) || defined(KINETISL) 
	// lets try new functions in my SPI to validate the pins and if they 
	// are not appropriate for the _spi object then try to set to using 
	// BUGBUG We are not verifying that the Mosi and SCK pins are valid...
	if (!_spi->pinIsMOSI(mosiPin) || !_spi->pinIsSCK(sckPin)) {
		#if SPI_INTERFACES_COUNT > 1
		if (SPI1.pinIsMOSI(mosiPin) && SPI1.pinIsSCK(sckPin)) {
			_spi = &SPI1;
			_spi_bus = 1;
		}
		#if SPI_INTERFACES_COUNT > 2
		else if (SPI2.pinIsMOSI(mosiPin) && SPI2.pinIsSCK(sckPin)) {
			_spi = &SPI2;
			_spi_bus = 2;
		}
		#endif  // > 2
		else
		#endif
		{
			Serial.print("Not valid SPI pins");
		}

	}

	_spi->setMOSI(mosiPin);	// set the mosi pin. 
	_spi->setSCK(sckPin);

	_spi->begin();		// startup SPI
	pinMode(csPin, OUTPUT);
	_csport    = portOutputRegister(digitalPinToPort(csPin));
	_cspinmask = digitalPinToBitMask(csPin);
	*_csport |= _cspinmask;
	pinMode(dcPin, OUTPUT);
	_dcport    = portOutputRegister(digitalPinToPort(dcPin));
	_dcpinmask = digitalPinToBitMask(dcPin);
	*_dcport |= _dcpinmask;
	_dcpinAsserted = 0;
#else
	//Serial.println("Not Teensy 3.x or LC not using SPIN");
	pinMode(csPin, OUTPUT);
	_csport    = portOutputRegister(digitalPinToPort(csPin));
	_cspinmask = digitalPinToBitMask(csPin);
	*_csport |= _cspinmask;
	pinMode(dcPin, OUTPUT);
	_dcport    = portOutputRegister(digitalPinToPort(dcPin));
	_dcpinmask = digitalPinToBitMask(dcPin);
	*_dcport |= _dcpinmask;
	_dcpinAsserted = 0;
	_spi->begin();
#endif	
}

