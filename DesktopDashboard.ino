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

#define SCREEN_DEBUG // when defined, display low level screen debug info 
#define JSON_DEBUG // when defined, display JSON debug info 
#define WIFI_DEBUG // when defined, display WiFi debug info 
#define SERIAL_SCREEN_DEBUG // when defined, display screen messages to serial port

//*******************************************************************************************************************************************
// Begin user config
//*******************************************************************************************************************************************
// My config is stored in myPrivateSettings.h file 
// if you choose not to use such a file, set this to false:
#define USE_myPrivateSettings true

// Note the two possible file name string formats.
#if USE_myPrivateSettings == true 
#include "/workspace-git/myPrivateSettings.h"
#else
// create your own myPrivateSettings.h, or uncomment the following lines:
//const char* WIFI_SSID = "my-wifi-SSID"
//const char* WIFI_PWD = "my-WiFi-PASSWORD"

//const char* DASHBOARD_DEFAULT_DATA = "sampledata.json";
//const char* DASHBOARD_PATH = "/theDataPath/";
//const char* DASHBOARD_APP  = "/theDashboardApplicationPath/";
//const char* DASHBOARD_HOST = "mydashboardhost.com";
//const char* DASHBOARD_KEY  = "XYZZY";
// will build:  http://mydashboardhost.com/theDashboardApplicationPath/
//      and:    http://mydashboardhost.com/theDataPath/
// 
// we will fetch data from a JSON file called http://mydashboardhost.com/theDataPath/XYZZYMMAABBCCDDEEFF
// where MMAABBCCDDEEFF is this device's HEX MAC Address, with no spaces, dashes, or commas
#endif
//*******************************************************************************************************************************************
// End user config
//*******************************************************************************************************************************************
#include "DashboardClient.h"
#include "htmlHelper.h"

String DasboardDataFile = DASHBOARD_DEFAULT_DATA; // set a default, but based on mac address we might determine a user-specific value
String myMacAddress = "";
htmlHelper myHtmlHelper;

//#include <vector>
//int test()
//{
//	std::vector<char> fcharacters;
//
//}


// include "ili9341test.h"
// include "settings.h"
// include "debughandler.h"
// include "wifiConnectHelper.h" // no longer used, see htmlHelper
// include "DashboardListener.h"   // this is our implementation of a JSON listener used by JsonStreamingParser
// include "/workspace/FastSeedTFTv2//FastTftILI9341.h" // needs avr/pgmspace - what to do for ESP8266?

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include "SPI.h"
#include "Adafruit_GFX.h"        // setup via Arduino IDE; Sketch - Include Library - Manage Libraries; Adafruit GFX Library 1.1.5
#include "Adafruit_ILI9341.h"    // setup via Arduino IDE; Sketch - Include Library - Manage Libraries; Adafruit ILI9341
#include "FreeSansBold24pt7b.h"  // copy to project directory from Adafruit-GFX-Library\Fonts; show all files. right-click "include in project"

#include "JsonStreamingParser.h" // this library is already included as local library, but may need to be copied manually from https://github.com/squix78/json-streaming-parser
#include "JsonListener.h"

#include "htmlHelper.h"          // htmlHelper files copied to this project from https://github.com/gojimmypi/VisitorWiFi-ESP8266


// TODO - delete display stuff once move to tftHelper
//#define TFT_DC 2
//#define TFT_CS 15
//
//#define Touch_CS 4
//#define Touch_IRQ 5
//#define SD_CS 9
// Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#include "ImageViewer.h"
#include "tftHelper.h"           // tft screen printing 

DashboardClient listener;





void fetchDashboardData(WiFiClient * client, JsonStreamingParser * parser) {
	boolean isBody = false;
	char c;
	String msg = "";
	int size = 0;
	client->setNoDelay(false);
#ifdef JSON_DEBUG
	int sizePreview = 120; // number of chars of JSON preview, as we don't usually need to see the whole file
	Serial.print("Showing first ");
	Serial.println(sizePreview);
	Serial.println(" bytes of JSON Data:");
#endif // JSON_DEBUG
	while (client->connected()) {
		while ((size = client->available()) > 0) {
			c = client->read();
			if (!isBody) {
				msg += c;
			}
#ifdef JSON_DEBUG
			sizePreview--;
			if (sizePreview >= 0) {
				Serial.print(c); // we read JSON data one char at a time.... but only display the first [sizePreview] bytes
			}
#endif // JSON_DEBUG

			if (c == '{' || c == '[') {
				isBody = true;
			}
			if (isBody) {
				parser->parse(c);
				yield();
			}
		}
	}
#ifdef JSON_DEBUG
	Serial.println("Done parsing data!\r\n\r\n");
	Serial.println("Message");
	Serial.println(msg);

#endif // JSON_DEBUG
	client->stopAll(); // flush client
	parser->setListener(NULL); // cleanup the parser

}

//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
void UpdateDashboard() {
	tftScreenClear();
	tftPrintlnCentered("Refreshing...");

	//tft.setRotation(3);
	//tft.setFont(&FreeSansBold24pt7b); // load our custom 24pt font

	//tftScreenClear();
	//tft.setCursor(0, 36);
	//tft.setTextColor(ILI9341_WHITE); // tft.setTextSize(1);
	//tft.println("Refreshing...");

	JsonStreamingParser parser; // note the parser can only be used once! (TODO - consider implementing some sort of re-init)
	parser.setListener(&listener); // init our JSON listener
	Serial.print("Heap=");
	Serial.println(ESP.getFreeHeap());

	WiFiClient client;
	String httpPayload = String("GET ") + "http://" + DASHBOARD_HOST + DASHBOARD_PATH + DasboardDataFile + " HTTP/1.1\r\n" +
		"Host: " + DASHBOARD_HOST + "\r\n" +
		"Connection: close\r\n\r\n";

#ifdef JSON_DEBUG
	Serial.println();
	Serial.println("JSON fetch httpPayload:");
	Serial.println(httpPayload);
#endif

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
			tftScreenClear();
			tftPrintlnCentered("WiFi Problem...");
			delay(5000);
			return;
		}
	}
	Serial.print("\r\n\r\nclient.status = ");
	Serial.print(client.status());

	fetchDashboardData(&client, &parser);

	tft.setRotation(3);
	tft.fillScreen(ILI9341_BLACK);
	unsigned long start = micros();
	tft.setFont(&FreeSansBold24pt7b); // load our custom 24pt font


	listener.open();
	
	while (listener.available()) {
		yield();
		tftScreenClear();
		String textItem;

		tft.setCursor(0, 36);

		textItem = listener.read();
		tft.setTextColor(ILI9341_WHITE); // tft.setTextSize(1);
		tftPrintlnCentered(textItem);//		tft.println(textItem);

		textItem = listener.read();
		tft.setTextColor(ILI9341_YELLOW);// tft.setTextSize(2);
		tftPrintlnCentered(textItem);//		tft.println(textItem);

		textItem = listener.read();
		tft.setTextColor(ILI9341_RED);   // tft.setTextSize(3);
		tftPrintlnCentered(textItem);//		tft.println(textItem);
		delay(2000);
	}
}

//*******************************************************************************************************************************************
// return baseURL like  http://myDashboardHost.com/theDashboardApplicationPath/
//*******************************************************************************************************************************************
String baseURL() {
	return String(httpText) + String(DASHBOARD_HOST) + String(DASHBOARD_APP);
}

//*******************************************************************************************************************************************
// show images (setRotation = 2)
//*******************************************************************************************************************************************
void showDasbboardImages() {
//	Server_Payroll_Hours.png
//	Server_Payroll_Hours.bmp
	tftScreenClear();

	tft.setFont(); // reset to default small font when drawing images so that any long error message is readable.
	tft.setRotation(2);

	tft.setCursor(1, 1);
	String thisImage = baseURL() + "imageConvert2BMP.aspx?targetImageName=server_payroll_hours.bmp&newImageSizeX=320";
	String oldLink = "http://gojimmypi-test-imageconvert2bmp.azurewebsites.net/default.aspx?targetHttpImage=http://healthagency.slocounty.ca.gov/azm/images/server_payroll_hours.bmp&newImageSizeX=320";

	Serial.print("Drawing: ");
	Serial.print(thisImage);
	bmpDrawFromUrlStream(&tft, thisImage, 0, 0);
	imageViewDelay(); 
}

//*******************************************************************************************************************************************
// startup image 
//*******************************************************************************************************************************************
void showStartupImage() {
	// startup image
	Serial.print("Build string...");

	tft.setRotation(2);
	String imageURL = "http://" + String(DASHBOARD_HOST) + String(DASHBOARD_APP) + "imageConvert2BMP.aspx?targetImageName=logo.png&newImageSizeX=320";
	Serial.println(imageURL);
	bmpDrawFromUrlStream(&tft, imageURL, 0, 0);

	Serial.print("Done with logo");
}



//*******************************************************************************************************************************************
// wifiConnect 
// 
//   WiFi.begin with repeated attempts with TFT screen and optional serial progress indication
//
//*******************************************************************************************************************************************
int wifiConnect(int maxAttempts = 50) {
	int countAttempt = 0;
	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PWD);

	myMacAddress = WiFi.macAddress(); // this returns 6 hex bytes, delimited by colons
	screenMessage("MAC Address", myMacAddress.substring(0, 9), myMacAddress.substring(9, 18)); // 01:34:67:90:12:45

#ifdef WIFI_DEBUG
	Serial.print("Connecting to ");
	Serial.print(WIFI_SSID);
#endif
	while (WiFi.status() != WL_CONNECTED) {  // try to connect wifi for 6 sec then reset
		
		// this tft code is not actualy DOING anything yet
		tft.setTextColor(ILI9341_BLUE);
		tft.setCursor(15, 195);
		delay(250);
		tft.setTextColor(ILI9341_RED);
		tft.setCursor(15, 195);

#ifdef WIFI_DEBUG
		Serial.print(".");
#endif
		delay(250);
		countAttempt++;
		if (countAttempt > maxAttempts) {
			countAttempt = 0;
#ifdef WIFI_DEBUG
			Serial.println("WiFi Disconnect... ");
#endif
			WiFi.disconnect();
			delay(5000);
#ifdef WIFI_DEBUG
			Serial.println("WiFi Retrying. ");
#endif
			WiFi.mode(WIFI_STA);
			WiFi.begin(WIFI_SSID, WIFI_PWD);
		}
	}
	delay(5000);
	myMacAddress.replace(":", "");
	myMacAddress.replace("-", ""); // probably not used, but just in case they MAC address starts returning other well known delimiters such as dash
	myMacAddress.replace(" ", ""); // or perhaps even a space

	Serial.println("MAC Address=" + myMacAddress);
}

//*******************************************************************************************************************************************
// 
// TODO - based on MAC address, determine data file name
//*******************************************************************************************************************************************
void GetDasboardDataFile() {
	// the files are expected to be static JSON, pre-generated by process at server (we don't want to wait on generation of files for display!)
	DasboardDataFile = DASHBOARD_KEY + myMacAddress + ".json";
	delay(2000);
	if (!htmlExists(DasboardDataFile)) {
		DasboardDataFile = "2134.json";
	}
	screenMessage("Using file", "ID: " + DasboardDataFile);
	// TODO - if the file does not exist, then use default.
}

//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
void setup() {
	delay(5000);
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
	tft.setCursor(1, 1);
	//String urlLogo = String("GET http://") + 
	//	             String(DASHBOARD_HOST) + 
	//	             String(DASHBOARD_APP) + 
 //   				 "/imageConvert2BMP.aspx?targetImageName=logo.png&newImageSizeX=320";

	//delay(20);
	//tft.setRotation(2);
	//tft.drawRect(0, 0, 240, 320, 0x00FF);
	//delay(20);

	wifiConnect(50);

	screenMessage("Connected to", WIFI_SSID);

#ifdef WIFI_DEBUG
	Serial.println("WiFi connected");
	Serial.println("");
	Serial.println(WiFi.localIP());
#endif

	if (confirmedInternetConnectivity(DASHBOARD_HOST) == 0) {
		Serial.println("Successfully connected!");
	}

	String htmlString = String("GET http://") + String(DASHBOARD_HOST) + "/" + " HTTP/1.1\r\n" +
		"Host: " + String(DASHBOARD_HOST) + "\r\n" +
		"Content-Encoding: identity" + "\r\n" +
		"Connection: Keep-Alive\r\n\r\n";

	htmlSend(DASHBOARD_HOST, 80, htmlString); // test to confirm internet operational

	GetDasboardDataFile();


	showStartupImage();

	// 


	//bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/image/24bit.bmp");
	//delay(2000);



	// Hello World!
	
	tft.setCursor(0, 0);
	tftScreenDiagnostics();

	Serial.println(F("Setup Done!"));

}

//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
void imageTest() {
	tft.setRotation(2);
	tft.setCursor(0, 0);
	bmpDrawFromUrlStream(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=IMG_20161109_133054198.jpg&newImageSizeY=240&newImageSizeX=320", 50, 50);
	delay(2000);
	tftScreenClear();

	bmpDrawFromUrlStream(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=buspirate.png&newImageSizeX=320");
	delay(2000);
	tftScreenClear();

	bmpDrawFromUrlStream(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=nasa1.jpg&newImageSizeX=320");
	delay(2000);
	tftScreenClear();

	bmpDrawFromUrlStream(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=nasa2.jpg&newImageSizeX=320");
	delay(2000);
	tftScreenClear();

	bmpDrawFromUrlStream(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=nasa3.png&newImageSizeX=320");
	delay(2000);
	tftScreenClear();



	bmpDrawFromUrlStream(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=IMG_20161109_133054198.jpg&newImageSizeY=240&newImageSizeX=320");
	delay(2000);
	tftScreenClear();

	bmpDrawFromUrlStream(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=image.png&newImageSizeY=55");
	delay(2000);
	tftScreenClear();

	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=image.png&newImageSizeY=60");
	delay(2000);
	tftScreenClear();

	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=image.png&newImageSizeY=70");
	delay(2000);
	tftScreenClear();

	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=image.png&newImageSizeY=80");
	delay(2000);
	tftScreenClear();

	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=image.png&newImageSizeY=90");
	delay(2000);
	tftScreenClear();

	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/default.aspx?targetImageName=image.png&newImageSizeY=100");
	delay(2000);
	tftScreenClear();

	tft.setCursor(0, 0);
	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/images/256color.bmp");
	delay(2000);
	tftScreenClear();

	tft.setCursor(0, 0);
	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/images/16color.bmp");
	delay(2000);
	tftScreenClear();

	tft.setCursor(0, 0);
	bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/images/mono.bmp");
	delay(2000);
	tftScreenClear();

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


//*******************************************************************************************************************************************
//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
//*******************************************************************************************************************************************
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



