// 
// 
// 

#include "DashboardListener.h"

#include "JsonListener.h"

#include <vector>

String currentKey;
int thisDashboardID;
//struct DashboardItem {
//	int dashboard_id;
//	String validation_key;
//	String dashboard_short_summary;
//	String dashboard_long_summary;
//};

// std::vector<DashboardItem> DashboardItems;
bool valueIs_dashboard_id;


void DashboardListener::whitespace(char c) {
	Serial.println("whitespace");
}

void DashboardListener::startDocument() {
	Serial.println("start document");
}

void DashboardListener::key(String key) {
	currentKey = String(key);
	valueIs_dashboard_id = false;
	if (key == "dashboard_id") {
		valueIs_dashboard_id = true;
	}
	Serial.println("key: " + key);
}

void DashboardListener::value(String value) {
	Serial.println("value: " + value);
	if (valueIs_dashboard_id) {
		thisDashboardID = value.toInt();
	}
}

void DashboardListener::endArray() {
	Serial.println("end array. ");
}

void DashboardListener::endObject() {
	Serial.println("end object. ");
}

void DashboardListener::endDocument() {
	Serial.println("end document. ");
}

void DashboardListener::startArray() {
	Serial.println("start array. ");
}

void DashboardListener::startObject() {
	Serial.println("start object. ");
}

