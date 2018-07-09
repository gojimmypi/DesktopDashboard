// 
// 
// 

#include "hardwareHelper.h"


void checkFlash() {
	uint32_t ideSize = ESP.getFlashChipSize();
	FlashMode_t ideMode = ESP.getFlashChipMode();

#ifdef ARDUINO_ARCH_ESP8266
	uint32_t realSize = ESP.getFlashChipRealSize();
	Serial.printf("Flash real size: %u bytes\n\n", realSize);
	Serial.printf("Flash real id:   %08X\n", ESP.getFlashChipId());
	if (ideSize != realSize) {
		Serial.println("Flash Chip configuration wrong!\n");
	}
	else {
		Serial.println("Flash Chip configuration ok.\n");
	}
#endif

#ifdef ARDUINO_ARCH_ESP32
	// flasg size and id not implemented in ESP32
#endif


	Serial.printf("Flash ide  size: %u bytes\n", ideSize);
	Serial.printf("Flash ide speed: %u Hz\n", ESP.getFlashChipSpeed());
	Serial.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

}