// created in Visual Studio 2015 with Visual Micro add-in:  vMicro - New Arduino Project
// View - Solution Explorer; Right-click on Solution in ; select "add to sourcce control"
// View - Team Explorer; Publish to GitHub (Get Started...)

// copy / paste Adafruit graphictest.ino  (see Arduino File-Examples...)
// if Arduino environment properly installed, simply vMicro-Build to automaticlly resolve references (no need to manually vMicro-Add Library...)

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


//#include "font_Arial.h"
//#include "ImageViewer.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
//#include "font_Arial.h"
//#include "FreeSansBold24pt7b.h"

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







void setup() {
	Serial.begin(9600);
	Serial.println("ILI9341 Test!");

	tft.begin();

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
	tft.setRotation(1);

	testText();
	delay(2000);
}


void screenDashboard() {
	tft.fillScreen(ILI9341_BLACK);
	tft.setCursor(0, 0);

}

unsigned long testFillScreen() {
	unsigned long start = micros();
	tft.fillScreen(ILI9341_BLACK);
	yield();
	tft.fillScreen(ILI9341_RED);
	yield();
	tft.fillScreen(ILI9341_GREEN);
	yield();
	tft.fillScreen(ILI9341_BLUE);
	yield();
	tft.fillScreen(ILI9341_BLACK);
	yield();
	return micros() - start;
}

unsigned long testText() {
	tft.fillScreen(ILI9341_BLACK);
	unsigned long start = micros();
	//tft.setFont(&FreeSansBold24pt7b);
	tft.setCursor(0, 36);
	tft.setTextColor(ILI9341_WHITE); // tft.setTextSize(1);
	tft.println("Hello World!");
	tft.setTextColor(ILI9341_YELLOW);// tft.setTextSize(2);
	tft.println(1234.56);
	tft.setTextColor(ILI9341_RED);   // tft.setTextSize(3);
	tft.println(0x121212, HEX);
	tft.setTextColor(ILI9341_GREEN);
	//tft.setTextSize(5);
	tft.println("Bisous!");
	//tft.setTextSize(2);
	return micros() - start;
}

unsigned long testLines(uint16_t color) {
	unsigned long start, t;
	int           x1, y1, x2, y2,
		w = tft.width(),
		h = tft.height();

	tft.fillScreen(ILI9341_BLACK);
	yield();

	x1 = y1 = 0;
	y2 = h - 1;
	start = micros();
	for (x2 = 0; x2<w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
	x2 = w - 1;
	for (y2 = 0; y2<h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
	t = micros() - start; // fillScreen doesn't count against timing

	yield();
	tft.fillScreen(ILI9341_BLACK);
	yield();

	x1 = w - 1;
	y1 = 0;
	y2 = h - 1;
	start = micros();
	for (x2 = 0; x2<w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
	x2 = 0;
	for (y2 = 0; y2<h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
	t += micros() - start;

	yield();
	tft.fillScreen(ILI9341_BLACK);
	yield();

	x1 = 0;
	y1 = h - 1;
	y2 = 0;
	start = micros();
	for (x2 = 0; x2<w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
	x2 = w - 1;
	for (y2 = 0; y2<h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);
	t += micros() - start;

	yield();
	tft.fillScreen(ILI9341_BLACK);
	yield();

	x1 = w - 1;
	y1 = h - 1;
	y2 = 0;
	start = micros();
	for (x2 = 0; x2<w; x2 += 6) tft.drawLine(x1, y1, x2, y2, color);
	x2 = 0;
	for (y2 = 0; y2<h; y2 += 6) tft.drawLine(x1, y1, x2, y2, color);

	yield();
	return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
	unsigned long start;
	int           x, y, w = tft.width(), h = tft.height();

	tft.fillScreen(ILI9341_BLACK);
	start = micros();
	for (y = 0; y<h; y += 5) tft.drawFastHLine(0, y, w, color1);
	for (x = 0; x<w; x += 5) tft.drawFastVLine(x, 0, h, color2);

	return micros() - start;
}

unsigned long testRects(uint16_t color) {
	unsigned long start;
	int           n, i, i2,
		cx = tft.width() / 2,
		cy = tft.height() / 2;

	tft.fillScreen(ILI9341_BLACK);
	n = min(tft.width(), tft.height());
	start = micros();
	for (i = 2; i<n; i += 6) {
		i2 = i / 2;
		tft.drawRect(cx - i2, cy - i2, i, i, color);
	}

	return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
	unsigned long start, t = 0;
	int           n, i, i2,
		cx = tft.width() / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	n = min(tft.width(), tft.height());
	for (i = n; i>0; i -= 6) {
		i2 = i / 2;
		start = micros();
		tft.fillRect(cx - i2, cy - i2, i, i, color1);
		t += micros() - start;
		// Outlines are not included in timing results
		tft.drawRect(cx - i2, cy - i2, i, i, color2);
		yield();
	}

	return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
	unsigned long start;
	int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

	tft.fillScreen(ILI9341_BLACK);
	start = micros();
	for (x = radius; x<w; x += r2) {
		for (y = radius; y<h; y += r2) {
			tft.fillCircle(x, y, radius, color);
		}
	}

	return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
	unsigned long start;
	int           x, y, r2 = radius * 2,
		w = tft.width() + radius,
		h = tft.height() + radius;

	// Screen is not cleared for this one -- this is
	// intentional and does not affect the reported time.
	start = micros();
	for (x = 0; x<w; x += r2) {
		for (y = 0; y<h; y += r2) {
			tft.drawCircle(x, y, radius, color);
		}
	}

	return micros() - start;
}

unsigned long testTriangles() {
	unsigned long start;
	int           n, i, cx = tft.width() / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	n = min(cx, cy);
	start = micros();
	for (i = 0; i<n; i += 5) {
		tft.drawTriangle(
			cx, cy - i, // peak
			cx - i, cy + i, // bottom left
			cx + i, cy + i, // bottom right
			tft.color565(i, i, i));
	}

	return micros() - start;
}

unsigned long testFilledTriangles() {
	unsigned long start, t = 0;
	int           i, cx = tft.width() / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	start = micros();
	for (i = min(cx, cy); i>10; i -= 5) {
		start = micros();
		tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
			tft.color565(0, i * 10, i * 10));
		t += micros() - start;
		tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
			tft.color565(i * 10, i * 10, 0));
		yield();
	}

	return t;
}

unsigned long testRoundRects() {
	unsigned long start;
	int           w, i, i2,
		cx = tft.width() / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	w = min(tft.width(), tft.height());
	start = micros();
	for (i = 0; i<w; i += 6) {
		i2 = i / 2;
		tft.drawRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(i, 0, 0));
	}

	return micros() - start;
}

unsigned long testFilledRoundRects() {
	unsigned long start;
	int           i, i2,
		cx = tft.width() / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	start = micros();
	for (i = min(tft.width(), tft.height()); i>20; i -= 6) {
		i2 = i / 2;
		tft.fillRoundRect(cx - i2, cy - i2, i, i, i / 8, tft.color565(0, i, 0));
		yield();
	}

	return micros() - start;
}