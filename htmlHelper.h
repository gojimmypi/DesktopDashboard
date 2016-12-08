// htmlHelper.h

#ifndef _HTMLHELPER_h
#define _HTMLHELPER_h
#include <ESP8266WiFi.h>

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

// int htmlSend(WiFiClient* thisClient, const char* thisHost, int thisPort);
// int htmlSend(WiFiClient* thisClient, const char* thisHost, int thisPort, String sendHeader);

int htmlSend(const char* thisHost, int thisPort, String sendHeader);

int doAcceptTermsAndConditions();

int confirmedInternetConnectivity(const char* host);

#endif

