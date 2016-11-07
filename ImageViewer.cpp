// 
// 
// 

#include "ImageViewer.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// ImageViewer.h
//
// code based on ESP8266 WiFi Image Viewer by James Eckert
// http://jeplans.com/default.php?targp=ESP2Electronics2
//

int ah2i(uint8_t s)
{
	int a = 0;

	uint8_t c = s;
	if (c >= 48 && c <= 57) {
		a = c - 48;
	}
	else if (c >= 65 && c <= 70) {
		a = 16;
		a += (c - 65);
	}
	else if (c >= 97 && c <= 102) {
		a = 16;
		a += (c - 97);
	}
	return a;
}


void dldDImage(Adafruit_ILI9341 * tft, uint16_t  xloc, uint16_t yloc) {
	uint16_t  r;
	uint8_t hb, lb, cv, lv1, lv2, cb1, cb2;
	uint16_t  frsize, iwidth, iheight;
	int x = 0, y = 0;
	int firstpacket = 0;
	int buffcnt = 0;
	int bytescolleted = 0;

	HTTPClient http;

	Serial.print("[HTTP] begin...\n");

	// configure server and url
	//char * imagePath = "http://beanstalk.azurewebsites.net/helloworld.png";
	char * imagePath = "http://beanstalk.azurewebsites.net/newimage.bin";
	http.begin(imagePath);
	Serial.print("[HTTP] GET...\n");
	Serial.print(imagePath);
	Serial.println();
	// start connection and send HTTP header
	int httpCode = http.GET();
	if (httpCode > 0) {
		// HTTP header has been send and Server response header has been handled
		Serial.printf("[HTTP] GET... code: %d\n", httpCode);

		// file found at server
		if (httpCode == HTTP_CODE_OK) {
			unsigned long DrawTime = millis();
			// get lenght of document (is -1 when Server sends no Content-Length header)
			int len = http.getSize();
			// create buffer for read
			uint8_t buff[2048] = { 0 };
			int  buffidx = sizeof(buff);
			// get tcp stream
			WiFiClient * stream = http.getStreamPtr();
			uint16_t BytesToRead1 = 0;
			uint16_t BytesToRead2 = 8192;
			// read all data from server
			while (http.connected() && (len > 0 || len == -1)) {
				// get available data size
				size_t size = stream->available();
				//Serial.println("size="+String(size));
				if (size) {
					// read up to 128 byte
					buffidx = 0;
					int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
					// Uncomment Below to send raw output it to Serial 
					// Serial.write(buff, c);

					while (buffidx < c) {
						// Strip out info not part of picture Steam could be multiple packets 
						// each packet has size data that needs to be striptied out or it will mess up picture 
						// also we need to know when data occurs next packet to strip out
						if (firstpacket == 0 || buffcnt > (BytesToRead2 - 1)) {
							int rflag = 0;
							int hcnt = 0;
							// Serial.print("<New Packet>");
							while (rflag == 0) {
								cv = buff[buffidx++];
								//Serial.write(cv);
								if (cv == 10 && lv1 == 13 && lv2 > 47) {
									rflag = 1;
								}
								if (cv > 47) {
									BytesToRead1 = BytesToRead1 << 4;
									BytesToRead1 = BytesToRead1 + ah2i(cv);
								}
								lv2 = lv1;
								lv1 = cv;
								hcnt++;
								//if end packet size data not found in fist 10 bytes only one packet reset buffidx so we don't miss width data
								if (hcnt > 10) { rflag = 1; buffidx = 0; BytesToRead1 = 8192; }
							}
							buffcnt = 0; //start count over to watch for next packet start
										 //First Two Bytes Contain Width 
							if (firstpacket == 0) {
								hb = buff[buffidx++];
								lb = buff[buffidx++];
								iwidth = hb;
								iwidth = iwidth << 8;
								iwidth = iwidth + lb;
								Serial.println("Width=" + String(iwidth));
								buffcnt = buffcnt + 2;
							}
							yield();
							//2nd Two Bytes Contain height
							if (firstpacket == 0) {
								hb = buff[buffidx++];
								lb = buff[buffidx++];
								iheight = hb;
								iheight = iheight << 8;
								iheight = iheight + lb;
								Serial.println("Height=" + String(iheight));
								buffcnt = buffcnt + 2;
							}
							firstpacket = 1;
							//Serial.println("Bytes to next packet="+String(BytesToRead1));
							BytesToRead2 = BytesToRead1;
							//Serial.println("</New Packet!!>");

						}

						// read in bytes 1 at a time when you have two plot pixel  
						// can't read 2 at a time becuse packet boundries are sometime odd
						if (bytescolleted < 1) {
							cb1 = buff[buffidx++];
							bytescolleted++;
							buffcnt++;
						}
						else {
							cb2 = buff[buffidx++];
							bytescolleted++;
							buffcnt++;
						}
						if (bytescolleted > 1) {
							r = word(cb1, cb2);
							if (y + yloc < iheight) {
								tft->drawPixel(x + xloc, y + yloc, r);
							}
							bytescolleted = 0;
							x++;
							if (x > iwidth - 1) {
								y++;x = 0;
							}
						}

					}
					if (len > 0) {
						len -= c;
					}
				}
				delay(2);
			}
			Serial.println("y=" + String(y));
			Serial.println("[HTTP] connection closed or file end.\n");
			Serial.println(String(millis() - DrawTime));
			tft->fillRect(1, 180, 238, 70, 0x0000);
			tft->setCursor(15, 195);
			tft->setTextColor(ILI9341_RED);  
			tft->println(String(millis() - DrawTime) + "ms");
			buffcnt = 0;
			firstpacket = 0;
			x = 0;y = 0;
			buffidx = 0;
			Serial.println("Done! waiting...");
			delay(5000);
			//t2Time = millis();
		}
	}
	else {
		Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
	}

	http.end();
	//HRequestsActive = 0;
}


/***************************************************
This is our Bitmap drawing example for the Adafruit ILI9341 Breakout and Shield
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


//#include <Adafruit_GFX.h>    // Core graphics library
////#include "Adafruit_ILI9341.h" // Hardware-specific library
//#include <ILI9341_t3.h>
//#include <SPI.h>
//#include <SD.h>

// TFT display and SD card will share the hardware SPI interface.
// Hardware SPI pins are specific to the Arduino board type and
// cannot be remapped to alternate pins.  For Arduino Uno,
// Duemilanove, etc., pin 11 = MOSI, pin 12 = MISO, pin 13 = SCK.

//#define TFT_DC 9
//#define TFT_CS 10
////Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
//ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, 8);
//
//#define SD_CS 4
//
//int pUp = 0;
//int pDown = 1;
//int pLeft = 2;
//int pRight = 3;
//int pCenter = 5;
//
//char *mains[] = { "m0.bmp","m1.bmp","m2.bmp" };
//bool main_page = true;
//bool run_page = false;
//bool menu_page = false;
//int mainn = 0;

//void setup(void) {
//
//	Serial.begin(9600);
//	Serial.println("OKOK");
//
//	delay(1000);
//
//	tft.begin();
//	tft.fillScreen(ILI9341_BLUE);
//
//	Serial.print("Initializing SD card...");
//	if (!SD.begin(SD_CS)) {
//		Serial.println("failed!");
//	}
//	else {
//		Serial.println("OK!");
//	}
//
//
//	pinMode(pUp, INPUT_PULLUP);
//	pinMode(pDown, INPUT_PULLUP);
//	pinMode(pLeft, INPUT_PULLUP);
//	pinMode(pRight, INPUT_PULLUP);
//	pinMode(pCenter, INPUT_PULLUP);
//
//	bmpDraw(mains[0], 0, 0);
//	//bmpDraw("test.bmp", 0, 0);
//}


//void loop() {
//
//	if (digitalRead(pUp) == LOW) {
//		delay(50);
//
//		if (main_page == true) {
//			mainn--;
//			if (mainn<0) { mainn = 3; }
//			//tft.fillScreen(ILI9341_BLACK);
//			bmpDraw(mains[mainn], 0, 0);
//		}
//	}
//	if (digitalRead(pDown) == LOW) {
//		delay(50);
//
//		if (main_page == true) {
//			mainn++;
//			if (mainn>2) { mainn = 0; }
//			//tft.fillScreen(ILI9341_BLACK);
//			bmpDraw(mains[mainn], 0, 0);
//		}
//	}
//
//	if (digitalRead(pCenter) == LOW) {
//		if (main_page == true) {
//			if (mainn == 0) {
//				main_page = false;
//				//tft.fillScreen(ILI9341_BLACK);
//				bmpDraw("run.bmp", 0, 0);
//			}
//			if (mainn == 2) {
//				main_page = false;
//				//tft.fillScreen(ILI9341_BLACK);
//				bmpDraw("menu.bmp", 0, 0);
//			}
//		}
//		else {
//			bmpDraw(mains[0], 0, 0);
//			mainn = 0;
//			main_page = true;
//		}
//	}
//
//}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(WiFiClient * f) {

	uint16_t result;
	((uint8_t *)&result)[0] = f->read(); // LSB
	((uint8_t *)&result)[1] = f->read(); // MSB
	return result;
}

uint32_t read32(WiFiClient * f) {
	uint32_t result;
	((uint8_t *)&result)[0] = f->read(); // LSB
	((uint8_t *)&result)[1] = f->read();
	((uint8_t *)&result)[2] = f->read();
	((uint8_t *)&result)[3] = f->read(); // MSB
	return result;
}

#define BUFFPIXEL 240
//===========================================================
// Try Draw using writeRect
void bmpDraw(Adafruit_ILI9341 * tft, uint8_t x, uint16_t y) {

	int      bmpWidth, bmpHeight;   // W+H in pixels
	uint8_t  bmpDepth;              // Bit depth (currently must be 24)
	uint32_t bmpImageoffset;        // Start of image data in file
	uint32_t rowSize;               // Not always = bmpWidth; may have padding
	uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
	uint16_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
	boolean  goodBmp = false;       // Set to true on valid header parse
	boolean  flip = true;        // BMP is stored bottom-to-top
	int      w, h, row, col;
	uint8_t  r, g, b;
	uint32_t pos = 0, startTime = millis();

	uint16_t awColors[320];  // hold colors for one row at a time...

	if ((x >= tft->width()) || (y >= tft->height())) return;

	Serial.print(F("Loading image '"));
	//Serial.print(filename);
	Serial.println('\'');

	uint32_t us;
	uint32_t total_seek = 0;
	uint32_t total_read = 0;
	uint32_t total_parse = 0;
	uint32_t total_draw = 0;

	HTTPClient http;

	Serial.print("[HTTP] begin...\n");

	// configure server and url
	//char * imagePath = "http://beanstalk.azurewebsites.net/helloworld.png";
	//char * imagePath = "http://beanstalk.azurewebsites.net/newimage.bin";
	char * imagePath = "http://beanstalk.azurewebsites.net/helloworld.bmp";
	http.begin(imagePath);
	Serial.print("[HTTP] GET...\n");
	Serial.print(imagePath);
	Serial.println();
	// start connection and send HTTP header
	int httpCode = http.GET();
	WiFiClient * stream = http.getStreamPtr();

	// Parse BMP header
	if (read16(stream) == 0x4D42) { // BMP signature
		Serial.print(F("File size: ")); Serial.println(read32(stream));
		(void)read32(stream); // Read & ignore creator bytes
		bmpImageoffset = read32(stream); // Start of image data
		Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
		// Read DIB header
		Serial.print(F("Header size: ")); Serial.println(read32(stream));
		bmpWidth = read32(stream);
		bmpHeight = read32(stream);
		if (read16(stream) == 1) { // # planes -- must be '1'
			bmpDepth = read16(stream); // bits per pixel
			Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
			if ((bmpDepth == 24) && (read32(stream) == 0)) { // 0 = uncompressed

				goodBmp = true; // Supported BMP format -- proceed!
				Serial.print(F("Image size: "));
				Serial.print(bmpWidth);
				Serial.print('x');
				Serial.println(bmpHeight);

				// BMP rows are padded (if needed) to 4-byte boundary
				rowSize = (bmpWidth * 3 + 3) & ~3;

				// If bmpHeight is negative, image is in top-down order.
				// This is not canon but has been observed in the wild.
				if (bmpHeight < 0) {
					bmpHeight = -bmpHeight;
					flip = false;
				}

				// Crop area to be loaded
				w = bmpWidth;
				h = bmpHeight;
				if ((x + w - 1) >= tft->width())  w = tft->width() - x;
				if ((y + h - 1) >= tft->height()) h = tft->height() - y;

				//usec = 0;
				for (row = 0; row<h; row++) { // For each scanline...

											  // Seek to start of scan line.  It might seem labor-
											  // intensive to be doing this on every line, but this
											  // method covers a lot of gritty details like cropping
											  // and scanline padding.  Also, the seek only takes
											  // place if the file position actually needs to change
											  // (avoids a lot of cluster math in SD library).
					if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
						pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
					else     // Bitmap is stored top-to-bottom
						pos = bmpImageoffset + row * rowSize;

					//if (bmpFile.position() != pos) { // Need seek?
					//	bmpFile.seek(pos);
					//	buffidx = sizeof(sdbuffer); // Force buffer reload
					//}

					//us = usec;
					//usec -= us;
					//total_seek += us;

					for (col = 0; col<w; col++) { // For each pixel...
												  // Time to read more pixel data?
						if (buffidx >= sizeof(sdbuffer)) { // Indeed
							//us = usec;
							//usec -= us;
							//total_parse += us;
							stream->read(sdbuffer, sizeof(sdbuffer));
							buffidx = 0; // Set index to beginning
							//us = usec;
							//usec -= us;
							//total_read += us;
						}

						// Convert pixel from BMP to TFT format, push to display
						b = sdbuffer[buffidx++];
						g = sdbuffer[buffidx++];
						r = sdbuffer[buffidx++];
						awColors[col] = tft->color565(r, g, b);
					} // end pixel
					//us = usec;
					//usec -= us;
					//total_parse += us;

					//tft.writeRect(0, row, w, 1, awColors); // TODO

					//us = usec;
					//usec -= us;
					//total_draw += us;
				} // end scanline
				Serial.print(F("Loaded in "));
				Serial.print(millis() - startTime);
				Serial.println(" ms");
				Serial.print("Seek: ");
				Serial.println(total_seek);
				Serial.print("Read: ");
				Serial.println(total_read);
				Serial.print("Parse: ");
				Serial.println(total_parse);
				Serial.print("Draw: ");
				Serial.println(total_draw);
			} // end goodBmp
		}
	}

	//bmpFile.close();
	if (!goodBmp) Serial.println(F("BMP format not recognized."));
}



