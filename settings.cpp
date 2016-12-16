// 
// 
// 

#include "settings.h"

struct SettingsStruct settings =
{
	//Default settings. Will be overwritten if valid values exists in EEPROM
	CONFIG_VERSION,
	false,    //APmode
	"SSID",//SSID
	"Password",  //Paswword
	"wifilcd",//mDNS name
	true,     //RunLED
	true,     //SerDebug
	true,     //LCDdebug
	1024,     //backlight
	true,     //PUT
	false,    //Autoget
	60,       //GET timer
	"http://url.to/file.bmp" //GET URL
};

void initConfig()
{
	EEPROM.begin(512);
	loadConfig();
}

void loadConfig()
{
	// To make sure there are settings, and they are OURS!
	// If nothing is found it will use the default settings.
	if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
		EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
		EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2])
		for (unsigned int t = 0; t<sizeof(settings); t++)
			*((char*)&settings + t) = EEPROM.read(CONFIG_START + t);
}

void saveConfig()
{
	for (unsigned int t = 0; t<sizeof(settings); t++)
		EEPROM.write(CONFIG_START + t, *((char*)&settings + t));
	EEPROM.commit();
}




