// htmlHelper.h

#ifndef htmlHelper_h_
#define htmlHelper_h_

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define USE_TLS_SSL // when defined, JSON data will use SSL

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

// int htmlSend(WiFiClient* thisClient, const char* thisHost, int thisPort);
// int htmlSend(WiFiClient* thisClient, const char* thisHost, int thisPort, String sendHeader);

int htmlSend(const char* thisHost, int thisPort, String sendHeader);


int doAcceptTermsAndConditions();

int confirmedInternetConnectivity(const char* host);
extern const char* httpText; // = "http://"; // this is defined once here to allow easy transition from http:// to https://

bool htmlExists(String targetURL);

class htmlHelper {

	// depending on the USE_TLS_SSL the myClient is either WiFiClient or WiFiClientSecure (but they are different! not sure secure or not...)

#ifdef USE_TLS_SSL
	WiFiClientSecure* myClient;
#else
	WiFiClient* myClient;
#endif
	const char* thisHost;
	int thisPort;
	String sendHeader;

public:

#ifdef USE_TLS_SSL
	htmlHelper(WiFiClientSecure*, const char*, int, String);
	htmlHelper(WiFiClientSecure*, const char*, int);
#else
	htmlHelper(WiFiClient*, const char*, int, String);
	htmlHelper(WiFiClient*, const char*, int);
#endif
	htmlHelper();
	int Send();
};

#endif

