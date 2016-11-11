// debughandler.h

#ifndef _DEBUGHANDLER_h
#define _DEBUGHANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#ifndef debughandler_h
#define debughandler_h
#include <ESP8266WiFi.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define YMAX 310

void debug_init(void);
void debug_handleTelnet();
void debugPrint(const char*, unsigned int color = ILI9341_WHITE);
void debugPrintln(const char*, unsigned int color = ILI9341_WHITE);

#endif




#endif

