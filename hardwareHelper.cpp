// 
// 
// 

#include "hardwareHelper.h"


void checkFlash() {
	uint32_t realSize = ESP.getFlashChipRealSize();
	uint32_t ideSize = ESP.getFlashChipSize();
	FlashMode_t ideMode = ESP.getFlashChipMode();

	Serial.printf("Flash real id:   %08X\n", ESP.getFlashChipId());
	Serial.printf("Flash real size: %u bytes\n\n", realSize);

	Serial.printf("Flash ide  size: %u bytes\n", ideSize);
	Serial.printf("Flash ide speed: %u Hz\n", ESP.getFlashChipSpeed());
	Serial.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

	if (ideSize != realSize) {
		Serial.println("Flash Chip configuration wrong!\n");
	}
	else {
		Serial.println("Flash Chip configuration ok.\n");
	}
}