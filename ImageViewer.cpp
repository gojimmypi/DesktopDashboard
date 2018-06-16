// ImageViewer.h
// 
// 
// 

#include "ImageViewer.h"




#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "htmlHelper.h" // note use of define USE_TLS_SSL here

#define IMAGEDEBUG true // define image debug to send processing data to serial port
#define IMAGEDEBUG1 true // define even more image debug to send processing data to serial port
#define IMAGEDEBUG2 true // define maximum image debug to send processing data to serial port

const int ESP_MIN_HEAP = 2048; // if the free heap space every falls below this number, abort.

//
// code based on ESP8266 WiFi Image Viewer by James Eckert - http://jeplans.com/default.php?targp=ESP2Electronics2
//  (although I was never able to get that PHP conversion to work, so I wrote my own in ASP.Net)
// see https://github.com/gojimmypi/imageConvert2BMP

// Other resources that I found helpful:
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

//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
void imageViewDelay() {
	Serial.print("Waiting.");
	delay(1000); Serial.print(".");
	delay(1000); Serial.print(".");
	delay(1000); Serial.print(".");
	delay(1000); Serial.print(".");

}

//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
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


//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
#ifdef USE_TLS_SSL
#else
#endif

uint16_t read16(WiFiClient * f) {

	uint16_t result;
	((uint8_t *)&result)[0] = f->read(); // LSB
	((uint8_t *)&result)[1] = f->read(); // MSB
	return result;
}

//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
#ifdef USE_TLS_SSL
#else
#endif
uint32_t read32(WiFiClient * f) {
	uint32_t result;
	((uint8_t *)&result)[0] = f->read(); // LSB
	((uint8_t *)&result)[1] = f->read();
	((uint8_t *)&result)[2] = f->read();
	((uint8_t *)&result)[3] = f->read(); // MSB
	return result;
}

//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
uint16_t read16(unsigned char * d, int fromIndex) {

	uint16_t result;
	((uint8_t *)&result)[0] = d[fromIndex]; // LSB
	((uint8_t *)&result)[1] = d[fromIndex + 1]; // MSB
	return result;
}

//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
uint32_t read32(unsigned char * d, int fromIndex) {
	uint32_t result;
	((uint8_t *)&result)[0] = d[fromIndex]; // LSB
	((uint8_t *)&result)[1] = d[fromIndex + 1];
	((uint8_t *)&result)[2] = d[fromIndex + 2];
	((uint8_t *)&result)[3] = d[fromIndex + 3]; // MSB
	return result;
}


//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
#ifdef USE_TLS_SSL
#else
#endif
unsigned char* bmpRawReader(WiFiClient * f) {
	// based on code from http://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file
	uint8_t info[54];

	f->readBytes(info, 54); // read the 54-byte header

							// extract image height and width from header
	int width = *(int*)&info[18];  // compiler complains: warning: dereferencing type-punned pointer will break strict-aliasing rules [-Wstrict-aliasing]
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

//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
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



int len = 1; // we'll determine to total length of the image byte stream
unsigned char* pImageBMP;
int currentStreamPosition = 0;
int currentStreamPayloadPosition = 0;  // there are often chunks of data returned in separate "payloads"
int totalBytesRead = 0;
bool abortMidStream = false;


// ***************************************************************************************************************************************************************
//  byteInStream - forward-reading only; get byte at [position] in a stream of web data. if it is not currently loaded into memory,
//                 discard prior chunk of data and read the next one until found. (works best when incrementing by 1 only!)
//
// TODO - add maximum amount of time to read before giving up
// ***************************************************************************************************************************************************************
#ifdef USE_TLS_SSL
uint8_t byteInStream(WiFiClient * stream, int position) { // TODO should this be WiFiClientSecure ? (the problem here is that there's no compatible http.getStreamPtr();
#else
uint8_t byteInStream(WiFiClient * stream, int position) {
#endif
	uint32_t thisPayloadBytesAvailable = 0;
	int thisPayloadByteCount = 0;
	int thisAttemptCount = 0;
	boolean foundPayload = false;
	abortMidStream = false; // if we lose our connection mid-stream, we may need to abort

	if (position > totalBytesRead) {
		// we only ever do anything here if the requested position in stream is greater than the total bytes we've read

		int startMillis = millis();

#ifdef IMAGEDEBUG
		Serial.print("byteInStream - reading next segment position ");
		Serial.print(position);
		Serial.print(" at time: "); 
		Serial.print(startMillis);
		Serial.print("; len =");
		Serial.println(len);
		yield();
#endif

		while ((totalBytesRead < len) && (!foundPayload)) {
			thisPayloadBytesAvailable = stream->available();
			if ((totalBytesRead < len) && (thisPayloadBytesAvailable == 0)) {
				yield();
				thisPayloadBytesAvailable = stream->available();
			}

			if (thisPayloadBytesAvailable > 0) {
				if (thisPayloadBytesAvailable < ESP.getFreeHeap() - ESP_MIN_HEAP) {
					delete pImageBMP; // without deleting old data, we run out of heap space!
					pImageBMP = new unsigned char[thisPayloadBytesAvailable]; // allocation of just enough memory for this incremenal payload of data
				}
				else {
					abortMidStream = true;
					Serial.println("Out of heap space while rendering!");
					break; // abort while loop when out of space
				}
				//Serial.print("Reading.... ");
				yield();
				
				//thisPayloadByteCount = stream->read(data + totalBytesRead, thisPayloadBytesAvailable); // the old code appended to one big data array

				if (stream->connected()) {
					thisPayloadByteCount = stream->read(pImageBMP, thisPayloadBytesAvailable);
					currentStreamPosition = totalBytesRead + 1; // the prior [total bytes read] + 1 is the new stream position
					totalBytesRead += thisPayloadByteCount; // this is the ending position of the stream payload we now have
															//Serial.print(thisPayloadByteCount);
															//Serial.println(" bytes. Done.");
															//Serial.printf("settings heap size: %u\n", ESP.getFreeHeap());
															//Serial.print("  total read = ");
															//Serial.println(totalBytesRead);
					foundPayload = true; // once we find and read the next payload, we exit the while loop 
				}
				else {
					Serial.println("Error: byteInStream called with disconnected stream!");
					yield();
					abortMidStream = true;
					break; // abort while loop when not connected
				}
			}
			else {
				yield();
				delay(1);
				thisAttemptCount++;
				if (thisAttemptCount > 1000) {
					abortMidStream = true;
#ifdef IMAGEDEBUG
					Serial.println("No payload available for more than 1000 attempts for stream.available!");
#endif
					break; // give up trying to read
				}
			}
		}
		//typically less than 1 milis to read

#ifdef IMAGEDEBUG
		Serial.print("done reading. ");
		Serial.print(millis() - startMillis);
		Serial.print(" millis to read. Available wait loops: ");
		Serial.println(thisAttemptCount);
#endif

	}
	else {
		// TODO - allow bi-directional reading? we'd likely need to start all over at the beginning. probably not the most efficient...
	}


	if (abortMidStream) {
#ifdef  IMAGEDEBUG 
		Serial.println("Error: byteInStream aborted mid-stream, returning NAN!");
#endif //  IMAGEDEBUG 

		return NAN; // we have no data
	}
	else
	{
		int thisPayloadPosition = position - currentStreamPosition + 1;
		if (thisPayloadPosition >= 0) {
			return pImageBMP[position - currentStreamPosition + 1]; // TODO, reference position in payload only, not full stream
		}
		else {
			return (uint8_t)0;
#ifdef IMAGEDEBUG
			Serial.println("Error: requested position not currently in memory (currently supporting forward-reading only)");
#endif
		} // else requested position is not in memory, not already processed, and stream cannot be reversed
	} // else not aborting mid-stream


}

// ***************************************************************************************************************************************************************
//  screenIsValidX - is a given X-ccordinate between 0 and [SCREEN_WIDTH - 1] ?
// ***************************************************************************************************************************************************************
int screenIsValidX(int thisX) {
	return ( (thisX < SCREEN_WIDTH) && (thisX >= 0) ? true : false);
}

// ***************************************************************************************************************************************************************
//  screenIsValidY - is a given Y-ccordinate between 0 and [SCREEN_HEIGHT - 1] ?
// ***************************************************************************************************************************************************************
int screenIsValidY(int thisY) {
	return ((thisY < SCREEN_HEIGHT) && (thisY >= 0) ? true : false);
}

// ***************************************************************************************************************************************************************
//  screenSafeX: ensure requested X-coordinate lands on screen, based on a resepctive starting offset value
// ***************************************************************************************************************************************************************
int screenSafeX(int thisX, int startX) {
	if (isnan(thisX) || isnan(startX)) return 0;

	int resultX = thisX + startX;
	if (screenIsValidX(resultX)) {
		return resultX;
	}
	else if (resultX < 0) {
		return 0;
	}
	else if (resultX > SCREEN_WIDTH) {
		return SCREEN_WIDTH;
	}
	else {
		return 0; // we'll likely never end up here. this is just to appease compiler warning. wanted screenIsValidY listed first for performance
	}
}

// ***************************************************************************************************************************************************************
//  screenSafeY: ensure requested Y-coordinate lands on screen, based on a resepctive starting offset value
// ***************************************************************************************************************************************************************
int screenSafeY(int thisY, int startY) {
	if (isnan(thisY) || isnan(startY)) return 0;

	int resultY = thisY + startY;
	if (screenIsValidY(resultY)) {
		return resultY;
	}
	else if (resultY < 0) {
		return 0;
	}
	else if (resultY > SCREEN_HEIGHT) {
		return SCREEN_HEIGHT;
	}
	else {
		return 0; // we'll likely never end up here. this is just to appease compiler warning. wanted screenIsValidY listed firsdt for performance
	}
}


// ***************************************************************************************************************************************************************
// ***************************************************************************************************************************************************************
//   bmpDrawFromUrlStream - get a BMP image at web location imageUrl, and draw it at an offset from (0,0) at (startX, startY)
//                          as many images will be bigger than all of available ESP8266 memory, we are rendering image as
//                          data is being read from the stream.
// ***************************************************************************************************************************************************************
// ***************************************************************************************************************************************************************
void bmpDrawFromUrlStream(Adafruit_ILI9341 * tftPtr, String imageUrl, int startX, int startY)
{
	pImageBMP = new unsigned char[len];  // we allocate / delete data as needed;this pointer to BMP image byte array contains only part of an image at any given time 
	uint32_t time = millis(); // we'll keep track of how long it takes to render

	Serial.print("Starting bmpDrawFromUrl 1.03: ");
	Serial.print(imageUrl);
	Serial.print("  -  ");

	currentStreamPosition = 0; // new images always start at byte position 0
	currentStreamPayloadPosition = 0;  // there are often chunks of data returned in separate "payloads"; this is the relative position in that payload
	totalBytesRead = 0; // how many bytes of total image size have been read ant any given time.
	len = 1; // total length of image data, including all expected chunks of data in all payloads read

	HTTPClient http;
	http.begin(imageUrl);

	int httpCode = http.GET();
	//if (!http.connected()) {
	//	Serial.println("HTTPClient not connected. Aborting bmpDrawFromUrl");
	//	return;
	//}
	if (httpCode > 0) {
		// HTTP header has been send and Server response header has been handled
		// file found at server
		if (httpCode == HTTP_CODE_OK) {
			Serial.printf("Initial heap size: %u\n", ESP.getFreeHeap());

			unsigned long DrawTime = millis();
			// get length of document (is -1 when Server sends no Content-Length header)
			len = http.getSize(); // TODO - this is no longer needed locally
//			unsigned char tmp;
			Serial.printf("settings heap size: %u\n", ESP.getFreeHeap());
			Serial.print("  length = ");
			Serial.println(len);

#ifdef USE_TLS_SSL
			WiFiClient * stream = http.getStreamPtr();
#else
			WiFiClient * stream = http.getStreamPtr();
#endif
			
			unsigned char testChar = byteInStream(stream, 1); // read the first chunk of data. TODO ensure we read at least 54 bytes of header.

			Serial.print("Step 1 ");
			Serial.println(testChar);
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


			// const int HEADER_SIZE = 54;
			// stream->read(info, HEADER_SIZE); // read the 54-byte header

			// extract image height and width from header
			int bfOffBits = read32(pImageBMP, 10); //  *(int*)&data[10]; // typically 1078 

			int biSize = read32(pImageBMP, 14); // Specifies the size of the BITMAPINFOHEADER structure, in bytes. (typically 40 bytes)
			int biWidth = read32(pImageBMP, 18); // *(int*)data + 18;
			int biHeight = read32(pImageBMP, 22); // *(int*)data + 22;
			int biBitCount = read32(pImageBMP, 28); // Specifies the number of bits per pixel.
			int biSizeImage = read32(pImageBMP, 34); // *(int*)data + 34;

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

			// int size = 3 * biWidth * biHeight;

			// int maxWait = 100;
			// int thisWait = 0;

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
			
			// ***************************************************************************************************************************************************************
			//  
			// ***************************************************************************************************************************************************************
			if (biBitCount == 1) {
				Serial.println("B/W not rendered");
				tftPtr->println("B/W not rendered");
			} // BW

			  // ***************************************************************************************************************************************************************
			  //  
			  // ***************************************************************************************************************************************************************
			else if (biBitCount == 4) {
				Serial.println("4 bit not rendered");
			} // 4 - bit
			
			  // ***************************************************************************************************************************************************************
			  //  
			  // ***************************************************************************************************************************************************************
			else if (biBitCount == 8) { // (((biWidth * biHeight) + HEADER_SIZE) == len) {
				Serial.println("8 bit pic");
				tftPtr->println("8-bit not rendered");
				for (int i = 0; i < biHeight; i++)
				{
					// thisWait = 0;
					yield();
					//while ((stream->available() == 0) && (thisWait++ < maxWait)) {
					//	Serial.print(".");
					//	delay(10);
					//}
					for (int j = 0; j < biWidth; j++)
					{
						tftPtr->drawPixel(screenSafeX(i, startX), screenSafeY(j, startY), pImageBMP[bfOffBits + biWidth * i + j]);
					}
				}
			} // end of 8 - bit

			  // ***************************************************************************************************************************************************************
			  // 24 bit BMP handler
			  // ***************************************************************************************************************************************************************
			else if (biBitCount == 24) {
				Serial.println("Rendering 24 bit bmpDrawFromUrl");
				//int count = bfOffBits;
				int count = 0; // note that we start at the START of the data [biSizeImage - count++]  and work ourselves to the beginning of the data
				int extra = biWidth % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
										 //for (int i = 0; i < biHeight; i++) // rows of data make up height
				for (int i = biHeight - 1; i >= 0; i--) // rows of data make up height
				{
					count += extra;
					yield();
					uint8_t r = 0; uint8_t g = 0; uint8_t b = 0;

					//for (int j = 0; j <= biWidth - 1; j++) // renders reverse image
					for (int j = biWidth - 1; j >= 0; j--)
					{
						for (int k = 0; k < 3; k++) {
							switch (k) {
							case 0:
								b = byteInStream(stream, count++); // r = data[len - ++count];
								break;
							case 1:
								g = byteInStream(stream, count++); //  pImageBMP[len - ++count];
								break;
							case 2:
								r = byteInStream(stream, count++); // b = data[len - ++count];
								break;
							}
						}
						if (abortMidStream) { break; }
						tftPtr->drawPixel(screenSafeX(i, startX), screenSafeY(j, startY), tftPtr->color565(r, g, b));
					}
					if (abortMidStream) { break; }
				}
			} // 24 bit

			  // ***************************************************************************************************************************************************************
			  // 32 bit BMP handler
			  // ***************************************************************************************************************************************************************
			else if (biBitCount == 32) {
				Serial.println("Rendering 32 bit bmpDrawFromUrl");
				int count = bfOffBits; // note that we start at the START of the data [biSizeImag]  and work ourselves to the END of the data
				//int extra = 0; // The nubmer of bytes in a row (cols) will already be a multiple of 4. (32 / 8) so we don'n need to pad with extra bytes as done in 24 bit

				// show the first chunk of data 
				//for (int j = 0; j < 66; j++) {
				//	for (int i = 0; i < 16; i++) {
				//		Serial.print(data[(j * 16) + i], HEX);
				//		Serial.print(" ");
				//	}
				//	Serial.println();
				//}
				//
				
				// scan rows of data, starting at bottom of the image, working our way up 
				for (int i = 0; i < biHeight; i++) // [biHeight] rows of data make up height of our image
				{
					//count += extra; // we don't need to pad to 4 byte boundries, as 32 bits is already 4 bytes!  (note difference from 24 bit rendering)
					yield(); // give the OS a bit of time after showing each row.
					uint8_t r = 0; uint8_t g = 0; uint8_t b = 0; uint8_t a = 0;

					// a loop to fetch a row of data from our picture
					for (int j = 0; j < biWidth; j++)
					{
						// a small loop to read 4 bytes in the stream; one pixel-worth of data to display
						for (int k = 0; k < 4; k++) { // there are 4 bytes per pixel: BGRA
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
							// show the pixel data (this can REALLY slow down a rendering, particularly for full screen)
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
						if (abortMidStream) { break; }
						// draw a single pixel of data from our byte stream
						tftPtr->drawPixel(screenSafeX(i, startX), screenSafeY(j, startY), tftPtr->color565(r, g, b));
					}
					// Serial.println("Current index: ");
					// Serial.println(len - count);
					if (abortMidStream) { break; }
				}
			} // 32 bit

			  // ***************************************************************************************************************************************************************
			  //  
			  // ***************************************************************************************************************************************************************
			else {
				Serial.print("Unsupported bit depth: ");
				Serial.println(biBitCount);
				tftPtr->print("Unsupported bit depth: ");
				tftPtr->println(biBitCount);
			}
			stream->flush();

#ifdef ARDUINO_ARCH_ESP8266
			stream->stopAll(); // flush client (only ESP8266 seems to have implemented stopAll)
#endif

#ifdef ARDUINO_ARCH_ESP32
			stream->stop(); // flush client (the ESP32 does not seem to have implemented stopAll)
#endif
			delete pImageBMP; // once we process the data, we're done with it.
		} // http file found
		else
		{ // show an HTTP error code
			Serial.print("HTTP ");
			Serial.println(httpCode);
		}
	}

	http.end();
	Serial.print(millis() - time, DEC);
	Serial.println(" ms");
}

// bmpDraw - load entire image into memory, then render it
// ***************************************************************************************************************************************************************
//  
// ***************************************************************************************************************************************************************
void bmpDraw(Adafruit_ILI9341 * tftPtr, char * imagePath)
{
	uint32_t time = millis();
	Serial.print("Starting bmpDraw 1.02: ");
	Serial.print(imagePath);
	Serial.print("  -  ");

	HTTPClient http;
	http.begin(imagePath);

	int httpCode = http.GET();
	if (httpCode > 0) {
		// HTTP header has been sent and Server response header has been handled
		// file found at server
		if (httpCode == HTTP_CODE_OK) {
			Serial.printf("settings heap size: %u\n", ESP.getFreeHeap());

			//unsigned long DrawTime = millis();
			// get lenght of document (is -1 when Server sends no Content-Length header)
			uint32_t len = http.getSize();

			if (len < 1) {
				Serial.println("Error: Bad content length.");
				Serial.println();
				Serial.println();
				return;
			}
			if (len < (ESP.getFreeHeap() - ESP_MIN_HEAP)) {
				pImageBMP = new unsigned char[len]; // allocation of just enough memory for this incremenal payload of data
			}
			else {
				Serial.println("Not enough heap space while rendering!\n\r (try bmpDrawFromUrlStream, instead).");
				Serial.print(ESP.getFreeHeap());
				Serial.println(" heap bytes free.");
				Serial.print(ESP_MIN_HEAP);
				Serial.println(" minimum bytes free configured.");
				Serial.print(len);
				Serial.println(" bytes needed for this image.");
				Serial.println();
				Serial.println();
				return;
			}
			//unsigned char tmp;
			Serial.printf("After allocation heap size: %u\n", ESP.getFreeHeap());
			Serial.print("  length = ");
			Serial.println(len);

#ifdef USE_TLS_SSL
#else
#endif
			WiFiClient * stream = http.getStreamPtr();

			Serial.println("Step 1");
			int thisBytesAvailable = stream->available();
			Serial.print("thisBytesAvailable = ");
			Serial.print(thisBytesAvailable);
			uint32_t thisResult = 0;
			Serial.println("Step 2");
			Serial.printf("settings heap size: %u\n", ESP.getFreeHeap());
			thisResult = stream->read(pImageBMP, thisBytesAvailable);
			uint32_t i = thisResult;
			Serial.print("Initial Read = ");
			Serial.println(i);


			// const int HEADER_SIZE = 54;
			// stream->read(info, HEADER_SIZE); // read the 54-byte header

									// extract image height and width from header
			int bfOffBits = read32(pImageBMP, 10); //  *(int*)&data[10]; // typically 1078 

			int biSize = read32(pImageBMP , 14); // Specifies the size of the BITMAPINFOHEADER structure, in bytes. (typically 40 bytes)
			int biWidth = read32(pImageBMP, 18); // *(int*)data + 18;
			int biHeight = read32(pImageBMP, 22); // *(int*)data + 22;
			int biBitCount = read32(pImageBMP, 28); // Specifies the number of bits per pixel.
			int biSizeImage = read32(pImageBMP, 34); // *(int*)data + 34;

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

			int size = 3 * biWidth * biHeight; // for Serial.print info only
			Serial.print("computed size = ");
			Serial.println(size);

			// int maxWait = 100;
			// int thisWait = 0;

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
					//Serial.print("Reading.... ");
					yield();
					thisResult = stream->read(pImageBMP + i, thisBytesAvailable);
					i += thisResult;
					//Serial.print(thisResult);
					//Serial.println(" bytes. Done.");
					//delay(1);
				}
			}
			Serial.print("Render! i = ");
			Serial.println(i);

			// ***************************************************************************************************************************************************************
			//  
			// ***************************************************************************************************************************************************************
			if (biBitCount == 1) {
				Serial.println("B/W not rendered");
				tftPtr->println("B/W not rendered");
			} // BW

			  // ***************************************************************************************************************************************************************
			  //  
			  // ***************************************************************************************************************************************************************
			else if (biBitCount == 4) {
				Serial.println("4 bit not rendered");
			} // 4 - bit

			  // ***************************************************************************************************************************************************************
			  //  
			  // ***************************************************************************************************************************************************************
			else if (biBitCount == 8) { // (((biWidth * biHeight) + HEADER_SIZE) == len) {
				Serial.println("8 bit pic");
				tftPtr->println("8-bit not rendered");
				for (int i = 0; i < biHeight; i++)
				{
					// thisWait = 0;
					yield();
					//while ((stream->available() == 0) && (thisWait++ < maxWait)) {
					//	Serial.print(".");
					//	delay(10);
					//}
					for (int j = 0; j < biWidth; j++)
					{
						tftPtr->drawPixel(j, i, pImageBMP[bfOffBits + biWidth * i + j]);
					}
				}
			} // 8 - bit

			  // ***************************************************************************************************************************************************************
			  //  
			  // ***************************************************************************************************************************************************************
			else if (biBitCount == 24) {
				Serial.println("bmp24");
				//int count = bfOffBits;
				int count = 0; // note that we start at the END of the data [biSizeImage - count++]  and work ourselves to the beginning of the data
				int extra = biWidth % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
				uint8_t r; uint8_t g; uint8_t b;
				r = 0, g = 0, b = 0;

				//for (int i = 0; i < biHeight; i++) // rows of data make up height
				for (int i = biHeight -1; i >= 0; i--) // rows of data make up height
				{
					count += extra;
					yield();
					//for (int j = 0; j <= biWidth - 1; j++) // renders reverse image
					for (int j = biWidth - 1; j >=0; j--)
						{
						for (int k = 0; k < 3; k++) {
							switch (k) {
							case 0:
								 r= pImageBMP[len - ++count];
								break;
							case 1:
								 g = pImageBMP[len - ++count];
								break;
							case 2:
								 b = pImageBMP[len - ++count];
								break;
							}
						}
						tftPtr->drawPixel(i, j, tftPtr->color565(r, g, b)); 
					}
				}
			} // 24 bit

			  // ***************************************************************************************************************************************************************
			  //  
			  // ***************************************************************************************************************************************************************
			else if (biBitCount == 32) {
				Serial.println("bmpDraw render 32 bit");
				//int count = bfOffBits;
				int count = 0; // note that we start at the END of the data [biSizeImage - count++]  and work ourselves to the beginning of the data
				// int extra = 0; // biWidth % 4; // The nubmer of bytes in a row (cols) will be a multiple of 4.
										 //for (int i = 0; i < biHeight; i++) // rows of data make up height

				// preint a preview of data
				//for (int i = len-1; i > len - 20; i--) {
				//	Serial.print(pImageBMP[i], HEX);
			    // 		Serial.print(" ");
				//}
				//Serial.println();
				for (int i = biHeight - 1; i >= 0; i--) // rows of data make up height
				{
					//count += extra;
					yield();
					uint8_t r = 0; uint8_t g = 0; uint8_t b = 0; uint8_t a = 0;

					//for (int j = 0; j <= biWidth - 1; j++) // renders reverse image
					for (int j = biWidth - 1; j >= 0; j--)
					{
						for (int k = 0; k < 4; k++) {
							switch (k) {
							case 0:
								a = pImageBMP[len - ++count]; // this is alpha, which we we simply ignore 
								                              // if this line is commented out, don't forgot to still increment [count]
								break;
							case 1:
								r = pImageBMP[len - ++count]; // red
								break;
							case 2:
								g = pImageBMP[len - ++count]; // green
								break;
							case 3:
								b = pImageBMP[len - ++count]; // blue
								break;
							}
							//Serial.print(r, HEX);
							//Serial.print(" ");
							//Serial.print(g, HEX);
							//Serial.print(" ");
							//Serial.print(b, HEX);
							//Serial.print(" ");
							//Serial.print(a, HEX);
							//Serial.println();

						}
						tftPtr->drawPixel(i, j, tftPtr->color565(r, g, b)); // show the pixel!
					}
				}
			} // 32 bit

			else {
				Serial.print("Unsupported bit depth: ");
				Serial.println(biBitCount);
				tftPtr->print("Unsupported bit depth: ");
				tftPtr->println(biBitCount);
			}

			stream->flush();

#ifdef ARDUINO_ARCH_ESP8266
			stream->stopAll(); // flush client (only ESP8266 seems to have implemented stopAll)
#endif

#ifdef ARDUINO_ARCH_ESP32
			stream->stop(); // flush client (the ESP32 does not seem to have implemented stopAll)
#endif
			delete pImageBMP;
		} // if httpcode = 200
		else {
			Serial.print("HTTP ");
			Serial.println(httpCode);
		}
	}
	Serial.print(millis() - time, DEC);
	Serial.println(" ms");
	http.end();
}


//void bmpDraw2(Adafruit_ILI9341 * tftPtr, char * imagePath)
//{
//	HTTPClient http;
//	http.begin(imagePath);
//
//	Serial.print("Starting bmpDraw: ");
//	Serial.print(imagePath);
//	Serial.print("  -  ");
//	uint32_t time = millis();
//	int httpCode = http.GET();
//	Serial.println(httpCode);
//	if (httpCode > 0) {
//		// HTTP header has been send and Server response header has been handled
//		// file found at server
//		if (httpCode == HTTP_CODE_OK) {
//			unsigned long DrawTime = millis();
//			// get lenght of document (is -1 when Server sends no Content-Length header)
//			int len = http.getSize();
//			Serial.print("  length = ");
//			Serial.println(len);
//			WiFiClient * stream = http.getStreamPtr();
//			bmpReadHeader(stream);
//			for (int i = 61; i >= 0; i--)
//			{
//				//Serial.println("i"); delay(100);
//				for (int j = 0; j<137; j++)
//				{
//					//Serial.println("j"); delay(100);
//					size_t size = stream->available();
//					//Serial.println("size=" + String(size)); yield(); delay(1);
//					if (size) {
//						yield();
//						//Serial.println("go"); delay(100);
//						uint8_t b = stream->read();
//						uint8_t g = stream->read();
//						uint8_t r = stream->read();
//						// Serial.println("draw"); delay(100);
//						tftPtr->drawPixel(j, i, tftPtr->color565(r, g, b));
//					}
//				}
//			}
//			//stream->flush();
//			//stream->stopAll();
//
//		}
//	}
//	Serial.print(millis() - time, DEC);
//	Serial.println(" ms");
//	http.end();
//}
//
//
//void bmpDraw3(Adafruit_ILI9341 * tftPtr, char * imagePath)
//{
//	HTTPClient http;
//	http.begin(imagePath);
//
//	Serial.print("Starting bmpDraw: ");
//	Serial.print(imagePath);
//	Serial.print("  -  ");
//	uint32_t time = millis();
//	int httpCode = http.GET();
//	Serial.println(httpCode);
//	if (httpCode > 0) {
//		// HTTP header has been send and Server response header has been handled
//		// file found at server
//		if (httpCode == HTTP_CODE_OK) {
//			unsigned long DrawTime = millis();
//			// get lenght of document (is -1 when Server sends no Content-Length header)
//			int len = http.getSize();
//			Serial.print("  length = ");
//			Serial.println(len);
//			WiFiClient * stream = http.getStreamPtr();
//			bmpReadHeader(stream);
//			for (int i = 61; i >= 0; i--)
//			{
//				//Serial.println("i"); delay(100);
//				for (int j = 0; j<137; j++)
//				{
//					//Serial.println("j"); delay(100);
//					size_t size = stream->available();
//					//Serial.println("size=" + String(size)); yield(); delay(1);
//					if (size) {
//						yield();
//						//Serial.println("go"); delay(100);
//						uint8_t b = stream->read();
//						uint8_t g = stream->read();
//						uint8_t r = stream->read();
//						// Serial.println("draw"); delay(100);
//						tftPtr->drawPixel(j, i, tftPtr->color565(r, g, b));
//					}
//				}
//			}
//			//stream->flush();
//			//stream->stopAll();
//
//		}
//	}
//	Serial.print(millis() - time, DEC);
//	Serial.println(" ms");
//	http.end();
//}
//
//void dldDImage2(Adafruit_ILI9341 * tftPtr, uint16_t  xloc, uint16_t yloc) {
//	uint16_t  r;
//	uint8_t hb, lb, cv, lv1, lv2, cb1, cb2;
//	uint16_t  frsize, iwidth, iheight;
//	int x = 0, y = 0;
//	int firstpacket = 0;
//	int buffcnt = 0;
//	int bytescolleted = 0;
//
//	HTTPClient http;
//
//	Serial.print("[HTTP] begin...\n");
//
//	// configure server and url
//	//char * imagePath = "http://beanstalk.azurewebsites.net/helloworld.png";
//	char * imagePath = "http://beanstalk.azurewebsites.net/newimage.bmp";
//	http.begin(imagePath);
//	Serial.print("[HTTP] GET...\n");
//	Serial.print(imagePath);
//	Serial.println();
//	// start connection and send HTTP header
//	int httpCode = http.GET();
//	if (httpCode > 0) {
//		// HTTP header has been send and Server response header has been handled
//		Serial.printf("[HTTP] GET... code: %d\n", httpCode);
//
//		// file found at server
//		if (httpCode == HTTP_CODE_OK) {
//			unsigned long DrawTime = millis();
//			// get lenght of document (is -1 when Server sends no Content-Length header)
//			int len = http.getSize();
//			// create buffer for read
//			uint8_t buff[2048] = { 0 };
//			int  buffidx = sizeof(buff);
//			// get tcp stream
//			WiFiClient * stream = http.getStreamPtr();
//			uint16_t BytesToRead1 = 0;
//			uint16_t BytesToRead2 = 8192;
//			// read all data from server
//			while (http.connected() && (len > 0 || len == -1)) {
//				// get available data size
//				size_t size = stream->available();
//				Serial.println("size=" + String(size)); yield(); delay(1);
//				if (size) {
//					// read up to 128 byte
//					buffidx = 0;
//					int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
//					// Uncomment Below to send raw output it to Serial 
//					// Serial.write(buff, c); yield(); delay(1);
//					while (buffidx < c) {
//
//						for (int y = 0; y < SCREEN_HEIGHT; y++) {
//							uint16_t buf[SCREEN_WIDTH];
//							for (int x = SCREEN_WIDTH - 1; x >= 0; x--) {
//								byte l = stream->read();
//								byte h = stream->read();
//								buf[x] = ((uint16_t)h << 8) | l;
//							}
//							tftPtr->setCursor(0, y);
//							//tft->(buf);
//						}
//
//
//					}
//					if (len > 0) {
//						len -= c;
//					}
//				}
//				delay(2);
//			}
//			Serial.println("y=" + String(y));
//			Serial.println("[HTTP] connection closed or file end.\n");
//			Serial.println(String(millis() - DrawTime));
//			tftPtr->fillRect(1, 180, 238, 70, 0x0000);
//			tftPtr->setCursor(15, 195);
//			tftPtr->setTextColor(ILI9341_RED);
//			tftPtr->println(String(millis() - DrawTime) + "ms");
//			buffcnt = 0;
//			firstpacket = 0;
//			x = 0; y = 0;
//			buffidx = 0;
//			Serial.println("Done! waiting...");
//			delay(5000);
//			//t2Time = millis();
//		}
//	}
//	else {
//		Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
//	}
//
//	http.end();
//	//HRequestsActive = 0;
//}
//

void dldDImage(Adafruit_ILI9341 * tftPtr, uint16_t  xloc, uint16_t yloc) {
	// this is the old, "load it all into memory first" method. Use bmpDrawFromUrlStream instead
	uint16_t  r;
	uint8_t hb, lb, cv, lv1 = 0, lv2 = 0, cb1, cb2;
	uint16_t  frsize, iwidth = 0, iheight = 0;
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

#ifdef USE_TLS_SSL
#else
#endif
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
								tftPtr->drawPixel(x + xloc, y + yloc, r);
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
			tftPtr->fillRect(1, 180, 238, 70, 0x0000);
			tftPtr->setCursor(15, 195);
			tftPtr->setTextColor(ILI9341_RED);  
			tftPtr->println(String(millis() - DrawTime) + "ms");
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
void bmpDraw(Adafruit_ILI9341 * tftPtr, uint8_t x, uint16_t y) {

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

	if ((x >= tftPtr->width()) || (y >= tftPtr->height())) return;

	Serial.print(F("Loading image '"));
	//Serial.print(filename);
	Serial.println('\'');

	uint32_t us;
	uint32_t total_seek = 0;
	uint32_t total_read = 0;
	uint32_t total_parse = 0;
	uint32_t total_draw = 0;

	HTTPClient http; // TODO WiFiClientSecure conversion

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

#ifdef USE_TLS_SSL
	WiFiClient * stream = http.getStreamPtr(); // TODO WiFiClientSecure conversion
#else
	WiFiClient * stream = http.getStreamPtr();
#endif

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
				if ((x + w - 1) >= tftPtr->width())  w = tftPtr->width() - x;
				if ((y + h - 1) >= tftPtr->height()) h = tftPtr->height() - y;

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
						awColors[col] = tftPtr->color565(r, g, b);
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



