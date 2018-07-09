// 
// 
// 

#include "GlobalDefine.h"
#include "sslHelper.h"

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


void testSSL() {

	THE_SSL_TYPE client;


	Serial.println("TestSSL");
	SET_HEAP_MESSAGE("testSSL");
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	const char* host = "api.github.com";
	const int httpsPort = 443;
	// Use web browser to view and copy
	// SHA1 fingerprint of the certificate
	const char* fingerprint = "5F F1 60 31 09 04 3E F2 90 D2 B0 8A 50 38 04 E8 37 9F BC 76"; //"35 85 74 EF 67 35 A7 CE 40 69 50 F3 C0 F6 80 CF 80 3B 2E 19";

	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	// Use WiFiClientSecure class to create TLS connection
	// WiFiClientSecure client;
	Serial.print("connecting to ");
	Serial.println(host);
	if (!client.connect(host, httpsPort)) {
		Serial.println("TLS connection failed");
		delay(10000);
		return;
	}


#ifdef ARDUINO_ARCH_ESP8266
	if (client.verify(fingerprint, host)) {
		Serial.println("TLS certificate matches");
}
	else {
		Serial.println("TLS client.verify not implemented in ESP32");
	}
#endif

#ifdef ARDUINO_ARCH_ESP32
	Serial.println("TLS certificate doesn't match");
#endif

	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);

	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	String url = "/repos/esp8266/Arduino/commits/master/status";
	Serial.print("requesting URL: ");
	Serial.println(url);
	Serial.println(DEBUG_SEPARATOR);

	String tlsHTML = String("GET ") + url + " HTTP/1.1\r\n" +
		"Host: " + host + "\r\n" +
		"User-Agent: BuildFailureDetectorESP8266\r\n" +
		"Connection: close\r\n\r\n";
	Serial.println("tlsHTML:");
	Serial.println(DEBUG_SEPARATOR);
	Serial.println(tlsHTML);
	Serial.println(DEBUG_SEPARATOR);

	client.print(tlsHTML);

	Serial.println("request sent");
	HEAP_DEBUG_PRINT(getHeapMsg()); HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	while (client.connected()) {
		String line = client.readStringUntil('\n');
		if (line == "\r") {
			Serial.println("headers received");
			break;
		}
	}
	String line = client.readStringUntil('\n');
	if (line.startsWith("{\"state\":\"success\"")) {
		Serial.println("esp8266/Arduino CI successfull!");
	}
	else {
		Serial.println("esp8266/Arduino CI has failed");
	}
	Serial.println("reply was:");
	Serial.println(DEBUG_SEPARATOR);
	Serial.println(line);
	Serial.println(DEBUG_SEPARATOR);
	Serial.println("closing connection");
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	line = "";
	// client.stop(); // don't stop the local client here, as other instances will not be able to reconnect (TODO - have exactly one WiFiClientSecure client;)
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	Serial.println("Flush...");
	client.flush();
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
	Serial.println("Stop...");
	client.stop();
	HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
}


