// 
// created in Visual Studio 2015 with Visual Micro add-in:  vMicro - New Arduino Project
//   https://www.visualstudio.com/downloads/
//
//   http://www.visualmicro.com/
//
// View - Solution Explorer; Right-click on Solution in ; select "add to sourcce control"
// View - Team Explorer; Publish to GitHub (Get Started...)
//
// copy / paste Adafruit graphictest.ino  (see Arduino File-Examples...)
// if Arduino environment properly installed, simply vMicro-Build to automaticlly resolve references (no need to manually vMicro-Add Library...)
//
// Default vMicro project location is C:\Users\<username>\Documents\Arduino\<ProjectName>
//
// Target display is like the Adafruit ILI9341 http://www.adafruit.com/products/1651
//
//***************************************************


// My config is stored in myPrivateSettings.h file 
// if you choose not to use such a file, set this to false:
#include "DashboardClient.h"
#define USE_myPrivateSettings true

// Note the two possible file name string formats.
#if USE_myPrivateSettings == true 
#  include "/workspace/myPrivateSettings.h"
#else
  // create your own myPrivateSettings.h, or uncomment the following lines:
const char* WIFI_SSID = "my-wifi-SSID"
const char* WIFI_PWD = "my-WiFi-PASSWORD"
const char* DASHBOARD_URL = "/mylink/myfile.json"
const char* DASHBOARD_HOST = "mydashboardhost.com"
#endif


//#include <vector>
//int test()
//{
//	std::vector<char> fcharacters;
//
//}


// include "ili9341test.h"

// include "settings.h"
// include "debughandler.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include "ImageViewer.h"
// include "wifiConnectHelper.h" // no longer used, see htmlHelper
#include "SPI.h"
#include "Adafruit_GFX.h"        // setup via Arduino IDE; Sketch - Include Library - Manage Libraries; Adafruit GFX Library 1.1.5
#include "Adafruit_ILI9341.h"    // setup via Arduino IDE; Sketch - Include Library - Manage Libraries; Adafruit ILI9341


#include "JsonStreamingParser.h" // this library is already included as local library, but may need to be copied manually from https://github.com/squix78/json-streaming-parser
#include "JsonListener.h"

#include "htmlHelper.h"          // htmlHelper files copied to this project from https://github.com/gojimmypi/VisitorWiFi-ESP8266

//include "DashboardListener.h"   // this is our implementation of a JSON listener used by JsonStreamingParser

// include "/workspace/FastSeedTFTv2//FastTftILI9341.h" // needs avr/pgmspace - what to do for ESP8266?

#include "FreeSansBold24pt7b.h"  // copy to project directory from Adafruit-GFX-Library\Fonts; show all files. right-click "include in project"

//// For the Adafruit shield, these are the default.
//#define TFT_DC 9
//#define TFT_CS 10
//
// For the esp shield, these are the default:
#define TFT_DC 2
#define TFT_CS 15

#define Touch_CS 4
#define Touch_IRQ 5
#define SD_CS 9

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
//PDQ_ILI9341 tft2;

// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

// https://www.adafruit.com/product/1601
// https://www.adafruit.com/product/2298

// CONNECTOR VIEW FROM BACK
//ESP8266     Name   Display Pins  Name     ESP8266
//----------  -----   -----------  -------  -----------
//D2 - GPIO4  TP CS    26     25   GND
//D8 - GPIO15 LCD CS   24     23   SPI CLK  D5 - GPIO14
//D4 - GPIO2  DC RS    22     21   MISO     D6 - GPIO12
//                     20     19   MOSI     D7 - GPIO13
//D1 - GPIO5           18     17
//                     16     15            not used
//                     14     13   reset
//                     12     11            not used
//               Rx    10      9   GND
//               Tx     8      7  
//                      6      5
//               +5     4      3
//+5 Vin/Vcc     +5     2      1   3.3V     3v3 

// SOLDER PIN VIEW FROM DISPLAY SIDE 
//ESP8266		     Display Pins		     ESP8266
//            GND      25     26   TP CS    D2 - GPIO4
//D5 - GPIO14 SPI CLK  23     24   LCD CS   D8 - GPIO15
//D6 - GPIO12 MISO     21     22   DCRS     D4 - GPIO2
//D7 - GPIO13 MOSI     19     20
//                     17     18   TP IRQ	D1 - GPIO5
//      not used       15     16
//         reset       13     14
//      not used       11     12
//           GND        9     10   Rx
//                      7      8   Tx
//                      5      6              GND
//                      3      4   +5         
//           + 3        1	   2   +5         +5 Vin


DashboardClient listener;


void screenClear() {
	tft.setCursor(0, 0);
	tft.fillScreen(ILI9341_BLACK);
	// tft.drawRect(0, 0, 240, 320, 0x00FF);
	// tft.drawRect(0, 0, 320, 240, 0x00FF);
	yield();
	// Serial.println("Screen clear\n\r\n\r");
}


void fetchDashboardData() {

}

void tftPrintlnCentered(String text) { 
	char  textArray[60];
	int16_t x1, y1;

	typedef signed short int16_t; // apparently we need to manually define this to appease tft.getTextBounds
	uint16_t w;
	uint16_t h;
	

	// getTextBounds expects an array of chars, but we are using strings, so convert
	text.toCharArray(textArray, 60);

	Serial.print("textItemArray= ");
	Serial.println(textArray);

	//  getTextBounds(char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h),
	tft.getTextBounds(textArray, 0, 36, &x1, &y1, &w, &h);
	Serial.printf("Text Bounds: x1=%3d y1=%3d w=%3d h=%3d\r\n", x1, y1, w, h);

	int newX, newY;
	newX = tft.getCursorX() + ((SCREEN_HEIGHT - w) / 2); // note we are using SCREEN_HEIGHT for x-direction since screen is rotated!
	if (newX < 0) { newX = 0; }
	newY = tft.getCursorY(); // + ((SCREEN_WIDTH - h - y1) / 2);
	
	Serial.printf("newX = %3d newY=%3d", newX, newY);
	tft.setCursor(newX, newY);
	tft.println(text);
	// delete[] textArray; // cleanup
	Serial.println(ESP.getFreeHeap());
}

void UpdateDashboard() {
	tft.setRotation(3);
	tft.setFont(&FreeSansBold24pt7b); // load our custom 24pt font

	screenClear();
	tft.setCursor(0, 36);
	tft.setTextColor(ILI9341_WHITE); // tft.setTextSize(1);
	tft.println("Refreshing...");

	JsonStreamingParser parser; // note the parser can only be used once! (TODO - consider implementing some sort of re-init)
	parser.setListener(&listener); // init our JSON listener
	Serial.print("Heap=");
	Serial.println(ESP.getFreeHeap());

	WiFiClient client;
	String httpPayload = String("GET ") + "http://" + DASHBOARD_HOST + DASHBOARD_URL + " HTTP/1.1\r\n" +
		"Host: " + DASHBOARD_HOST + "\r\n" +
		"Connection: close\r\n\r\n";
	//Serial.println(httpPayload);

	const int httpPort = 80;
	if (!client.connect(DASHBOARD_HOST, httpPort)) {
		Serial.println("connection failed");
	}

	client.print(httpPayload);

	int retryCounter = 0;
	while (!client.available()) {
		delay(1000);
		retryCounter++;
		if (retryCounter > 10) {
			Serial.println("Abort!");
			return;
		}
	}

	boolean isBody = false;
	char c;

	int size = 0;
	client.setNoDelay(false);
	while (client.connected()) {
		while ((size = client.available()) > 0) {
			c = client.read();
			if (c == '{' || c == '[') {
				isBody = true;
			}
			if (isBody) {
				parser.parse(c);
				yield();
			}
		}
	}
	Serial.println("Done!");
	client.stopAll();
	parser.setListener(NULL);


	tft.fillScreen(ILI9341_BLACK);
	unsigned long start = micros();
	tft.setFont(&FreeSansBold24pt7b); // load our custom 24pt font


	listener.open();
	
	while (listener.available()) {
		yield();
		screenClear();
		String textItem;

		tft.setCursor(0, 36);

		textItem = listener.read();
		tft.setTextColor(ILI9341_WHITE); // tft.setTextSize(1);
		tftPrintlnCentered(textItem);//		tft.println(textItem);

		textItem = listener.read();
		tft.setTextColor(ILI9341_YELLOW);// tft.setTextSize(2);
		tftPrintlnCentered(textItem);//		tft.println(textItem);

		//tft.println(listener.read());

		textItem = listener.read();
		tft.setTextColor(ILI9341_RED);   // tft.setTextSize(3);
		tftPrintlnCentered(textItem);//		tft.println(textItem);
		//tft.println(listener.read());
		delay(2000);
	}
}

void showDasbboardImages() {
//	Server_Payroll_Hours.png
//	Server_Payroll_Hours.bmp
	screenClear();
	tft.setFont(); // reset to default small font when drawing images so that any long error message is readable.
	tft.setRotation(2);

	tft.setCursor(1, 1);
	bmpDrawFromUrlStream(&tft, "http://gojimmypi-test-imageconvert2bmp.azurewebsites.net/default.aspx?targetHttpImage=http://healthagency.slocounty.ca.gov/azm/images/server_payroll_hours.bmp&newImageSizeX=320", 0, 0);
	delay(5000);
	bmpDrawFromUrlStream(&tft, "http://gojimmypi-test-imageconvert2bmp.azurewebsites.net/default.aspx?targetHttpImage=http://healthagency.slocounty.ca.gov/azm/images/server_payroll_hours.bmp&newImageSizeX=320", 0, 0);
	delay(5000);
	// bmpDrawFromUrlStream(&tft, "http://healthagency.slocounty.ca.gov/azm/images/server_payroll_hours.bmp", 50, 50);
	// delay(2000);
}

void screenMessage(String message, String messageLine2 = "", String messageLine3 = "") {
	tft.setRotation(3);
	screenClear();
	tft.setCursor(0, 36);
	tft.setTextColor(ILI9341_WHITE); // tft.setTextSize(1);
	tft.println(message);
	tft.println(messageLine2);
	tft.println(messageLine3);
}

void setup() {
	Serial.begin(115200);
	Serial.println("ILI9341 Test!");



	//char json[] = "{\"a\":3, \"b\":{\"c\":\"d\"}}";
	//for (int i = 0; i < sizeof(json); i++) {
	//	parser.parse(json[i]);
	//}



	tft.begin();
	tft.setFont(&FreeSansBold24pt7b); // load our custom 24pt font

	delay(20);
	uint8_t tx = tft.readcommand8(ILI9341_RDMODE);
	tx = tft.readcommand8(ILI9341_RDSELFDIAG);

	delay(20);
	Serial.print("Self Diagnostic: 0x"); Serial.println(tx, HEX);
	delay(20);


	screenMessage("Startup...");


	//delay(20);
	//tft.setRotation(2);
	//tft.drawRect(0, 0, 240, 320, 0x00FF);
	//delay(20);


	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PWD);
	Serial.print("Connecting to ");
	Serial.print(WIFI_SSID);
	while (WiFi.status() != WL_CONNECTED) {  // try to connect wifi for 6 sec then reset
		tft.setTextColor(ILI9341_BLUE);
		tft.setCursor(15, 195);
		delay(250);
		tft.setTextColor(ILI9341_RED);
		tft.setCursor(15, 195);
		Serial.print(".");
		delay(250);
	}
	screenMessage("Connected to", WIFI_SSID);
	Serial.println("WiFi connected");
	Serial.println("");
	Serial.println(WiFi.localIP());


	if (confirmedInternetConnectivity(DASHBOARD_HOST) == 0) {
		Serial.println("Successfully connected!");
	}

	String htmlString = String("GET http://") + String(DASHBOARD_HOST) + "/" + " HTTP/1.1\r\n" +
		"Host: " + String(DASHBOARD_HOST) + "\r\n" +
		"Content-Encoding: identity" + "\r\n" +
		"Connection: Keep-Alive\r\n\r\n";

	htmlSend(DASHBOARD_HOST, 80, htmlString);




	// 


	//bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/image/24bit.bmp");
	//delay(2000);



	// Hello World!
	
	tft.setCursor(0, 0);

	// read diagnostics (optional but can help debug problems)
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

	Serial.println(F("Done!"));

}

void imageTest() {
	tft.setRotation(2);
	tft.setCursor(0, 0);
	bmpDrawFromUrlStream(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=IMG_20161109_133054198.jpg&newImageSizeY=240&newImageSizeX=320", 50, 50);
	delay(2000);
	screenClear();

	bmpDrawFromUrlStream(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=buspirate.png&newImageSizeX=320");
	delay(2000);
	screenClear();

	bmpDrawFromUrlStream(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=nasa1.jpg&newImageSizeX=320");
	delay(2000);
	screenClear();

	bmpDrawFromUrlStream(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=nasa2.jpg&newImageSizeX=320");
	delay(2000);
	screenClear();

	bmpDrawFromUrlStream(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=nasa3.png&newImageSizeX=320");
	delay(2000);
	screenClear();



	bmpDrawFromUrlStream(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=IMG_20161109_133054198.jpg&newImageSizeY=240&newImageSizeX=320");
	delay(2000);
	screenClear();

	bmpDrawFromUrlStream(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=image.png&newImageSizeY=55");
	delay(2000);
	screenClear();

	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=image.png&newImageSizeY=60");
	delay(2000);
	screenClear();

	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=image.png&newImageSizeY=70");
	delay(2000);
	screenClear();

	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=image.png&newImageSizeY=80");
	delay(2000);
	screenClear();

	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=image.png&newImageSizeY=90");
	delay(2000);
	screenClear();

	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=image.png&newImageSizeY=100");
	delay(2000);
	screenClear();

	tft.setCursor(0, 0);
	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/images/256color.bmp");
	delay(2000);
	screenClear();

	tft.setCursor(0, 0);
	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/images/16color.bmp");
	delay(2000);
	screenClear();

	tft.setCursor(0, 0);
	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/images/mono.bmp");
	delay(2000);
	screenClear();

}

//unsigned long testText() {
//	tft.fillScreen(ILI9341_BLACK);
//	unsigned long start = micros();
//	tft.setFont(&FreeSansBold24pt7b); // load our custom 24pt font
//	tft.setCursor(0, 36);
//	tft.setTextColor(ILI9341_WHITE); // tft.setTextSize(1);
//	tft.println("Hello World!");
//	//tft.setTextSize(5);
//	tft.println("");
//	//tft.setTextSize(2);
//	return micros() - start;
//}
//
//unsigned long testText2() {
//	tft.fillScreen(ILI9341_BLACK);
//	unsigned long start = micros();
//	tft.setFont(&FreeSansBold24pt7b); // load our custom 24pt font
//	tft.setCursor(0, 36);
//	tft.setTextColor(ILI9341_WHITE); // tft.setTextSize(1);
//	tft.println("Staff");
//	tft.setTextColor(ILI9341_YELLOW);// tft.setTextSize(2);
//	tft.println("Productivity:");
//	tft.setTextColor(ILI9341_RED);   // tft.setTextSize(3);
//	tft.println("  35%");
//	tft.setTextColor(ILI9341_GREEN);
//	//tft.setTextSize(5);
//	tft.println("");
//	//tft.setTextSize(2);
//	return micros() - start;
//}
//
//unsigned long testText3() {
//	tft.fillScreen(ILI9341_BLACK);
//	unsigned long start = micros();
//	tft.setFont(&FreeSansBold24pt7b); // load our custom 24pt font
//	tft.setCursor(0, 36);
//	tft.setTextColor(ILI9341_WHITE); // tft.setTextSize(1);
//	tft.println(" Budget YTD");
//	tft.setTextColor(ILI9341_YELLOW);// tft.setTextSize(2);
//	tft.println("   Spent:");
//	tft.setTextColor(ILI9341_RED);   // tft.setTextSize(3);
//	tft.println("  $36,123");
//	tft.setTextColor(ILI9341_GREEN);
//	//tft.setTextSize(5);
//	tft.println("");
//	//tft.setTextSize(2);
//	return micros() - start;
//}


void loop(void) {

	// visitor WiFi access may timeout at some point, so we many need to re-accept the Terms and Conditions.
	if (confirmedInternetConnectivity(DASHBOARD_HOST) == 0) {
		Serial.println("Successfully connected!");
	}

	// Server_Payroll_Hours.png
	tft.setRotation(3); // 3 = connector to right, long side down
	Serial.println("Updating... \r\n");
	showDasbboardImages();
	UpdateDashboard();
}



