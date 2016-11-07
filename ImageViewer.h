// ImageViewer.h

#ifndef _IMAGEVIEWER_h
#define _IMAGEVIEWER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Adafruit_ILI9341.h"  // setup via Arduino IDE; Sketch - Include Library - Manage Libraries; Adafruit ILI9341

int ah2i(uint8_t s);
void dldDImage(Adafruit_ILI9341 * tft, uint16_t  xloc, uint16_t yloc);
 

#endif

