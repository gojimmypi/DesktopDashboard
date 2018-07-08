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


#include <ESP8266WiFi.h>
#ifdef USE_TLS_SSL
// #include <WiFiClientSecure.h> // included in <ESP8266WiFi.h>
#else
#endif // USE_TLS_SSL


String queryStringValue(String urlString, String keyString);



// html headers have [keyword: value] settings, getHeaderValue gets integer or string values
String getHeaderValue(String keyWord, String str);
void getHeaderValue(String keyWord, String str, uint& OutValue);
void getHeaderValue(String keyWord, String str, String& OutValue);

// assemble a basic header in a string given verb, host, and URL:
String htmlBasicHeaderText(String verb, const char* host, String);

int htmlSend(WiFiClient* thisClient, const char* thisHost, int thisPort);
int htmlSend(WiFiClient* thisClient, const char* thisHost, int thisPort, String sendHeader);
int htmlSend(THE_SSL_TYPE* thisClient, const char* thisHost, int thisPort);
int htmlSend(THE_SSL_TYPE* thisClient, const char* thisHost, int thisPort, String sendHeader);
 

int htmlSend(const char* thisHost, int thisPort, String sendHeader);
int htmlSendPlainText(const char* thisHost, String sendHeader);

#ifdef USE_TLS_SSL
void htmlSetClient(THE_SSL_TYPE* thisClient);
#else
void htmlSetClient(WiFiClient* thisClient);
#endif

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

#ifdef USE_TLS_SSL
	htmlHelper(THE_SSL_TYPE*, const char*, int, String);
	htmlHelper(THE_SSL_TYPE*, const char*, int);
#else
	htmlHelper(WiFiClient*, const char*, int, String);
	htmlHelper(WiFiClient*, const char*, int);
#endif
	htmlHelper();
	int Send();
};

#endif

