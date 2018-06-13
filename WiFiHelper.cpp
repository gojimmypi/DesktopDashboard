#include "WiFiHelper.h"


#if USE_myPrivateSettings == true 
#include "/workspace-git/myPrivateSettings.h"
#else
#include "/workspace-git/myPrivateSettings.h"
//// create your own myPrivateSettings.h, or uncomment the following lines:
//static const char* WIFI_SSID = "my-wifi-SSID"
//static const char* WIFI_PWD = "my-WiFi-PASSWORD"

//static const char* DASHBOARD_DEFAULT_DATA = "sampledata.json";
//static const char* DASHBOARD_PATH = "/theDataPath/";
//static const char* DASHBOARD_APP  = "/theDashboardApplicationPath/";
//static const char* DASHBOARD_HOST = "mydashboardhost.com";
//static const char* DASHBOARD_KEY  = "XYZZY";
// will build:  http://mydashboardhost.com/theDashboardApplicationPath/
//      and:    http://mydashboardhost.com/theDataPath/
// 
// we will fetch data from a JSON file called http://mydashboardhost.com/theDataPath/XYZZYMMAABBCCDDEEFF
// where MMAABBCCDDEEFF is this device's HEX MAC Address, with no spaces, dashes, or commas
#endif



#include <ESP8266HTTPClient.h> // includes WiFiClient.h
#include <ESP8266WiFi.h>
#include "tftHelper.h"



//*******************************************************************************************************************************************
// wifiConnect 
// 
//   WiFi.begin with repeated attempts with TFT screen and optional serial progress indication
//
//*******************************************************************************************************************************************
int wifiConnect(int maxAttempts) {
#ifdef WIFI_DEBUG
	Serial.println("*****************************************************************");
	Serial.print("wifiConnect: Connecting to ");
	Serial.println(WIFI_SSID);
	Serial.println("*****************************************************************");
#endif

	int countAttempt = 0;
	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PWD);
	
	String myMacAddress;
	myMacAddress = WiFi.macAddress(); // this returns 6 hex bytes, delimited by colons
	screenMessage("MAC Address", myMacAddress.substring(0, 9), myMacAddress.substring(9, 18)); // 01:34:67:90:12:45

#ifdef WIFI_DEBUG
	Serial.println("Starting Connection Loop...");
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
#ifdef WIFI_DEBUG
	Serial.println("Connected!");
#endif
	delay(5000);
	myMacAddress.replace(":", "");
	myMacAddress.replace("-", ""); // probably not used, but just in case they MAC address starts returning other well known delimiters such as dash
	myMacAddress.replace(" ", ""); // or perhaps even a space

	Serial.println("MAC Address=" + myMacAddress);
	return 0;
}