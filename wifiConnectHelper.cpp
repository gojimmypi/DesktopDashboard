// 
// wifiConnectHelper
// 
#include <ESP8266WiFi.h>
#include "wifiConnectHelper.h"


const int MAX_CONNECTION_TIMEOUT_MILLISECONDS = 8000;

const int wifiConnectHelperDebugLevel = 3; // 3 is maximum verbose level

//
// Use WiFiClient class to create TCP connections
WiFiClient client;
unsigned long wifiConnectHelperTimeout = millis();
//**************************************************************************************************************
//**************************************************************************************************************
String htmlTagValue(String  thisHTML, String thisTag) {
	String tagStart = "<" + thisTag + ">";
	String tagEnd = "</" + thisTag + ">";
	int startPosition = thisHTML.indexOf(tagStart);
	Serial.print("START = ");
	Serial.println(startPosition);
	int endPosition = thisHTML.indexOf(tagEnd);
	return thisHTML.substring(startPosition, endPosition);
}

// test
String testHTML() {
	char * myBuffer;
	int mySize;
	// * stream = client.readBytes(*myBuffer, mySize);

	client.readBytes(myBuffer, mySize);
}

//**************************************************************************************************************
//  htmlSend - send thisHeader to targetHost:targetPort
//
//  return codes:   0 success
//                  1 failed to connect
//                  2 client timeout (see MAX_CONNECTION_TIMEOUT_MILLISECONDS)
//**************************************************************************************************************
int htmlSend(const char* targetHost, int targetPort, String sendHeader) {
	int countReadResponseAttempts = 5; // this a is a somewhat arbitrary number, mainly to handle large HTML payloads
	String thisResponse; thisResponse = "";
	String thisResponseHeader; thisResponseHeader = "";

	Serial.println("*****************************************************************");
	Serial.print("Connecting to ");
	Serial.println(targetHost); // e.g. www.google.com

	if (!client.connect(targetHost, targetPort)) {
		Serial.println("connection failed");
		return 1;
	}

	Serial.println("Sending HTML: ");
	Serial.println(sendHeader);

	// BEGIN TIME SENSITIVE SECTION (edit with care, don't waste CPU, yield to OS!)
	client.flush(); // discard any incoming data
	client.print(sendHeader); //blocks until either data is sent and ACKed, or timeout occurs (currently hard-coded to 5 seconds). 
	wifiConnectHelperTimeout = millis();
	while (client.available() == 0) {
		yield(); // give the OS a little breathing room 
		if ((millis() - wifiConnectHelperTimeout) > MAX_CONNECTION_TIMEOUT_MILLISECONDS) {
			Serial.println(">>> Client Timeout !");
			client.stop();
			return 2;
		}
		delay(100); // TODO does delay offer any benefit over yield() ?
	}

	bool endOfHeader = false;
	int thisLineCount = 0;
	while ((countReadResponseAttempts > 0)) {
		// Read all the lines of the reply from server and print them to Serial
		while (client.available()) {
			delay(1); // TODO - which is better: yield or delay?
			String line = client.readStringUntil('\r');
			yield(); // give the OS a little breathing room when loading large documents
			
		    //Serial.print(".");

			if ((endOfHeader)) { thisResponse += line; } // always capture the response
			if ((!endOfHeader) && (wifiConnectHelperDebugLevel >= 2)) { thisResponseHeader += line; } // capture header only for debug level 2 or greater
			if (line.length() <= 1) {
				endOfHeader = true;
			}
			thisLineCount++;
		}
		//Serial.print("(");
		//Serial.print(countReadResponseAttempts);
		//Serial.print(")");
		countReadResponseAttempts--; // TODO - do we stil need to look for more data like this?
		delay(100); // give the OS a little breathing room when loading large documents
	}
	// END TIME SENSITIVE SECTION 

	Serial.println("Response Header:");
	Serial.println("");
	Serial.println(thisResponseHeader);
	Serial.println("");
	if (wifiConnectHelperDebugLevel >= 2) { // only show the response content for debug level 2 or greater
		Serial.println("Response Payload Content:");
		Serial.println("");
		Serial.println(thisResponse);
		Serial.println("");
	}
	//Serial.println("Page Title=" + htmlTagValue(thisResponse, "TITLE") );
	Serial.println("");
	Serial.println("");
	Serial.print("Read done! Additional Read Responses:");
	Serial.println(countReadResponseAttempts);
	Serial.print("*** End of response. ");
	Serial.print(thisLineCount);
	Serial.println(" lines. ");
}


