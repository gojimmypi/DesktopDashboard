// DashboardClient.h

// https://github.com/squix78/json-streaming-parser
// Until the library becomes available in the Arduino IDE library manager you'll have to do a bit more work by hand.
//  1.Download this library : https://github.com/squix78/json-streaming-parser/archive/master.zip
//	2.Rename master.zip to json - streaming - parser.zip
//	3.Open the zip file in the Arduino IDE from menu Sketch > Include Library > Add ZIP Library...


#ifndef _DASHBOARDCLIENT_h
#define _DASHBOARDCLIENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma once

#include <JsonListener.h> // this is expected to be installed via Arduino IDE at someplace like: c:\Users\[username]\Documents\Arduino\libraries\Json_Streaming_Parser\JsonListener.h
#include <JsonStreamingParser.h>



#include <vector>
class DashboardClient : public JsonListener {
private:
	String currentKey;
	int thisDashboardID;
	bool foundNewObject = false; // when we find a new ID, the very next field *MUST* be its value (ID must be first item in JSON)


	struct DashboardItem {
		int itemID = 1;
		int dashboard_id = 0;
		String validation_key = "";
		String dashboard_short_summary = "Empty";
		String dashboard_long_summary = "Empty";
		String current_value = "Empty";
		String target_value = "1";
		String current_value_display = "0";
		String units = "No data";
		DashboardItem* next = NULL; // we have a linked list of dashboard items
	};
	int itemCount = 1;
	DashboardItem* headItem = new DashboardItem; // for a great tutorial on linked lists, see https://www.youtube.com/watch?v=o5wJkJJpKtM
	DashboardItem* thisItem = headItem;
	DashboardItem* nextItem = NULL;

public:
	DashboardClient();

	virtual void open(); 

	virtual bool available();

	virtual String read(); // when less then zero, read sequantially, pointing to next DashboardItem at position 2

    virtual String read(int displayLine); // when less then zero, read sequantially, pointing to next DashboardItem at position 2

	virtual void whitespace(char c);

	virtual void startDocument();

	virtual void key(String key);

	virtual void value(String value);

	virtual void endArray();

	virtual void endObject();

	virtual void endDocument();

	virtual void startArray();

	virtual void startObject();
};
#endif

