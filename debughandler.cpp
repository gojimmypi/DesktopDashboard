// 
// 
// 


#include "debughandler.h"
#include "settings.h"
#include <Arduino.h>
//#include <String>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

extern struct SettingsStruct settings;
extern Adafruit_ILI9341 tft;

void debug_init()
{

}

void debugPrint(const char* str, unsigned int color)
{
	if (settings.SerDebug == true)
		Serial.print(str);
	if (settings.LcdDebug) {
		if (tft.getCursorY() > YMAX)
			tft.setCursor(0, 0);
		tft.setTextSize(2);
		tft.setTextColor(color, ILI9341_BLACK);
		tft.print(str);
	}
}

void debugPrintln(const char* str, unsigned int color)
{
	if (settings.SerDebug)
		Serial.println(str);
	if (settings.LcdDebug) {
		if (tft.getCursorY() > YMAX)
			tft.setCursor(0, 0);
		tft.setTextSize(2);
		tft.setTextColor(color, ILI9341_BLACK);
		tft.println(str);
	}
}

