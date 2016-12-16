// DashboardClient.h

#ifndef _DASHBOARDCLIENT_h
#define _DASHBOARDCLIENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#pragma once

#include <JsonListener.h>
#include <JsonStreamingParser.h>



#include <vector>
class DashboardClient : public JsonListener {
private:
	String currentKey;
	int thisDashboardID;
	bool foundNewObject = false; // when we find a new ID, the very next field *MUST* be its value (ID must be first item in JSON)


	struct DashboardItem {
		int itemID = 0;
		int dashboard_id = 0;
		String validation_key = "";
		String dashboard_short_summary = "";
		String dashboard_long_summary = "";
		String current_value = "";
		String target_value = "";
		String current_value_display = "";
		String units = "";
		DashboardItem* next = NULL; // we have a linked list of dashboard items
	};
	int itemCount = 0;
	DashboardItem* headItem = new DashboardItem; // for a great tutorial on linked lists, see https://www.youtube.com/watch?v=o5wJkJJpKtM
	DashboardItem* thisItem = headItem;
	DashboardItem* nextItem = headItem;

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

