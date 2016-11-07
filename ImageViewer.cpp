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