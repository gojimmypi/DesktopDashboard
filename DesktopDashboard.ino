// 
// created in Visual Studio 2015 with Visual Micro add-in:  vMicro - New Arduino Project
//   https://www.visualstudio.com/downloads/
//
//   http://www.visualmicro.com/
//
// View - Solution Explorer; Right-click on Solution in ; select "add to sourcce control"
// View - Team Explorer; Publish to GitHub (Get Started...)
//
// copy / paste Adafruit graphictest.ino  (see Arduino File-Examples...)
// if Arduino environment properly installed, simply vMicro-Build to automaticlly resolve references (no need to manually vMicro-Add Library...)
//
// Default vMicro project location is C:\Users\<username>\Documents\Arduino\<ProjectName>

/***************************************************
This is our GFX example for the Adafruit ILI9341 Breakout and Shield
----> http://www.adafruit.com/products/1651

Check out the links above for our tutorials and wiring diagrams
These displays use SPI to communicate, 4 or 5 pins are required to
interface (RST is optional)
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
MIT license, all text above must be included in any redistribution
****************************************************/


// include "ili9341test.h"
#include "settings.h"
#include "debughandler.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include "ImageViewer.h"
#include "wifiConnectHelper.h"
#include "SPI.h"
#include "Adafruit_GFX.h" // setup via Arduino IDE; Sketch - Include Library - Manage Libraries; Adafruit GFX Library 1.1.5
#include "Adafruit_ILI9341.h"  // setup via Arduino IDE; Sketch - Include Library - Manage Libraries; Adafruit ILI9341
#include "FreeSansBold24pt7b.h" // copy to project directory from Adafruit-GFX-Library\Fonts; show all files. right-click "include in project"

//// For the Adafruit shield, these are the default.
//#define TFT_DC 9
//#define TFT_CS 10
// For the esp shield, these are the default.
#define TFT_DC 2
#define TFT_CS 15

#define Touch_CS 4
#define Touch_IRQ 5
#define SD_CS 9

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

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




const char* ssid = "mySSID"; 
const char* password = "myPassword";



void setup() {
	Serial.begin(115200);
	Serial.println("ILI9341 Test!");

	tft.begin();

	delay(20);
	uint8_t tx = tft.readcommand8(ILI9341_RDMODE);
	tx = tft.readcommand8(ILI9341_RDSELFDIAG);
	delay(20);
	Serial.print("Self Diagnostic: 0x"); Serial.println(tx, HEX);
	delay(20);
	tft.fillScreen(ILI9341_BLACK);
	delay(20);
	tft.setRotation(2);
	tft.drawRect(0, 0, 240, 320, 0x00FF);
	delay(20);


	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {  // try to connect wifi for 6 sec then reset
		tft.setTextColor(ILI9341_BLUE);
		tft.setCursor(15, 195);
		delay(250);
		tft.setTextColor(ILI9341_RED);
		tft.setCursor(15, 195);
		Serial.print(".");
		delay(250);
	}
	Serial.println("WiFi connected");
	Serial.println("");
	Serial.println(WiFi.localIP());

	tft.setCursor(0, 0);
	// http://healthagency.slocounty.ca.gov/macVPN/24bit.bmp
	bmpDraw(&tft, "http://gojimmypi-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=image.png&newImageSizeY=100");
	delay(2000);

	tft.setCursor(0, 0);
	bmpDraw(&tft, "http://healthagency.slocounty.ca.gov/macVPN/256color.bmp");
	delay(2000);

	tft.setCursor(0, 0);
	bmpDraw(&tft, "http://healthagency.slocounty.ca.gov/macVPN/16color.bmp");
	delay(2000);

	tft.setCursor(0, 0);
	bmpDraw(&tft, "http://healthagency.slocounty.ca.gov/macVPN/mono.bmp");
	delay(2000);
	// Hello World!
	
	tft.setCursor(0, 0);
	//dldDImage(&tft, 0, 0);

	// read diagnostics (optional but can help debug problems)
	uint8_t x = tft.readcommand8(ILI9341_RDMODE);
	Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX); // success =  0x9C
	x = tft.readcommand8(ILI9341_RDMADCTL);
	Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX); // Sucess =  0x48
	x = tft.readcommand8(ILI9341_RDPIXFMT);
	Serial.print("Pixel Format: 0x"); Serial.println(x, HEX); // Success =  0x5
	x = tft.readcommand8(ILI9341_RDIMGFMT);
	Serial.print("Image Format: 0x"); Serial.println(x, HEX); // Success = 0x0
	x = tft.readcommand8(ILI9341_RDSELFDIAG);
	Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); // Success =  0x0
//
	//Serial.println(F("Benchmark                Time (microseconds)"));
	//delay(10);
	//Serial.print(F("Screen fill              "));
	//Serial.println(testFillScreen());
	//delay(500);

	//Serial.print(F("Text                     "));
	//Serial.println(testText());
	//delay(3000);

	//Serial.print(F("Lines                    "));
	//Serial.println(testLines(ILI9341_CYAN));
	//delay(500);

	//Serial.print(F("Horiz/Vert Lines         "));
	//Serial.println(testFastLines(ILI9341_RED, ILI9341_BLUE));
	//delay(500);

	//Serial.print(F("Rectangles (outline)     "));
	//Serial.println(testRects(ILI9341_GREEN));
	//delay(500);

	//Serial.print(F("Rectangles (filled)      "));
	//Serial.println(testFilledRects(ILI9341_YELLOW, ILI9341_MAGENTA));
	//delay(500);

	//Serial.print(F("Circles (filled)         "));
	//Serial.println(testFilledCircles(10, ILI9341_MAGENTA));

	//Serial.print(F("Circles (outline)        "));
	//Serial.println(testCircles(10, ILI9341_WHITE));
	//delay(500);

	//Serial.print(F("Triangles (outline)      "));
	//Serial.println(testTriangles());
	//delay(500);

	//Serial.print(F("Triangles (filled)       "));
	//Serial.println(testFilledTriangles());
	//delay(500);

	//Serial.print(F("Rounded rects (outline)  "));
	//Serial.println(testRoundRects());
	//delay(500);

	//Serial.print(F("Rounded rects (filled)   "));
	//Serial.println(testFilledRoundRects());
	//delay(500);

	Serial.println(F("Done!"));

}


void loop(void) {
	tft.setRotation(3);

	testText();
	delay(2000);
}


void screenDashboard() {
	tft.fillScreen(ILI9341_BLACK);
	tft.setCursor(0, 0);

}

unsigned long testText() {
	tft.fillScreen(ILI9341_BLACK);
	unsigned long start = micros();
	tft.setFont(&FreeSansBold24pt7b); // load our custom 24pt font
	tft.setCursor(0, 36);
	tft.setTextColor(ILI9341_WHITE); // tft.setTextSize(1);
	tft.println("Hello World!");
	tft.setTextColor(ILI9341_YELLOW);// tft.setTextSize(2);
	tft.println("Productivity:");
	tft.setTextColor(ILI9341_RED);   // tft.setTextSize(3);
	tft.println("  35%");
	tft.setTextColor(ILI9341_GREEN);
	//tft.setTextSize(5);
	tft.println("");
	//tft.setTextSize(2);
	return micros() - start;
}