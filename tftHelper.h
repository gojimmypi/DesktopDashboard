// tftHelper.h

#ifndef _TFTHELPER_h
#define _TFTHELPER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#define SERIAL_SCREEN_DEBUG // when defined, will print all screen messages also to Serial
#define TFT_DEBUG // when defined, will print all tft diagnostics to serial

#define SCREEN_WIDTH 240 
#define SCREEN_HEIGHT 320


#include "SPI.h"
#include "Adafruit_GFX.h"        // setup via Arduino IDE; Sketch - Include Library - Manage Libraries; Adafruit GFX Library 1.1.5
#include "Adafruit_ILI9341.h"    // setup via Arduino IDE; Sketch - Include Library - Manage Libraries; Adafruit ILI9341


//// For the Adafruit shield, these are the default.
//#define TFT_DC 9
//#define TFT_CS 10
//
// For the esp shield, these are the default:

#ifdef ARDUINO_ARCH_ESP8266
#define TFT_DC 2
#define TFT_CS 15
#endif

#ifdef ARDUINO_ARCH_ESP32
#define TFT_CS 14  // Chip select control pin
#define TFT_DC 27  // Data Command control pin
#define TFT_RST 33 // Reset pin (could connect to Arduino RESET pin)

// Screen
//#define TFT_LED_PIN 32
// above #define TFT_DC_PIN 27
// above #define TFT_CS_PIN 14
//#define TFT_MOSI_PIN 23
//#define TFT_CLK_PIN 18
// above #define TFT_RST_PIN 33
//#define TFT_MISO_PIN 19
#endif


#define Touch_CS 4
#define Touch_IRQ 5
#define SD_CS 9

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
extern Adafruit_ILI9341 tft;

void tftPrintlnCentered(String text);
void tftScreenClear();
void tftScreenDiagnostics();
void screenMessage(String message, String messageLine2 = "", String messageLine3 = "");


// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
// Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

// https://www.adafruit.com/product/1601
// https://www.adafruit.com/product/2298

// CONNECTOR VIEW FROM BACK
//ESP8266     Name   Display Pins  Name     ESP8266
//----------  -----   -----------  -------  -----------
//D2 - GPIO4  TP CS    26     25   GND
//D8 - GPIO15 LCD CS   24     23   SPI CLK  D5 - GPIO14
//D4 - GPIO2  DC RS    22     21   MISO     D6 - GPIO12
//                     20     19   MOSI     D7 - GPIO13
//D1 - GPIO5           18     17
//                     16     15            not used
//                     14     13   reset
//                     12     11            not used
//               Rx    10      9   GND
//               Tx     8      7  
//                      6      5
//               +5     4      3
//+5 Vin/Vcc     +5     2      1   3.3V     3v3 

// SOLDER PIN VIEW FROM DISPLAY SIDE 
//ESP8266		     Display Pins		     ESP8266
//            GND      25     26   TP CS    D2 - GPIO4
//D5 - GPIO14 SPI CLK  23     24   LCD CS   D8 - GPIO15
//D6 - GPIO12 MISO     21     22   DCRS     D4 - GPIO2
//D7 - GPIO13 MOSI     19     20
//                     17     18   TP IRQ	D1 - GPIO5
//      not used       15     16
//         reset       13     14
//      not used       11     12
//           GND        9     10   Rx
//                      7      8   Tx
//                      5      6              GND
//                      3      4   +5         
//           + 3        1	   2   +5         +5 Vin



#endif

