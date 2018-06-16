// ImageViewer.h

#ifndef _IMAGEVIEWER_h
#define _IMAGEVIEWER_h

#undef FOUND_BOARD
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#define FOUND_BOARD ESP8266
#endif

#ifdef ARDUINO_ARCH_ESP32
#include <HTTPClient.h>
#include <WiFi.h>
#define FOUND_BOARD ESP32
#endif

#ifndef FOUND_BOARD
#pragma message(Reminder "Error Target hardware not defined !")
#endif // ! FOUND_BOARD


#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Adafruit_ILI9341.h"  // setup via Arduino IDE; Sketch - Include Library - Manage Libraries; Adafruit ILI9341

// note the default "width" is the short dimension. We use the display rotated 90 degrees.
// but width and height are determined AFTER screen rotation is set!
// note we usually display pictures with setRotation(2); (long side down)

#include "tftHelper.h"


int ah2i(uint8_t s);
void imageViewDelay();
void dldDImage(Adafruit_ILI9341 * tft, uint16_t  xloc, uint16_t yloc);

/// <summary>
/// Display the BMP image at imageUrl on the tft ILI9341 
/// </summary>
/// <remarks>
/// imagePath is an HTTP (not HTTPS) url that points to a BMP source image.
/// see https://github.com/gojimmypi/imageConvert2BMP for runtime conversions
/// </remarks>
void bmpDraw(Adafruit_ILI9341 * tftPtr, char * imagePath);

void bmpDrawFromUrlStream(Adafruit_ILI9341 * tftPtr, String imageUrl, int startX = 0, int startY = 0);

#endif

