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
// http://stackoverflow.com/questions/5751749/how-can-i-read-bmp-pixel-values-into-an-array
// https://msdn.microsoft.com/en-us/library/dd183391(v=vs.85).aspx
// http://www.instructables.com/id/Arduino-TFT-display-of-bitmap-images-from-an-SD-Ca/?ALLSTEPS
// https://github.com/Bodmer/TFT_ILI9341
// https://github.com/magore/esp8266_ili9341
// https://github.com/adafruit/Adafruit-GFX-Library
// http://stackoverflow.com/questions/19131556/how-to-get-rgb888-24-bit-and-rgb565-16-bit-framebuffer-dump-from-a-jpg-ima
// http://www.avrfreaks.net/forum/displaying-image-tft-display
// https://en.wikipedia.org/wiki/RGBA_color_space
// https://sourceforge.net/projects/easybmp/?source=typ_redirect
// https://bytes.com/topic/c/answers/554304-using-32-bit-bitmaps-c

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

uint16_t read16(unsigned char * d, int fromIndex) {

	uint16_t result;
	((uint8_t *)&result)[0] = d[fromIndex]; // LSB
	((uint8_t *)&result)[1] = d[fromIndex + 1]; // MSB
	return result;
}

uint32_t read32(unsigned char * d, int fromIndex) {
	uint32_t result;
	((uint8_t *)&result)[0] = d[fromIndex]; // LSB
	((uint8_t *)&result)[1] = d[fromIndex + 1];
	((uint8_t *)&result)[2] = d[fromIndex + 2];
	((uint8_t *)&result)[3] = d[fromIndex + 3]; // MSB
	return result;
}


unsigned char* bmpRawReader(WiFiClient * f) {
	// based on code from http://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file
	uint8_t info[54];

	f->readBytes(info, 54); // read the 54-byte header

							// extract image height and width from header
	int width = *(int*)&info[18];
	int height = *(int*)&info[22];

	int size = 3 * width * height;
	unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel

	f->readBytes(data, size);

	for (int i = 0; i < size; i += 3)
	{
		unsigned char tmp = data[i];
		data[i] = data[i + 2];
		data[i + 2] = tmp;
	}
	return data;
}

boolean bmpReadHeader(WiFiClient * f)
{

	// read header
	uint32_t tmp;
	uint8_t bmpDepth;

	if (read16(f) != 0x4D42) {
		Serial.println("Magic bytes missing");
		return false;
	}

	// read file size
	tmp = read32(f);
	Serial.print("size 0x");
	Serial.println(tmp, HEX);

	// read and ignore creator bytes
	read32(f);

	uint32_t offset = read32(f);
	Serial.print("offset ");
	Serial.println(offset, DEC);

	// read DIB header
	tmp = read32(f);
	Serial.print("header size ");
	Serial.println(tmp, DEC);


	int bmp_width = read32(f);
	int bmp_height = read32(f);
	Serial.print("Width:");
	Serial.println(bmp_width);
	Serial.print("Height:");
	Serial.println(bmp_height);

	//if (bmp_width != 240 || bmp_height != 320)      // if image is not 320x240, return false
	//{
	//	Serial.println("Wrong dimensions");
	//	return false;
	//}

	if (read16(f) != 1)
	{
		Serial.println("Wrong # color planes");
		return false;
	}

	bmpDepth = read16(f);
	Serial.print("bitdepth ");
	Serial.println(bmpDepth, DEC);

	if (read32(f) != 0) {
		Serial.println("Compression not supported!");
		return false;
	}

	read32(f); //imagesizes
	read32(f);//horizontal resolution
	read32(f);//vertical resolution
	read32(f);//number of colors
	read32(f);//number of important colors
	return true;
}

// http://cppcoder.blogspot.com/2007/11/bmp-file-format.html



int len = 1;
unsigned char* data = new unsigned char[len];
int currentStreamPosition = 0;
int currentStreamPayloadPosition = 0;  // there are often chunks of data returned in separate "payloads"
int totalBytesRead = 0;

uint8_t byteInStream(WiFiClient * stream, int position) {
	int thisPayloadBytesAvailable = 0;
	int thisPayloadByteCount = 0;
	boolean foundPayload = false;

	if (position > totalBytesRead) {
		while ((totalBytesRead < len) && (!foundPayload)) {
			thisPayloadBytesAvailable = stream->available();
			if ((totalBytesRead < len) && (thisPayloadBytesAvailable == 0)) {
				yield();
				thisPayloadBytesAvailable = stream->available();
			}
			if (thisPayloadBytesAvailable > 0) {
				delete data; // without deleting old data, we run out of heap space!
				data = new unsigned char[thisPayloadBytesAvailable]; // allocation of just enough memory for this incremenal payload of data
				Serial.print("Reading.... ");
				yield();
				
				//thisPayloadByteCount = stream->read(data + totalBytesRead, thisPayloadBytesAvailable); // the old code appended to one big data array

				thisPayloadByteCount = stream->read(data, thisPayloadBytesAvailable);
				currentStreamPosition = totalBytesRead + 1; // the prior [total bytes read] + 1 is the new stream position
				totalBytesRead += thisPayloadByteCount; // this is the ending position of the stream payload we now have
				Serial.print(thisPayloadByteCount);
				Serial.println(" bytes. Done.");
				foundPayload = true; // once we find and read the next payload, we exit
				Serial.printf("settings heap size: %u\n", ESP.getFreeHeap());
				Serial.print("  total read = ");
				Serial.println(totalBytesRead);
			}
		}
	}
	return data[position - currentStreamPosition + 1]; // TODO, reference position in payload only, not full stream
}
void bmpDrawFromUrl(Adafruit_ILI9341 * tft, char * imagePath)
{
	HTTPClient http;
	http.begin(imagePath);

	Serial.print("Starting bmpDrawFromUrl 1.01: ");
	Serial.print(imagePath);
	Serial.print("  -  ");
	uint32_t time = millis();
	int httpCode = http.GET();
	Serial.println(httpCode);
	if (httpCode > 0) {
		// HTTP header has been send and Server response header has been handled
		// file found at server
		if (httpCode == HTTP_CODE_OK) {
			Serial.printf("settings heap size: %u\n", ESP.getFreeHeap());

			unsigned long DrawTime = millis();
			// get lenght of document (is -1 when Server sends no Content-Length header)
			 len = http.getSize();
			unsigned char tmp;
			Serial.printf("settings heap size: %u\n", ESP.getFreeHeap());
			Serial.print("  length = ");
			Serial.println(len);
			WiFiClient * stream = http.getStreamPtr();
			
			unsigned char testChar = byteInStream(stream, 1); // test only TODO remove

			Serial.println("Step 1");
//			int thisBytesAvailable = stream->available();
//			data = new unsigned char[thisBytesAvailable];
//
//			Serial.print("thisBytesAvailable = ");
//			Serial.print(thisBytesAvailable);
//			Serial.println("Step 2");
			Serial.printf("settings heap size: %u\n", ESP.getFreeHeap());

//			totalBytesRead = stream->read(data, thisBytesAvailable);
			Serial.print("Initial Read = ");
			Serial.println(totalBytesRead);


			const int HEADER_SIZE = 54;
			// stream->read(info, HEADER_SIZE); // read the 54-byte header

			// extract image height and width from header
			int bfOffBits = read32(data, 10); //  *(int*)&data[10]; // typically 1078 

			int biSize = read32(data, 14); // Specifies the size of the BITMAPINFOHEADER structure, in bytes. (typically 40 bytes)
			int biWidth = read32(data, 18); // *(int*)data + 18;
			int biHeight = read32(data, 22); // *(int*)data + 22;
			int biBitCount = read32(data, 28); // Specifies the number of bits per pixel.
			int biSizeImage = read32(data, 34); // *(int*)data + 34;

			Serial.print("filelen = ");
			Serial.println(len);

			Serial.print("biSize = ");
			Serial.println(biSize);
			Serial.print("Offset = "); // Specifies the offset from the beginning of the file to the bitmap data.
			Serial.println(bfOffBits);


			Serial.print("biSize = "); // Specifies the size of the BITMAPINFOHEADER structure, in bytes.
			Serial.println(biSize);
			Serial.print("biWidth = ");
			Serial.println(biWidth);
			Serial.print("biHeight = ");
			Serial.println(biHeight);
			Serial.print("biBitCount = ");
			Serial.println(biBitCount);
			Serial.print("biSizeImage = ");
			Serial.println(biSizeImage);

			int size = 3 * biWidth * biHeight;

			int maxWait = 100;
			int thisWait = 0;

			// moved to byteInStream
			//int thisBytesRead = HEADER_SIZE;
			//Serial.print("Initial bytes available: ");
			//Serial.println(thisBytesAvailable);
			//while (i < len) {
			//	thisBytesAvailable = stream->available();
			//	if ((i < len) && (thisBytesAvailable == 0)) {
			//		yield();
			//		thisBytesAvailable = stream->available();
			//	}
			//	if (thisBytesAvailable > 0) {
			//		Serial.print("Reading.... ");
			//		yield();
			//		thisResult = stream->read(data + i, thisBytesAvailable);
			//		i += thisResult;
			//		Serial.print(thisResult);
			//		Serial.println(" bytes. Done.");
			//		delay(1);
			//	}
			//}
			Serial.print("Render! totalBytesRead = ");
			Serial.println(totalBytesRead);
			if (biBitCount == 1) {
				Serial.println("B/W not rendered");
				tft->println("B/W not rendered");
			} // BW
			else if (biBitCount == 4) {
				Serial.println("4 bit not rendered");
			} // 4 - bit
			else if (biBitCount == 8) { // (((biWidth * biHeight) + HEADER_SIZE) == len) {
				Serial.println("8 bit pic");
				tft->println("8-bit not rendered");
				for (int i = 0; i < biHeight; i++)
				{
					thisWait = 0;
					yield();
					//while ((stream->available() == 0) && (thisWait++ < maxWait)) {
					//	Serial.print(".");
					//	delay(10);
					//}
					for (int j = 0; j < biWidth; j++)
					{
						tft->drawPixel(j, i, data[bfOffBits + biWidth * i + j]);
					}
				}
			} // 8 - bit

			else if (biBitCount == 24) {
				Serial.println("bmp24");
				//int count = bfOffBits;
				int count = 0; // note that we start at the END of the data [biSizeImage - count++]  and work ourselves to the beginning of the data
				int extra = biWidth % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
										 //for (int i = 0; i < biHeight; i++) // rows of data make up height
				for (int i = biHeight - 1; i >= 0; i--) // rows of data make up height
				{
					count += extra;
					yield();
					uint8_t r; uint8_t g; uint8_t b;

					//for (int j = 0; j <= biWidth - 1; j++) // renders reverse image
					for (int j = biWidth - 1; j >= 0; j--)
					{
						for (int k = 0; k < 3; k++) {
							switch (k) {
							case 0:
								r = data[len - ++count];
								break;
							case 1:
								g = data[len - ++count];
								break;
							case 2:
								b = data[len - ++count];
								break;
							}
						}
						// Convert (B, G, R) to (R, G, B)
						//tmp = data[j];
						//data[j] = data[j + 2];
						//data[j + 2] = tmp;
						// int thisIndex = bfOffBits + biWidth * i + j;
						tft->drawPixel(i, j, tft->color565(r, g, b));
					}
				}
				delay(2000000);
			} // 24 bit

			else if (biBitCount == 32) {
				//int count = bfOffBits;
				int count = bfOffBits; // note that we start at the START of the data [biSizeImag]  and work ourselves to the END of the data
				//int extra = 0; // The nubmer of bytes in a row (cols) will already be a multiple of 4. (32 / 8) so we don'n need to pad with extra bytes as done in 24 bit

				// show the first chunk of data 
				for (int j = 0; j < 66; j++) {
					for (int i = 0; i < 16; i++) {
						Serial.print(data[(j * 16) + i], HEX);
						Serial.print(" ");
					}
					Serial.println();
				}
				//for (int i = biHeight - 1; i >= 0; i--) // rows of data make up height
				for (int i = 0; i < biHeight; i++) // rows of data make up height
				{
					//count += extra;
					yield();
					uint8_t r; uint8_t g; uint8_t b; uint8_t a;

					for (int j = 0; j < biWidth; j++)
					{
						for (int k = 0; k < 4; k++) {
							switch (k) {
							case 3:
								a = byteInStream(stream, count++); // data[count++]; // this is alpha, which we we simply ignore. TODO what if it is not 0xFF?
								break;
							case 2:
								r = byteInStream(stream, count++); // red
								break;
							case 1:
								g = byteInStream(stream, count++); // green
								break;
							case 0:
								b = byteInStream(stream, count++); // blue
								break;
							}
							yield();
							//Serial.print(count);
							//Serial.print(" : ");
							//Serial.print(r, HEX);
							//Serial.print(" ");
							//Serial.print(g, HEX);
							//Serial.print(" ");
							//Serial.print(b, HEX);
							//Serial.print(" ");
							//Serial.print(a, HEX);
							//Serial.println();

						}
						tft->drawPixel(i, j, tft->color565(r, g, b));
					}
					Serial.println(len - count);
				}
				delay(2000000);
			} // 32 bit

			else {
				Serial.print("Unsupported bit depth: ");
				Serial.println(biBitCount);
				tft->print("Unsupported bit depth: ");
				tft->println(biBitCount);
			}



			stream->flush();
			stream->stopAll();

		}
	}
	Serial.print(millis() - time, DEC);
	Serial.println(" ms");
	http.end();
}


void bmpDraw(Adafruit_ILI9341 * tft, char * imagePath)
{
	HTTPClient http;
	http.begin(imagePath);

	Serial.print("Starting bmpDraw 1.01: ");
	Serial.print(imagePath);
	Serial.print("  -  ");
	uint32_t time = millis();
	int httpCode = http.GET();
	Serial.println(httpCode);
	if (httpCode > 0) {
		// HTTP header has been send and Server response header has been handled
		// file found at server
		if (httpCode == HTTP_CODE_OK) {
			Serial.printf("settings heap size: %u\n", ESP.getFreeHeap());

			unsigned long DrawTime = millis();
			// get lenght of document (is -1 when Server sends no Content-Length header)
			int len = http.getSize();
			unsigned char* data = new unsigned char[len]; 
			unsigned char tmp;
			Serial.printf("settings heap size: %u\n", ESP.getFreeHeap());
			Serial.print("  length = ");
			Serial.println(len);
			WiFiClient * stream = http.getStreamPtr();

			Serial.println("Step 1");
			int thisBytesAvailable = stream->available();
			Serial.print("thisBytesAvailable = ");
			Serial.print(thisBytesAvailable);
			int thisResult = 0;
			Serial.println("Step 2");
			Serial.printf("settings heap size: %u\n", ESP.getFreeHeap());
			thisResult = stream->read(data, thisBytesAvailable);
			int i = thisResult;
			Serial.print("Initial Read = ");
			Serial.println(i);


			const int HEADER_SIZE = 54;
			// stream->read(info, HEADER_SIZE); // read the 54-byte header

									// extract image height and width from header
			int bfOffBits = read32(data, 10); //  *(int*)&data[10]; // typically 1078 

			int biSize = read32(data , 14); // Specifies the size of the BITMAPINFOHEADER structure, in bytes. (typically 40 bytes)
			int biWidth = read32(data, 18); // *(int*)data + 18;
			int biHeight = read32(data, 22); // *(int*)data + 22;
			int biBitCount = read32(data, 28); // Specifies the number of bits per pixel.
			int biSizeImage = read32(data, 34); // *(int*)data + 34;

			Serial.print("filelen = ");
			Serial.println(len);

			Serial.print("biSize = ");
			Serial.println(biSize);
			Serial.print("Offset = "); // Specifies the offset from the beginning of the file to the bitmap data.
			Serial.println(bfOffBits);


			Serial.print("biSize = "); // Specifies the size of the BITMAPINFOHEADER structure, in bytes.
			Serial.println(biSize);
			Serial.print("biWidth = ");
			Serial.println(biWidth);
			Serial.print("biHeight = ");
			Serial.println(biHeight);
			Serial.print("biBitCount = ");
			Serial.println(biBitCount);
			Serial.print("biSizeImage = ");
			Serial.println(biSizeImage);

			int size = 3 * biWidth * biHeight;

			int maxWait = 100;
			int thisWait = 0;

			//int thisBytesRead = HEADER_SIZE;
			Serial.print("Initial bytes available: ");
			Serial.println(thisBytesAvailable);
			while (i < len) {
				thisBytesAvailable = stream->available();
				if ((i < len) && (thisBytesAvailable == 0)) {
					yield();
					thisBytesAvailable = stream->available();
				}
				if (thisBytesAvailable > 0) {
					Serial.print("Reading.... ");
					yield();
					thisResult = stream->read(data + i, thisBytesAvailable);
					i += thisResult;
					Serial.print(thisResult);
					Serial.println(" bytes. Done.");
					delay(1);
				}
			}
			Serial.print("Render! i = ");
			Serial.println(i);
			if (biBitCount == 1) {
				Serial.println("B/W not rendered");
				tft->println("B/W not rendered");
			} // BW
			else if (biBitCount == 4) {
				Serial.println("4 bit not rendered");
			} // 4 - bit
			else if (biBitCount ==8) { // (((biWidth * biHeight) + HEADER_SIZE) == len) {
				Serial.println("8 bit pic");
				tft->println("8-bit not rendered");
				for (int i = 0; i < biHeight; i++)
					{
						thisWait = 0;
						yield();
						//while ((stream->available() == 0) && (thisWait++ < maxWait)) {
						//	Serial.print(".");
						//	delay(10);
						//}
						for (int j = 0; j < biWidth; j++)
						{
							tft->drawPixel(j, i, data[bfOffBits + biWidth * i + j]);
						}
					}
				} // 8 - bit

			else if (biBitCount == 24) {
				Serial.println("bmp24");
				//int count = bfOffBits;
				int count = 0; // note that we start at the END of the data [biSizeImage - count++]  and work ourselves to the beginning of the data
				int extra = biWidth % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
				//for (int i = 0; i < biHeight; i++) // rows of data make up height
				for (int i = biHeight -1; i >= 0; i--) // rows of data make up height
				{
					count += extra;
					yield();
					uint8_t r; uint8_t g; uint8_t b;

					//for (int j = 0; j <= biWidth - 1; j++) // renders reverse image
					for (int j = biWidth - 1; j >=0; j--)
						{
						for (int k = 0; k < 3; k++) {
							switch (k) {
							case 0:
								 r= data[len - ++count];
								break;
							case 1:
								 g = data[len - ++count];
								break;
							case 2:
								 b = data[len - ++count];
								break;
							}
						}
						// Convert (B, G, R) to (R, G, B)
						//tmp = data[j];
						//data[j] = data[j + 2];
						//data[j + 2] = tmp;
						// int thisIndex = bfOffBits + biWidth * i + j;
						tft->drawPixel(i, j, tft->color565(r, g, b));
					}
				}
				delay(2000000);
			} // 24 bit

			else if (biBitCount == 32) {
				Serial.println("bmp32 - not currently decoding properly. cannot simply ignore alpha");
				//int count = bfOffBits;
				int count = 0; // note that we start at the END of the data [biSizeImage - count++]  and work ourselves to the beginning of the data
				int extra = 0; // biWidth % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
										 //for (int i = 0; i < biHeight; i++) // rows of data make up height
				Serial.print("extra=");
				Serial.println(extra);
				for (int i = len-1; i > len - 20; i--) {
					Serial.print(data[i], HEX);
					Serial.print(" ");
				}
				Serial.println();
				for (int i = biHeight - 1; i >= 0; i--) // rows of data make up height
				{
					//count += extra;
					yield();
					uint8_t r; uint8_t g; uint8_t b; uint8_t a;

					//for (int j = 0; j <= biWidth - 1; j++) // renders reverse image
					for (int j = biWidth - 1; j >= 0; j--)
					{
						for (int k = 0; k < 4; k++) {
							switch (k) {
							case 0:
								a = data[len - ++count]; // this is alpha, which we we simply ignore
								break;
							case 1:
								r = data[len - ++count];
								break;
							case 2:
								g = data[len - ++count];
								break;
							case 3:
								b = data[len - ++count];
								break;
							}
							Serial.print(r, HEX);
							Serial.print(" ");
							Serial.print(g, HEX);
							Serial.print(" ");
							Serial.print(b, HEX);
							Serial.print(" ");
							Serial.print(a, HEX);
							Serial.println();

						}
						// Convert (B, G, R) to (R, G, B)
						//tmp = data[j];
						//data[j] = data[j + 2];
						//data[j + 2] = tmp;
						// int thisIndex = bfOffBits + biWidth * i + j;
						tft->drawPixel(i, j, tft->color565(r, g, b));
					}
					Serial.println(len - count);
				}
				delay(2000000);
			} // 32 bit

			else {
				Serial.print("Unsupported bit depth: ");
				Serial.println(biBitCount);
				tft->print("Unsupported bit depth: ");
				tft->println(biBitCount);
			}



			stream->flush();
			stream->stopAll();

		}
	}
	Serial.print(millis() - time, DEC);
	Serial.println(" ms");
	http.end();
}


void bmpDraw2(Adafruit_ILI9341 * tft, char * imagePath)
{
	HTTPClient http;
	http.begin(imagePath);

	Serial.print("Starting bmpDraw: ");
	Serial.print(imagePath);
	Serial.print("  -  ");
	uint32_t time = millis();
	int httpCode = http.GET();
	Serial.println(httpCode);
	if (httpCode > 0) {
		// HTTP header has been send and Server response header has been handled
		// file found at server
		if (httpCode == HTTP_CODE_OK) {
			unsigned long DrawTime = millis();
			// get lenght of document (is -1 when Server sends no Content-Length header)
			int len = http.getSize();
			Serial.print("  length = ");
			Serial.println(len);
			WiFiClient * stream = http.getStreamPtr();
			bmpReadHeader(stream);
			for (int i = 61; i >= 0; i--)
			{
				//Serial.println("i"); delay(100);
				for (int j = 0; j<137; j++)
				{
					//Serial.println("j"); delay(100);
					size_t size = stream->available();
					//Serial.println("size=" + String(size)); yield(); delay(1);
					if (size) {
						yield();
						//Serial.println("go"); delay(100);
						uint8_t b = stream->read();
						uint8_t g = stream->read();
						uint8_t r = stream->read();
						// Serial.println("draw"); delay(100);
						tft->drawPixel(j, i, tft->color565(r, g, b));
					}
				}
			}
			//stream->flush();
			//stream->stopAll();

		}
	}
	Serial.print(millis() - time, DEC);
	Serial.println(" ms");
	http.end();
}


void bmpDraw3(Adafruit_ILI9341 * tft, char * imagePath)
{
	HTTPClient http;
	http.begin(imagePath);

	Serial.print("Starting bmpDraw: ");
	Serial.print(imagePath);
	Serial.print("  -  ");
	uint32_t time = millis();
	int httpCode = http.GET();
	Serial.println(httpCode);
	if (httpCode > 0) {
		// HTTP header has been send and Server response header has been handled
		// file found at server
		if (httpCode == HTTP_CODE_OK) {
			unsigned long DrawTime = millis();
			// get lenght of document (is -1 when Server sends no Content-Length header)
			int len = http.getSize();
			Serial.print("  length = ");
			Serial.println(len);
			WiFiClient * stream = http.getStreamPtr();
			bmpReadHeader(stream);
			for (int i = 61; i >= 0; i--)
			{
				//Serial.println("i"); delay(100);
				for (int j = 0; j<137; j++)
				{
					//Serial.println("j"); delay(100);
					size_t size = stream->available();
					//Serial.println("size=" + String(size)); yield(); delay(1);
					if (size) {
						yield();
						//Serial.println("go"); delay(100);
						uint8_t b = stream->read();
						uint8_t g = stream->read();
						uint8_t r = stream->read();
						// Serial.println("draw"); delay(100);
						tft->drawPixel(j, i, tft->color565(r, g, b));
					}
				}
			}
			//stream->flush();
			//stream->stopAll();

		}
	}
	Serial.print(millis() - time, DEC);
	Serial.println(" ms");
	http.end();
}

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
void dldDImage2(Adafruit_ILI9341 * tft, uint16_t  xloc, uint16_t yloc) {
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
	//char * imagePath = "http://beanstalk.azurewebsites.net/newimage.bin";
	char * imagePath = "http://healthagency.slocounty.ca.gov/macVPN/newimage.bmp";
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
				Serial.println("size=" + String(size)); yield(); delay(1);
				if (size) {
					// read up to 128 byte
					buffidx = 0;
					int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
					// Uncomment Below to send raw output it to Serial 
					// Serial.write(buff, c); yield(); delay(1);
					while (buffidx < c) {

						for (int y = 0; y < SCREEN_HEIGHT; y++) {
							uint16_t buf[SCREEN_WIDTH];
							for (int x = SCREEN_WIDTH - 1; x >= 0; x--) {
								byte l = stream->read();
								byte h = stream->read();
								buf[x] = ((uint16_t)h << 8) | l;
							}
							tft->setCursor(0, y);
							//tft->(buf);
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
			x = 0; y = 0;
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
	//char * imagePath = "http://beanstalk.azurewebsites.net/newimage.bin";
	char * imagePath = "http://gojimmypi-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=image.png&newImageSizeY=320";

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
				Serial.println("size="+String(size)); yield(); delay(1);
				if (size) {
					// read up to 128 byte
					buffidx = 0;
					int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
					// Uncomment Below to send raw output it to Serial 
					// Serial.write(buff, c); yield(); delay(1);

					while (buffidx < c) {
						// Strip out info not part of picture Steam could be multiple packets 
						// each packet has size data that needs to be striptied out or it will mess up picture 
						// also we need to know when data occurs next packet to strip out
						if (firstpacket == 0 || buffcnt > (BytesToRead2 - 1)) {
							int rflag = 0;
							int hcnt = 0;
							Serial.print("<New Packet>"); yield(); delay(1);
							while (rflag == 0) {
								cv = buff[buffidx++];
								Serial.write(cv); yield(); delay(1);
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
								Serial.println("Width=" + String(iwidth)); yield(); delay(1);
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
								Serial.println("Height=" + String(iheight)); yield(); delay(1);
								buffcnt = buffcnt + 2;
							}
							firstpacket = 1;
							Serial.println("Bytes to next packet=" + String(BytesToRead1)); yield(); delay(1);
							BytesToRead2 = BytesToRead1;
							Serial.println("</New Packet!!>");

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


// reverse endian (?)
//uint16_t read16(WiFiClient * f) {
//
//	uint16_t result;
//	((uint8_t *)&result)[1] = f->read(); // MSB
//	((uint8_t *)&result)[0] = f->read(); // LSB
//	return result;
//}
//
//uint32_t read32(WiFiClient * f) {
//	uint32_t result;
//	((uint8_t *)&result)[3] = f->read(); // MSB
//	((uint8_t *)&result)[2] = f->read();
//	((uint8_t *)&result)[1] = f->read();
//	((uint8_t *)&result)[0] = f->read(); // LSB
//	return result;
//}

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



