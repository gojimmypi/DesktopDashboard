// 
// 
// 

#include "tftHelper.h"

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

typedef signed short int16_t; // apparently we need to manually define this to appease tft.getTextBounds


//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
void tftPrintlnCentered(String text) {
	char  textArray[60];
	int16_t x1, y1; // return position parameters of the TextBounds

	uint16_t w; // return width value of TextBounds
	uint16_t h; // return height value of TextBounds


				// getTextBounds expects an array of chars, but we are using strings, so convert
	text.toCharArray(textArray, 60);

	//Serial.print("textItemArray= ");
	//Serial.println(textArray);

	//  getTextBounds(char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h),
	tft.getTextBounds(textArray, 0, 36, &x1, &y1, &w, &h);
	// Serial.printf("Text Bounds: x1=%3d y1=%3d w=%3d h=%3d\r\n", x1, y1, w, h);

	int newX, newY;
	newX = tft.getCursorX() + ((SCREEN_HEIGHT - w) / 2); // note we are using SCREEN_HEIGHT for x-direction since screen is rotated!
	if (newX < 0) { newX = 0; }
	newY = tft.getCursorY();  // y position does not change for horizontal centering  // + ((SCREEN_WIDTH - h)  / 2);

							  //Serial.printf("newX = %3d newY=%3d", newX, newY);
							  //Serial.println();
	tft.setCursor(newX, newY);
	tft.println(text);
#ifdef SERIAL_SCREEN_DEBUG
	Serial.print("Centered text:");
	Serial.println(text);
#endif
	//Serial.print("Heap=");
	//Serial.println(ESP.getFreeHeap());
}

//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
void tftScreenClear() {
	tft.setCursor(0, 0);
	tft.fillScreen(ILI9341_BLACK);
	// tft.drawRect(0, 0, 240, 320, 0x00FF);
	// tft.drawRect(0, 0, 320, 240, 0x00FF);
	yield();
#ifdef SERIAL_SCREEN_DEBUG
	Serial.println("Screen clear\n\r");
#endif
}


//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
void tftScreenDiagnostics() {
	// read diagnostics (optional but can help debug problems)
#ifdef TFT_DEBUG
	uint8_t x = tft.readcommand8(ILI9341_RDMODE);
	Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX); // success =  0x9C
	x = tft.readcommand8(ILI9341_RDMADCTL);
	Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX); // Sucess =  0x48
	x = tft.readcommand8(ILI9341_RDPIXFMT);
	Serial.print("Pixel Format: 0x"); Serial.println(x, HEX); // Success =  0x5
	x = tft.readcommand8(ILI9341_RDIMGFMT);
	Serial.print("Image Format: 0x"); Serial.println(x, HEX); // Success = 0x0
	x = tft.readcommand8(ILI9341_RDSELFDIAG);
	Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); // Success =  0x0
#endif
}

//*******************************************************************************************************************************************
//  screenMessage   (setRotation = 3)
//*******************************************************************************************************************************************
void screenMessage(String message, String messageLine2, String messageLine3) {
	tftScreenClear();
	tft.setRotation(3);
	tft.setCursor(0, 36);
	tft.setTextColor(ILI9341_WHITE); // tft.setTextSize(1);
	tft.println(message);
	if (messageLine2 > "") { tft.println(messageLine2); }
	if (messageLine3 > "") { tft.println(messageLine3); }

#ifdef SERIAL_SCREEN_DEBUG
	Serial.println(message);
	if (messageLine2 > "") { Serial.println(messageLine2); }
	if (messageLine3 > "") { Serial.println(messageLine3); }
#endif
}
