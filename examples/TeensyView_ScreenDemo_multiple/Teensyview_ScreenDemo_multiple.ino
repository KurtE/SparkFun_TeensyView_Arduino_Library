/******************************************************************************
TeensyView_Demo.ino
SFE_TeensyView Library Demo
Jim Lindblom @ SparkFun Electronics
Original Creation Date: October 27, 2014
Modified Febuary 2, 2017
This sketch uses the TeensyView library to draw a 3-D projected
cube, and rotate it along all three axes.
Development environment specifics:
Arduino IDE 1.6.12 w/ Teensyduino 1.31
Arduino IDE 1.8.1 w/ Teensyduino 1.35
TeensyView v1.0
This code is beerware; if you see me (or any other SparkFun employee) at thewa
local, and you've found our code helpful, please buy us a round!
Distributed as-is; no warranty is given.
******************************************************************************/
#include <SPI.h>
#include <TeensyView.h> // Include the SFE_TeensyView library
// /////////////////////////////////
// TeensyView Object Declaration //
// /////////////////////////////////
// #define TLC_SPI1
// #define T36_SPI1
// #define T36_SPI2
// #define DEFAULT_PINS
#ifdef DEFAULT_PINS
#define PIN_RESET 15
#define PIN_DC 5
#define PIN_CS 10
#define PIN_SCK 13
#define PIN_MOSI 11
#endif

#ifdef TLC_SPI1
#define PIN_RESET 15
#define PIN_SCK 20
#define PIN_MOSI 21
#define PIN_DC 4
#define PIN_CS 3
#endif

#ifdef T36_SPI1
#define PIN_RESET 15
#define PIN_SCK 20
#define PIN_MOSI 21
#define PIN_DC 31
#define PIN_CS 32
#endif

#ifdef T36_SPI2
#define PIN_RESET 15
#define PIN_SCK 53
#define PIN_MOSI 52
#define PIN_DC 55
#define PIN_CS 51
#endif

// Kurt's setup

#ifndef PIN_SCK

#if defined(KINETISK)
#define PIN_RESET 15
#define PIN_SCK 13
#define PIN_MOSI 11
#define PIN_DC 21
#define PIN_CS 20
// Setup 2nd one SPI1
#define PIN_RESET1 16
#define PIN_SCK1 32
#define PIN_MOSI1 0
#define PIN_DC1 31
#define PIN_CS1 30
#define SPI1_SPI SPI1

#elif defined(KINETISL)  // Teensy LC
// For multiple need to change CS to not 20 as only valid sck1 on lc
#define PIN_RESET 15
#define PIN_SCK 13
#define PIN_MOSI 11
#define PIN_DC 21
#define PIN_CS 22
// #undef SPI_INTERFACES_COUNT
// #define SPI_INTERFACES_COUNT 1
#define PIN_RESET1 16
#define PIN_SCK1 20
#define PIN_MOSI1 0
#define PIN_DC1 6
#define PIN_CS1 7
#define SPI1_SPI SPI1
#elif defined(__OPENCR__)
#define PIN_RESET 8
#define PIN_SCK 13
#define PIN_MOSI 11
#define PIN_DC 9
#define PIN_CS 10
// Setup 2nd one SPI1
#define PIN_RESET1 55
#define PIN_SCK1 58
#define PIN_MOSI1 60
#define PIN_DC1 56
#define PIN_CS1 57
#define SPI1_SPI SPI_EXT

#undef SPI_INTERFACES_COUNT
#define SPI_INTERFACES_COUNT 2  // only do 1 to start testing

#else // AVR... T2 SS(0), SCK(1), MOSI(2), MISO(3)
// For T2.0
#define PIN_RESET 5
#define PIN_SCK 1
#define PIN_MOSI 2
#define PIN_DC 4
#define PIN_CS 0
#endif

// Pins on connector on Beta T3.6 board (3.3, GND)(48, 47)(57 56) (51 52) (53 55)
#define PIN_RESET2 48
// #define PIN_MISO2 51
#define PIN_MOSI2 52
#define PIN_SCK2 53
#define PIN_DC2 55
#define PIN_CS2 56
#endif

TeensyView32 oled(PIN_RESET, PIN_DC, PIN_CS, PIN_SCK, PIN_MOSI);

#if SPI_INTERFACES_COUNT > 1
TeensyView32 oled1(PIN_RESET1, PIN_DC1, PIN_CS1, PIN_SCK1, PIN_MOSI1, &SPI1_SPI);
    #if SPI_INTERFACES_COUNT > 2
TeensyView32 oled2(PIN_RESET2, PIN_DC2, PIN_CS2, PIN_SCK2, PIN_MOSI2);
    #endif

#endif

#ifdef SPI_HAS_TRANSFER_ASYNC
#define DO_ASYNC true
#define DISPLAYFUNC displayAsync
#else
#define DO_ASYNC false
#define DISPLAYFUNC display
#endif


TeensyView * oleds[] =
{
#if 0
    & oled,
    & oled1
#else  
    & oled

#if SPI_INTERFACES_COUNT > 1
    , & oled1

    #if SPI_INTERFACES_COUNT > 2
    , & oled2
    #endif
#endif
#endif

};

uint8_t oled_which_test[] =
{
    0, 0, 0
};

uint16_t test_iterations_left[] =
{
    0xffff, 0xffff, 0xffff
};

uint32_t last_test_start_time[] =
{
    0, 0, 0
};

uint32_t next_test_start_time[] =
{
    0, 0, 0
};

extern void testRects(TeensyView * _oled, bool draw_async, uint16_t & iterations_left);
extern void testCircles(TeensyView * _oled, bool draw_async, uint16_t & iterations_left);
extern void TestpixelsAsync(TeensyView * _oled, uint16_t & iterations_left);
extern void TestFillRects(TeensyView * _oled, uint16_t & iterations_left);
extern void testdrawline(TeensyView * _oled, uint16_t & iterations_left, uint32_t & next_test_start_time);
extern void testscrolltext(TeensyView * _oled, uint16_t & iterations_left, uint32_t & next_test_start_time);
void setup()
{
//    pinMode(2, OUTPUT);
//    digitalWrite(2, LOW);
//    pinMode(3, OUTPUT);
//    digitalWrite(3, LOW);
    while (!Serial && millis() < 3000);
    Serial.begin(38400);

    Serial.println(F_CPU, DEC);
    Serial.printf("OlED R:%d D:%d C:%d S:%d M:%d\n", PIN_RESET, PIN_DC, PIN_CS, PIN_SCK, PIN_MOSI);
    // If other CS pins for displays set them up initially as not active...
#ifdef PIN_CS1
    pinMode(PIN_CS1, OUTPUT);
    digitalWrite(PIN_CS1, OUTPUT);
#endif            
#ifdef PIN_CS2
    pinMode(PIN_CS2, OUTPUT);
    digitalWrite(PIN_CS2, OUTPUT);
#endif            
    oled.begin(); // Initialize the OLED
    oled.display(); // Display what's in the buffer (splashscreen)

#if SPI_INTERFACES_COUNT > 1
    Serial.printf("OlED1 R:%d D:%d C:%d S:%d M:%d\n", PIN_RESET1, PIN_DC1, PIN_CS1, PIN_SCK1, PIN_MOSI1);
    oled1.begin(); // Initialize the OLED
    oled1.display(); // Display what's in the buffer (splashscreen)''
    Serial.println("oled1 displayed");
#endif

#if SPI_INTERFACES_COUNT > 2
    oled2.begin(); // Initialize the OLED
    oled2.display(); // Display what's in the buffer (splashscreen)''
    Serial.println("oled2 displayed");
#endif

    delay(1000); // Delay 1000 ms
    oled.clear(HARDWARE_MEM); // Clear the buffer.

#if SPI_INTERFACES_COUNT > 1
    oled1.clear(HARDWARE_MEM); // Clear the buffer.
#endif

#if SPI_INTERFACES_COUNT > 2
    oled2.clear(HARDWARE_MEM);
#endif
//  Serial.printf("Hit any key to continue");
//  while (Serial.read() == -1) ;
//  while (Serial.read() != -1) ;
#ifdef A1
    randomSeed(analogRead(A0) + analogRead(A1));
#else
   randomSeed(analogRead(A0) + (analogRead(A0)<<1));
#endif    
}

void loop()
{
    // Lets see which of our displays is ready to display something different
    for (uint8_t i = 0; i < sizeof(oleds) /sizeof(oleds[0]); i++)
    {
#ifdef SPI_HAS_TRANSFER_ASYNC
        if ((millis() > next_test_start_time[i]) && !oleds[i] -> displayAsyncActive())
#else
        if ((millis() > next_test_start_time[i]))
#endif
        {
            last_test_start_time[i] = millis();
            switch (oled_which_test[i])
            {
                case 0:
                    testRects(oleds[i], DO_ASYNC, test_iterations_left[i]);
                    break;
                case 1:
                    testRects(oleds[i], DO_ASYNC, test_iterations_left[i]);
                    break;
                case 2:
                    TestpixelsAsync(oleds[i], test_iterations_left[i]);
                    break;
                case 3:
                    TestFillRects(oleds[i], test_iterations_left[i]);
                    break;
                case 4:
                    testdrawline(oleds[i], test_iterations_left[i], next_test_start_time[i]);
                    break;
                case 5:
                    testscrolltext(oleds[i], test_iterations_left[i], next_test_start_time[i]);
                    break;
            }
            if (test_iterations_left[i] == 0)
            {
                oled_which_test[i] ++;
                if (oled_which_test[i] > 5)
                    oled_which_test[i] = 0;
                test_iterations_left[i] = 0xffff; // mark it special for first call
                next_test_start_time[i] = millis() + 100;
            }
        }
        else
            if ((millis() - last_test_start_time[i]) > 2500)
            {
                Serial.printf("Oled %d hung test: %d iter: %d\n ", i, oled_which_test[i], test_iterations_left[i]);
                printDebugInfo(oleds[i]);
                last_test_start_time[i] = millis();
            }
    }
}

void printDebugInfo(TeensyView * _oled)
{

#ifdef KINETISK

  #ifdef SPI_DEBUG_ASYNC_T3X
    extern void dumpDMA_TCD(const char * psz, DMABaseClass * dmabc);
    dumpDMA_TCD("TX:", _oled -> _spi -> _dmaTX);
    dumpDMA_TCD("RX:", _oled -> _spi -> _dmaRX);
  #endif  
#else

        #ifdef SPI_DEBUG_ASYNC_LC
    extern void dumpDMA_CFG(const char * sz, DMABaseClass * dmabc);
    dumpDMA_CFG("TX:", _oled -> _spi -> _dmaTX);
    dumpDMA_CFG("RX:", _oled -> _spi -> _dmaRX);
        #endif

#endif

}

void testRects(TeensyView * _oled, bool draw_async, uint16_t & iterations_left)
{
    int n, i, i2;
    int cx = _oled -> getLCDWidth() / 2;
    int cy = _oled -> getLCDHeight() / 2;
    _oled -> clear(PAGE);
    n = min(_oled -> getLCDWidth(), _oled -> getLCDHeight());
    for (i = 2; i < n; i += 6)
    {
        i2 = i / 2;
        _oled -> rect(cx - i2, cy - i2, i, i);
    }
    if (draw_async)
    {
        _oled -> DISPLAYFUNC();
    }
    else
    {
        _oled -> display();
    }
    iterations_left = 0;
}

void testCircles(TeensyView * _oled, bool draw_async, uint16_t & iterations_left)
{
    uint16_t radius = 10;
    int x, y, r2 = radius * 2,
    w = _oled -> getLCDWidth() + radius,
    h = _oled -> getLCDHeight() + radius;
    _oled -> clear(PAGE);
    for (x = 0; x < w; x += r2)
    {
        for (y = 0; y < h; y += r2)
        {
            _oled -> circle(x, y, radius);
        }
    }
    if (draw_async)
    {
        _oled -> DISPLAYFUNC();
    }
    else
    {
        _oled -> display();
    }
    iterations_left = 0;
}

void TestpixelsAsync(TeensyView * _oled, uint16_t & iterations_left)
{
    if (iterations_left == 0xffff)
    {
        _oled -> clear(PAGE);
        iterations_left = 1024;
    }
    _oled -> pixel(random(_oled -> getLCDWidth()), random(_oled -> getLCDHeight()));
    _oled -> DISPLAYFUNC();
    iterations_left--;
}

void TestFillRects(TeensyView * _oled, uint16_t & iterations_left)
{
    if (iterations_left == 0xffff)
    {
        _oled -> clear(PAGE);
        iterations_left = 0; // Not really, but makes it simple as we will update
    }
    _oled -> rectFill(iterations_left, iterations_left,
    _oled -> getLCDWidth() - iterations_left * 2, _oled -> getLCDHeight() - iterations_left * 2,(iterations_left & 1)? 0:1, NORM);
    _oled -> DISPLAYFUNC();
    iterations_left += 3;
    if (iterations_left >= _oled -> getLCDHeight() /2)
    {
        iterations_left = 0; // we are done.
    }
}

void testdrawline(TeensyView * _oled, uint16_t & iterations_left, uint32_t & next_test_start_time)
{
    // Serial.printf("testDrawline %x %x\n", _oled, iterations_left);
    if (iterations_left == 0xffff)
    {
        _oled -> clear(PAGE);
        iterations_left = 0; // Not really, but makes it simple as we will update
    }
    uint8_t line_test = iterations_left >> 8;
    uint8_t i = iterations_left & 0xff;
    switch (line_test)
    {
        case 0:
            _oled -> line(0, 0, i, _oled -> getLCDHeight() - 1);
            _oled -> DISPLAYFUNC();
            i += 4;
            if (i >= _oled -> getLCDWidth())
            {
                i = 0;
                line_test++;
            }
            break;
        case 1:
            _oled -> line(0, 0, _oled -> getLCDWidth() - 1, i);
            _oled -> DISPLAYFUNC();
            i += 4;
            if (i >= _oled -> getLCDHeight())
            {
                i = 0;
                line_test++;
                next_test_start_time = millis() + 250;
            }
        case 2:
            if (i == 0)
            {
                _oled -> clear(PAGE);
            }
            _oled -> line(0, _oled -> getLCDHeight() - 1, i, 0);
            _oled -> DISPLAYFUNC();
            i += 4;
            if (i >= _oled -> getLCDWidth())
            {
                i = 0;
                line_test++;
            }
            break;
        case 3:
            _oled -> line(0, _oled -> getLCDHeight() - 1, _oled -> getLCDWidth() - 1, i);
            _oled -> DISPLAYFUNC();
            i += 4;
            if (i >= _oled -> getLCDHeight())
            {
                i = 0;
                line_test++;
                next_test_start_time = millis() + 250;
            }
            break;
        case 4:
            if (i == 0)
            {
                _oled -> clear(PAGE);
            }
            _oled -> line(_oled -> getLCDWidth() - 1, _oled -> getLCDHeight() - 1, i, 0);
            _oled -> DISPLAYFUNC();
            i += 4;
            if (i >= _oled -> getLCDWidth())
            {
                i = 0;
                line_test++;
            }
            break;
        case 5:
            _oled -> line(_oled -> getLCDWidth() - 1, _oled -> getLCDHeight() - 1, 0, i);
            _oled -> DISPLAYFUNC();
            i += 4;
            if (i >= _oled -> getLCDHeight())
            {
                i = 0;
                line_test++;
                next_test_start_time = millis() + 250;
            }
            break;
        case 6:
            if (i == 0)
            {
                _oled -> clear(PAGE);
            }
            _oled -> line(_oled -> getLCDWidth() - 1, 0, 0, i);
            _oled -> DISPLAYFUNC();
            i += 4;
            if (i >= _oled -> getLCDHeight())
            {
                i = 0;
                line_test++;
            }
            break;
        case 7:
            _oled -> line(_oled -> getLCDWidth() - 1, 0, i, _oled -> getLCDHeight() - 1);
            _oled -> DISPLAYFUNC();
            i += 4;
            if (i >= _oled -> getLCDWidth())
            {
                i = 0;
                line_test = 0; // Say we are done
            }
    }
    iterations_left = (line_test << 8) | i;
}

void testscrolltext(TeensyView * _oled, uint16_t & iterations_left, uint32_t & next_test_start_time)
{
    if (iterations_left == 0xffff)
    {
        int middleX = _oled -> getLCDWidth() / 2;
        int middleY = _oled -> getLCDHeight() / 2;
        _oled -> clear(PAGE);
        _oled -> setFontType(1);
        // Try to set the cursor in the middle of the screen
        _oled -> setCursor(middleX - (_oled -> getFontWidth() * (6 / 2)),
        middleY - (_oled -> getFontWidth() / 2));
        // Print the title:
        _oled -> print("Scroll");
        _oled -> DISPLAYFUNC();
        iterations_left = 1; // Not really, but makes it simple as we will update
        return;
    }
    switch (iterations_left)
    {
        case 1:
            _oled -> scrollRight(0x00, 0x0f, true);
            next_test_start_time = millis() + 2000;
            break;
        case 2:
            _oled -> scrollStop();
            next_test_start_time = millis() + 1000;
            break;
        case 3:
            _oled -> scrollLeft(0x00, 0x0f, true);
            next_test_start_time = millis() + 2000;
            break;
        case 4:
            _oled -> scrollStop();
            next_test_start_time = millis() + 1000;
            break;
        case 5:
            _oled -> scrollVertRight(0x00, 0x07, true);
            next_test_start_time = millis() + 2000;
            break;
        case 6:
            _oled -> scrollVertLeft(0x00, 0x07, true);
            next_test_start_time = millis() + 2000;
            break;
        case 7:
            _oled -> scrollStop();
            next_test_start_time = millis() + 50;
            break;
    }
    if (++iterations_left > 7)
        iterations_left = 0;
}
