// settings.h

#ifndef _SETTINGS_h
#define _SETTINGS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#ifndef settings_h
#define settings_h

#include <Arduino.h>
#include <inttypes.h>
#include <EEPROM.h>

#define CONFIG_VERSION "010"
#define CONFIG_START 0
#define FWVERSION "1.10"

struct SettingsStruct {
	// This is for detection if they are your settings
	char version[4];
	// The variables of your settings
	bool APmode;
	char ssid[32];
	char password[32];
	char name[32];
	bool runLED;
	bool SerDebug;
	bool LcdDebug;
	uint16_t backlight;
	bool allowPUT;
	bool autoGET;
	uint16_t getInterval;
	char url[64];
};

void initConfig(void);
void loadConfig(void);
void saveConfig(void);

#endif



#endif

