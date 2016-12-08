// 
// 
// 

#include "DashboardClient.h"

DashboardClient::DashboardClient() {

}

void DashboardClient::whitespace(char c) {
	Serial.println("whitespace");
}

void DashboardClient::startDocument() {
	Serial.println("start document");
}

void DashboardClient::key(String key) {
	currentKey = String(key);
	valueIs_dashboard_id = false;
	if (key == "dashboard_id") {
		valueIs_dashboard_id = true;
	}
	Serial.println("key: " + key);
}

void DashboardClient::value(String value) {
	Serial.println("value: " + value);
	if (valueIs_dashboard_id) {
		thisDashboardID = value.toInt;
	}
}

void DashboardClient::endArray() {
	Serial.println("end array. ");
}

void DashboardClient::endObject() {
	Serial.println("end object. ");
}

void DashboardClient::endDocument() {
	Serial.println("end document. ");
}

void DashboardClient::startArray() {
	Serial.println("start array. ");
}

void DashboardClient::startObject() {
	Serial.println("start object. ");
}
