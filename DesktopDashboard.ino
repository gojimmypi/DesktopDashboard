// 
// created in Visual Studio 2017 with Visual Micro add-in:  vMicro - New Arduino Project
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
#include "FS.h"

#include "GlobalDefine.h"
#include "hardwareHelper.h"
#include "DashboardClient.h"
#include "htmlHelper.h"

String DasboardDataFile = DASHBOARD_DEFAULT_DATA; // set a default, but based on mac address we might determine a user-specific value
// htmlHelper myHtmlHelper;




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

#pragma region BOARD_DETECT

#undef FOUND_BOARD

#ifdef ARDUINO_ARCH_ESP8266
  #include <ESP8266HTTPClient.h>
  #include <ESP8266WiFi.h>
  #define FOUND_BOARD ESP8266
#endif

#ifdef ARDUINO_ARCH_ESP32
  #include <HTTPClient.h>
  #include <WiFi.h>
  #define FOUND_BOARD ESP32
#endif

#ifndef FOUND_BOARD
#pragma message(Reminder "Error Target hardware not defined !")
#endif // ! FOUND_BOARD

#pragma endregion


//
// #include <ESP8266WiFi.h>

#include <ESP8266HTTPClient.h> // includes WiFiClient.h

#include <ESP8266WiFi.h>
#ifdef USE_TLS_SSL
// #include <WiFiClientSecure.h>
#endif // USE_TLS_SSL


#include "SPI.h"
#include "Adafruit_GFX.h"        // setup via Arduino IDE; Sketch - Include Library - Manage Libraries; Adafruit GFX Library 1.1.5
#include "Adafruit_ILI9341.h"    // setup via Arduino IDE; Sketch - Include Library - Manage Libraries; Adafruit ILI9341
#include "FreeSansBold24pt7b.h"  // Adafruit_ILI9341.h is needed; copy to project directory from Adafruit-GFX-Library\Fonts; show all files. right-click "include in project"


#include "sslHelper.h"

#include "JsonStreamingParser.h" // this library is already included as local library, but may need to be copied manually from https://github.com/squix78/json-streaming-parser
#include "JsonListener.h"

#include "WiFiHelper.h"
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
String myMacAddress;
JsonStreamingParser parser; // note the parser can only be used once! (TODO - consider implementing some sort of re-init)

#ifdef USE_TLS_SSL
THE_SSL_TYPE  client;
//WiFiClientSecure client;
const int httpPort = 443;
// Serial.println("Using WiFiClientSecure!");
#else
WiFiClient client;
const int httpPort = 80;
#endif




//#ifdef USE_TLS_SSL
//void fetchDashboardData(THE_SSL_TYPE * client, JsonStreamingParser * parser) {
//	Serial.println("Using WiFiClientSecure!");
//#else
//void fetchDashboardData(WiFiClient * client, JsonStreamingParser * parser) {
//#endif
//	parser->reset();
//	boolean isBody = false;
//	char c;
//	String msg = "";
//	int size = 0;
//	client->setNoDelay(false);
//#ifdef JSON_DEBUG
//	int sizePreview = 0; // number of chars of JSON preview, as we don't usually need to see the whole file
//	Serial.print("Showing first ");
//	Serial.println(sizePreview);
//	Serial.println(" bytes of JSON Data:");
//#endif // JSON_DEBUG
//
//	//while (client->connected()) {
//		while ((size = client->available()) > 0) {
//			c = client->read();
//			if (!isBody) {
//				msg += c;
//			}
//#ifdef JSON_DEBUG
//			sizePreview--;
//			if (sizePreview >= 0) {
//				Serial.print(c); // we read JSON data one char at a time.... but only display the first [sizePreview] bytes
//			}
//#endif // JSON_DEBUG
//
//			if (c == '{' || c == '[') {
//				isBody = true;
//			}
//			if (isBody) {
//				parser->parse(c);
//				yield();
//			}
//		}
//	//}
//
//#ifdef JSON_DEBUG
//	Serial.println("Done parsing data!\r\n\r\n");
//	Serial.println("Message");
//	Serial.println(msg);
//
//#endif // JSON_DEBUG
//
//
//#ifdef ARDUINO_ARCH_ESP8266
//	client->stopAll(); // flush client (only ESP8266 seems to have implemented stopAll)
//#endif
//
//#ifdef ARDUINO_ARCH_ESP32
//	client->stop(); // flush client (the ESP32 does not seem to have implemented stopAll)
//#endif
//
//	parser->setListener(NULL); // cleanup the parser
//
//}

//*******************************************************************************************************************************************
// UpdateDashboard (deprecated)
//*******************************************************************************************************************************************
//void UpdateDashboard() {
//	tftScreenClear();
//	tftPrintlnCentered("Refreshing...");
//
//	//tft.setRotation(3);
//	//tft.setFont(&FreeSansBold24pt7b); // load our custom 24pt font
//
//	//tftScreenClear();
//	//tft.setCursor(0, 36);
//	//tft.setTextColor(ILI9341_WHITE); // tft.setTextSize(1);
//	//tft.println("Refreshing...");
//
//	parser.setListener(&listener); // init our JSON listener
//	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
//
////#ifdef USE_TLS_SSL
////	WiFiClientSecure client;
////	const int httpPort = 443;
////	Serial.println("Using WiFiClientSecure!");
////#else
////	WiFiClient client;
////	const int httpPort = 80;
////#endif
//	Serial.println("UpdateDashboard for file =" + DasboardDataFile);
//	String httpPayload2 = String("GET ") + DASHBOARD_PATH + DasboardDataFile + " HTTP/1.1\r\n" +
//		"Host: " + DASHBOARD_HOST + "\r\n" +
//	    String(WIFI_USER_AGENT) + "\r\n" + // see myPrivbateSettings, typical value: "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; ASTE; rv:11.0) like Gecko\r\n";
//	    "Connection: Close\r\n\r\n";
//
//	Serial.println("JSON fetch httpPayload2:");
//	Serial.println(DEBUG_SEPARATOR);
//	Serial.println(httpPayload2);
//	Serial.println(DEBUG_SEPARATOR);
//
//	String httpPayload = htmlBasicHeaderText("GET", DASHBOARD_HOST, DASHBOARD_PATH + DasboardDataFile);
//#ifdef JSON_DEBUG
//	Serial.println();
//	Serial.println("JSON fetch httpPayload:");
//	Serial.println(DEBUG_SEPARATOR);
//	Serial.println(httpPayload);
//	Serial.println(DEBUG_SEPARATOR);
//#endif
//
//
//	if (!client.connect(DASHBOARD_HOST, httpPort)) {
//		Serial.println("connection failed");
//		Serial.println("");
//	}
//
//	if (client.verify(DASHBOARD_HOST_THUMBPRINT, DASHBOARD_HOST)) {
//		Serial.println("TLS certificate matches");
//	}
//	else {
//		Serial.println("TLS certificate doesn't match");
//	}
//
//	client.print(httpPayload);
//	Serial.println("request sent"); // delay needed here?
//
//	Serial.println("Reading Headers...");
//	Serial.println(DEBUG_SEPARATOR);
//
//	String line = "";
//	unsigned long timeout = millis();
//
//	timeout = millis();
//	while (client.connected()) {
//		while (client.available() == 0) {
//			yield();
//			if (millis() - timeout > 10000) {
//				Serial.println(">>> Client Timeout !");
//				client.stop();
//				return;
//			}
//		}
//		if (client.available()) {
//			yield();
//			timeout = millis();
//			line = client.readStringUntil('\n');
//			Serial.println(line);
//			if (line == "\r") {
//				Serial.println("headers received!");
//				Serial.println(DEBUG_SEPARATOR);
//				break;
//			}
//		}
//		//else {
//		//	Serial.print("No data available; status = ");
//		//	Serial.println(client.status());
//		//	Serial.println(DEBUG_SEPARATOR);
//		//	Serial.println("waiting 60 seconds...");
//		//	delay(60000);
//		//	return;
//		//}
//		yield();
//	}
//
//
//	int retryCounter = 0;
//	// see http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html
//	
//	Serial.print("\r\n\r\nProceeding! client.status = ");
//
//#ifdef ARDUINO_ARCH_ESP8266
//	Serial.print(client.status()); // (only ESP8266 seems to have implemented (client.status)
//#endif
//
//#ifdef ARDUINO_ARCH_ESP32
//	Serial.print("client.status() not available for ESP32"); // flush client (the ESP32 does not seem to have implemented client.status)
//#endif	
//
//	fetchDashboardData(&client, &parser);
//
//	tft.setRotation(3);
//	tft.fillScreen(ILI9341_BLACK);
//	unsigned long start = micros();
//	tft.setFont(&FreeSansBold24pt7b); // load our custom 24pt font
//
//
//
//#ifdef ARDUINO_ARCH_ESP8266
//	client.stopAll(); // flush client (only ESP8266 seems to have implemented stopAll)
//#endif
//
//#ifdef ARDUINO_ARCH_ESP32
//	client.stop(); // flush client (the ESP32 does not seem to have implemented stopAll)
//#endif
//}


//*******************************************************************************************************************************************
// fetchDashboardDataChar
//*******************************************************************************************************************************************
bool fetchingData = false;
boolean isBody = false;
String msg = "";
int size = 0;


//*******************************************************************************************************************************************
#ifdef USE_TLS_SSL
void fetchDashboardDataChar(THE_SSL_TYPE * client, JsonStreamingParser * parser) {
#else
void fetchDashboardDataChar(WiFiClient * client, JsonStreamingParser * parser) {
#endif
//*******************************************************************************************************************************************
	char c;
	client->setNoDelay(false);
#ifdef JSON_DEBUG
	int sizePreview = 0; // number of chars of JSON preview, as we don't usually need to see the whole file
#endif // JSON_DEBUG

	//while (client->connected()) {
	if ((size = client->available()) > 0) {
		c = client->read();
		//if (!isBody) {
		//	msg += c;
		//}
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
	else {
		if (fetchingData) {
			Serial.println("End of Fetch Data! Cleaning up....");
#ifdef ARDUINO_ARCH_ESP8266
			client->stopAll(); // flush client (only ESP8266 seems to have implemented stopAll)
#endif

#ifdef ARDUINO_ARCH_ESP32
			client->stop(); // flush client (the ESP32 does not seem to have implemented stopAll)
#endif

			parser->setListener(NULL); // cleanup the parser
			fetchingData = false;
			isBody = false;
			msg = "";
			size = 0;
		}
	}
}

unsigned long timeoutDashboardDataRefresh = 0;
unsigned long timerRefresh = 0;
const int DATA_REFRESH_INTERVAL = 10000;
bool fetchWaiting = false;

bool readyDashboardDataRefresh() {
#ifdef TIMER_DEBUG
	if ((millis() - timerRefresh) > 1000) {
		// Serial.print("Ready in: ");
		Serial.print((int)((DATA_REFRESH_INTERVAL - (millis() - timeoutDashboardDataRefresh)) / 1000));
		Serial.print(".");
		timerRefresh = millis();
	}
#endif
	return ((millis() - timeoutDashboardDataRefresh) > DATA_REFRESH_INTERVAL);
}

void testParser() {
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	Serial.print("&parser =");
	Serial.println((long)&parser);

	JsonStreamingParser newParser;

	Serial.print("&newParser =");
	Serial.println((long)&newParser);


	parser = newParser;
	Serial.println("Assigned newParser");
	Serial.print("&parser =");
	Serial.println((long)&parser);

	parser.setListener(&listener); // init our JSON listener
	HEAP_DEBUG_PRINT("Heap=");
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
}

struct tcp_pcb;
extern struct tcp_pcb* tcp_tw_pcbs;
extern "C" void tcp_abort(struct tcp_pcb* pcb);

void tcpCleanup()
{
	while (tcp_tw_pcbs != NULL)
	{
		tcp_abort(tcp_tw_pcbs);
	}
}

void GetDashboardData() {
	HEAP_DEBUG_MSG = "GetDashboardData ";
	if (!fetchingData && !fetchWaiting && !readyDashboardDataRefresh()) {
		// HEAP_DEBUG_PRINT("Prestop ");  HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
		client.stopAll();
		yield();

		// 
		//  HEAP_DEBUG_PRINT("Stop ");  HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
		// solution #1 assign new client object
		tcpCleanup();

		// client == NULL;
		 //THE_SSL_TYPE newClient;
		 //client = newClient;
		 //client.setInsecure(); // TODO fix this. Needed for BearSSL
		// HEAP_DEBUG_PRINT("Reassign ");  HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);

		// solution #2 call tcpCleanup
	    // tcpCleanup();
	}

	if (!fetchingData && !fetchWaiting && readyDashboardDataRefresh()){
		HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
		Serial.print("&parser =");
		Serial.println((long)&parser);

		JsonStreamingParser newParser;

		Serial.print("&newParser =");
		Serial.println((long)&newParser);


		parser = newParser;
		Serial.println("Assigned newParser");
		Serial.print("&parser =");
		Serial.println((long)&parser);

		parser.setListener(&listener); // init our JSON listener
		HEAP_DEBUG_PRINT("Heap=");
		HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);

		//String httpPayload2 = String("GET ") + DASHBOARD_PATH + DasboardDataFile + " HTTP/1.1\r\n" +
		//	"Host: " + DASHBOARD_HOST + "\r\n" +
		//	String(WIFI_USER_AGENT) + "\r\n" + // see myPrivbateSettings, typical value: "User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; ASTE; rv:11.0) like Gecko\r\n";
		//	"Connection: Keep-Alive\r\n\r\n";

		//Serial.println("JSON fetch httpPayload2:");
		//Serial.println(DEBUG_SEPARATOR);
		//Serial.println(httpPayload2);
		//Serial.println(DEBUG_SEPARATOR);

		String httpPayload = htmlBasicHeaderText("GET", DASHBOARD_HOST, DASHBOARD_PATH + DasboardDataFile);
#ifdef JSON_DEBUG
		Serial.println();
		HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
		Serial.println("JSON fetch httpPayload:");
		Serial.println(DEBUG_SEPARATOR);
		Serial.println(httpPayload);
		Serial.println(DEBUG_SEPARATOR);
#endif

		Serial.println("calling client.connect...");
		if (client == NULL) {
			HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
			Serial.println("creating new client...");
			THE_SSL_TYPE newClient;
			client = newClient;
			client.setInsecure(); // TODO fix this. Needed for BearSSL
			HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
		}
		delay(1000);
		if (!client.connect(DASHBOARD_HOST, httpPort)) {
			Serial.println("connection failed; need to implement wait clear");
			Serial.println("");
			fetchWaiting = true;
			return;
		}

		if (client.verify(DASHBOARD_HOST_THUMBPRINT, DASHBOARD_HOST)) {
			Serial.println("GetDashboardData TLS certificate matches");
		}
		else {
			Serial.println("GetDashboardData TLS certificate doesn't match");
		}

		HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
		client.print(httpPayload);
		Serial.println("request sent"); // delay needed here?

		HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
		Serial.println("Reading Headers...");
		Serial.println(DEBUG_SEPARATOR);

		String line = "";
		unsigned long timeout = millis();

		timeout = millis();
		while (client.connected()) {
			//HEAP_DEBUG_PRINT("1");  HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
			while (client.available() == 0) {
				yield();
				// HEAP_DEBUG_PRINT("2");  HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
				if (millis() - timeout > 10000) {
					Serial.println(">>> GetDashboardData Client Timeout !");
					client.stop();
					HEAP_DEBUG_PRINT("3");  HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
					Serial.println("RE-Connecting to WiFi...");
					wifiConnect(50);
					HEAP_DEBUG_PRINT("4");  HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
					return;
				}
			}
			//HEAP_DEBUG_PRINT("5");  HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
			if (client.available()) {
				yield();
				//HEAP_DEBUG_PRINT("6");  HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
				timeout = millis();
				//HEAP_DEBUG_PRINT("7");  HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
				line = client.readStringUntil('\n');
				//HEAP_DEBUG_PRINT("8");  HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
				Serial.println(line);
				if (line == "\r") {
					Serial.println("headers received!");
					Serial.print(sizeof(line));
					Serial.println(" bytes");
					Serial.println(DEBUG_SEPARATOR);
					break;
				}
			}
			//else {
			//	Serial.print("No data available; status = ");
			//	Serial.println(client.status());
			//	Serial.println(DEBUG_SEPARATOR);
			//	Serial.println("waiting 60 seconds...");
			//	delay(60000);
			//	return;
			//}
			yield();
			fetchingData = true;
			timeoutDashboardDataRefresh = millis();
		}


		int retryCounter = 0;
		// see http://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/client-examples.html

		Serial.print("\r\n\r\nProceeding! client.status = ");

#ifdef ARDUINO_ARCH_ESP8266
		Serial.print(client.status()); // (only ESP8266 seems to have implemented (client.status)
#endif

#ifdef ARDUINO_ARCH_ESP32
		Serial.print("client.status() not available for ESP32"); // flush client (the ESP32 does not seem to have implemented client.status)
#endif	
	}
	else {
		fetchDashboardDataChar(&client, &parser);
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
//int wifiConnect(int maxAttempts = 50) {
//	int countAttempt = 0;
//	WiFi.mode(WIFI_STA);
//	WiFi.begin(WIFI_SSID, WIFI_PWD);
//
//	myMacAddress = WiFi.macAddress(); // this returns 6 hex bytes, delimited by colons
//	screenMessage("MAC Address", myMacAddress.substring(0, 9), myMacAddress.substring(9, 18)); // 01:34:67:90:12:45
//
//#ifdef WIFI_DEBUG
//	Serial.print("Connecting to ");
//	Serial.print(WIFI_SSID);
//#endif
//	while (WiFi.status() != WL_CONNECTED) {  // try to connect wifi for 6 sec then reset
//		
//		// this tft code is not actualy DOING anything yet
//		tft.setTextColor(ILI9341_BLUE);
//		tft.setCursor(15, 195);
//		delay(250);
//		tft.setTextColor(ILI9341_RED);
//		tft.setCursor(15, 195);
//
//#ifdef WIFI_DEBUG
//		Serial.print(".");
//#endif
//		delay(250);
//		countAttempt++;
//		if (countAttempt > maxAttempts) {
//			countAttempt = 0;
//#ifdef WIFI_DEBUG
//			Serial.println("WiFi Disconnect... ");
//#endif
//			WiFi.disconnect();
//			delay(5000);
//#ifdef WIFI_DEBUG
//			Serial.println("WiFi Retrying. ");
//#endif
//			WiFi.mode(WIFI_STA);
//			WiFi.begin(WIFI_SSID, WIFI_PWD);
//		}
//	}
//	delay(5000);
//	myMacAddress.replace(":", "");
//	myMacAddress.replace("-", ""); // probably not used, but just in case they MAC address starts returning other well known delimiters such as dash
//	myMacAddress.replace(" ", ""); // or perhaps even a space
//
//	Serial.println("MAC Address=" + myMacAddress);
//}

//*******************************************************************************************************************************************
// 
// TODO - based on MAC address, determine data file name
//*******************************************************************************************************************************************
void GetDashboardDataFile() {
	// the files are expected to be static JSON, pre-generated by process at server (we don't want to wait on generation of files for display!)
	DasboardDataFile =  String(DASHBOARD_KEY) + myMacAddress + ".json";
	delay(2000);
	HEAP_DEBUG_PRINT("Heap (before htmlExists) "); HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	if (!htmlExists(DasboardDataFile)) {
		Serial.println(DasboardDataFile + " not found, using default");
		DasboardDataFile = "2134.json";
	}
	HEAP_DEBUG_PRINT("Heap (after htmlExists) "); HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	screenMessage("Using file", "ID: " + DasboardDataFile);
	// TODO - if the file does not exist, then use default.
}

#pragma region setup

int setupSPIFFS() {
	int okSPIFFS = 0;
	HEAP_DEBUG_PRINT("Heap (before SPIFFS.begin) "); HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	SPIFFS.begin();
	// SPIFFS.format();
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	File f = SPIFFS.open("/myFile.txt", "w");
	if (!f) {
		Serial.println("file open failed 1");
		SPIFFS.format();
	}
	else {
		Serial.println("file open SUCCESS 1");
	}
	f.close();

	f = SPIFFS.open("/myFile.txt", "w");
	if (!f) {
		Serial.println("file open failed 1.1");
		okSPIFFS = 1;
	}
	else {
		Serial.println("file open SUCCESS 1.1");
		f.println("Hello World!");
	}
	f.close();

	Dir dir = SPIFFS.openDir("/");
	while (dir.next()) {
		Serial.print(dir.fileName());
		File f = dir.openFile("r");
		Serial.println(f.size());
	}
	String fmsg;
	f = SPIFFS.open("/myFile.txt", "r");
	if (!f) {
		Serial.println("file open failed 2");
	}
	else {
		Serial.println("file open SUCCESS 2");
		okSPIFFS = 2;
		fmsg = f.readString();
	}
	Serial.println("msg=");
	Serial.println(fmsg);
	f.close();
	Serial.println("File test done!");
	SPIFFS.end();
	HEAP_DEBUG_PRINT("Heap (after SPIFFS.end)="); HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	return okSPIFFS;
}

int setupDisplay() {
	Serial.println("ILI9341 Test!");
	tft.begin();
	tft.setFont(&FreeSansBold24pt7b); // load our custom 24pt font
	tftScreenDiagnostics();


	delay(20);
	uint8_t tx = tft.readcommand8(ILI9341_RDMODE);
	tx = tft.readcommand8(ILI9341_RDSELFDIAG);

	delay(20);
	Serial.print("Self Diagnostic: 0x"); Serial.println(tx, HEX);
	delay(20);

	tft.setCursor(1, 1);
	tftScreenDiagnostics();
	screenMessage("Startup...");
	return 0;
}

int setupWiFi() {
	HEAP_DEBUG_MSG = "setupWiFi Heap = ";
	HEAP_DEBUG_PRINTLN("Heap (setupWiFi begin)");

	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	htmlSetClient(&client);
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);

	WIFI_DEBUG_PRINTLN("calling wifiConnect...");
	wifiConnect(50);
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);

	screenMessage("Connected to", WIFI_SSID);

	WIFI_DEBUG_PRINTLN("WiFi connected. My IP address:");
	WIFI_DEBUG_PRINTLN("");
	WIFI_DEBUG_PRINTLN(WiFi.localIP());
	if (confirmedInternetConnectivity(DASHBOARD_HOST) == 0) {
		Serial.println("Successfully connected!");
	}

	// testSSL();
	HEAP_DEBUG_PRINT("Heap (setupWiFi end); "); HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	return 0;
}
#pragma endregion

#pragma region OldStuff
//char json[] = "{\"a\":3, \"b\":{\"c\":\"d\"}}";
//for (int i = 0; i < sizeof(json); i++) {
//	parser.parse(json[i]);
//}



//String urlLogo = String("GET http://") + 
//	             String(DASHBOARD_HOST) + 
//	             String(DASHBOARD_APP) + 
//   				 "/imageConvert2BMP.aspx?targetImageName=logo.png&newImageSizeX=320";

//delay(20);
//tft.setRotation(2);
//tft.drawRect(0, 0, 240, 320, 0x00FF);
//delay(20);



//String htmlString = String("GET http://") + String(DASHBOARD_HOST) + "/" + " HTTP/1.1\r\n" +
//	"Host: " + String(DASHBOARD_HOST) + "\r\n" +
//	"Content-Encoding: identity" + "\r\n" +
//	"Connection: Keep-Alive\r\n\r\n";

//htmlSend(DASHBOARD_HOST, 80, htmlString); // test to confirm internet operational
//showStartupImage();

// 


//bmpDraw(&tft, "http://gojimmypi-dev-imageconvert2bmp.azurewebsites.net/image/24bit.bmp");
//delay(2000);



// Hello World!


#pragma endregion


//*******************************************************************************************************************************************
// 
//*******************************************************************************************************************************************
void setup() {

	delay(500);
	Serial.begin(115200);
	HEAP_DEBUG_MSG = "mySetup; heap = ";
	HEAP_DEBUG_PRINTLN("info for (begin)");
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);

	Serial.println(DEBUG_SEPARATOR);

	HEAP_DEBUG_PRINTLN("");
	HEAP_DEBUG_PRINTLN("Heap (Setup begin)="); 
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);

	setupDisplay();
	checkFlash();
	setupSPIFFS();
	setupWiFi();
	client.setInsecure(); // TODO fix this. Needed for BearSSL

	timeoutDashboardDataRefresh = millis();

	GetDashboardDataFile();
	
	// UpdateDashboard(); // we do this in the loop only

	Serial.println(F("Setup Done!"));
	HEAP_DEBUG_PRINT("Heap (setup complete)="); HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);

	// #pragma message(Reminder "Fix this problem!")

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
//void loopNew(void) {
//	//CheckDashboardFetch();
//
//	// visitor WiFi access may timeout at some point, so we many need to re-accept the Terms and Conditions.
//	if (confirmedInternetConnectivity(DASHBOARD_HOST) == 0) {
//		Serial.println("Successfully connected!");
//	}
//
//	// Server_Payroll_Hours.png
//	tft.setRotation(3); // 3 = connector to right, long side down
//	Serial.println("Updating... \r\n");
//	// showDasbboardImages();
//	UpdateDashboard();
//}


unsigned long timeoutDisplayItem = millis();
//*******************************************************************************************************************************************
//*******************************************************************************************************************************************
void UpdateDashboardDisplay() {
	if (listener.available()) {
		if (millis() - timeoutDisplayItem > 2000) {
			yield();
			tftScreenClear();
			tft.setRotation(3); // 3 = connector to right, long side down
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
			timeoutDisplayItem = millis();
			// HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
		}
	}
	else {
		listener.open(); // start over
	}
}

//*******************************************************************************************************************************************
//*******************************************************************************************************************************************
//  loop - a pseudo-RTOS function that polls each process to see if it needs attention
//*******************************************************************************************************************************************
//*******************************************************************************************************************************************
unsigned long timeoutConnect = millis();

void loop(void) {
	yield();
	// tftScreenDiagnostics();

	UpdateDashboardDisplay(); // we'll only actually update the display after given amount of time has passed...
	GetDashboardData();

	// visitor WiFi access may timeout at some point, so we many need to re-accept the Terms and Conditions.
	//if (confirmedInternetConnectivity(DASHBOARD_HOST) == 0) {
	//	Serial.println("Successfully connected!");
	//}

	// Server_Payroll_Hours.png
	// Serial.println("Updating... \r\n");
	// showDasbboardImages();
}



