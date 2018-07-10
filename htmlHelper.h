// htmlHelper.h

#ifndef _HTMLHELPER_h
#define _HTMLHELPER_h

#include "GlobalDefine.h"

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


#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif



String queryStringValue(String urlString, String keyString);

// html headers have [keyword: value] settings, getHeaderValue gets integer or string values
String getHeaderValue(String keyWord, String str);
void getHeaderValue(String keyWord, String str, uint& OutValue);
void getHeaderValue(String keyWord, String str, String& OutValue);

// assemble a basic header in a string given verb, host, and URL:
String htmlBasicHeaderText(String verb, const char* host, String);

//int htmlSend(WiFiClient* thisClient, const char* thisHost, int thisPort);
//int htmlSend(WiFiClient* thisClient, const char* thisHost, int thisPort, String sendHeader);
int htmlSend(WIFI_CLIENT_CLASS* thisClient, const char* thisHost, int thisPort);
int htmlSend(WIFI_CLIENT_CLASS* thisClient, const char* thisHost, int thisPort, String sendHeader);
 

int htmlSend(const char* thisHost, int thisPort, String sendHeader);
int htmlSendPlainText(const char* thisHost, String sendHeader);

void htmlSetClient(WIFI_CLIENT_CLASS* thisClient);

int doAcceptTermsAndConditions();

int confirmedInternetConnectivity(const char* host);
extern const char* httpText; // = "http://"; // this is defined once here to allow easy transition from http:// to https://

bool htmlExists(String targetURL);

class htmlHelper {

	// depending on the USE_TLS_SSL the myClient is either WiFiClient or WiFiClientSecure (but they are different! not sure secure or not...)

	const char* thisHost;
	int thisPort;
	String sendHeader;

public:

	htmlHelper(WIFI_CLIENT_CLASS*, const char*, int, String);
	htmlHelper(WIFI_CLIENT_CLASS*, const char*, int);

    htmlHelper();
	int Send();
};

#endif

