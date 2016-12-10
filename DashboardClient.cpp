// 
// 
// 
#include "DashboardClient.h"


DashboardClient::DashboardClient() {

}

//**************************************************************************************************************
// 
//**************************************************************************************************************
void DashboardClient::open() {
	// this is a common syntax, to "open" something for reading
	thisItem = headItem; // set our linked list item to the first (header) item
}

//**************************************************************************************************************
// 
//**************************************************************************************************************
bool DashboardClient::available() {
	return !(thisItem == NULL) ; // the default "next item" is NULL, so when thisItem == NULL, we no longer have data "available" to read
}

//**************************************************************************************************************
// 
//**************************************************************************************************************
int readCounter = 0;
String  DashboardClient::read() {
	return read(-1);
}

String  DashboardClient::read(int displayLine) {
	String result = "";
	if (thisItem == NULL) { return result; } // return blank if there's no item

	switch (readCounter)
	{
	case 0:
		result = thisItem->dashboard_short_summary;
		readCounter++;
		break;
	case 1:
		result = thisItem->current_value;
		readCounter++;
		break;
	case 2:
		result = thisItem->units;
		readCounter = 0;
		thisItem = thisItem->next; // when reading last item, go to next one
		break;
	default:
		break;
	}
	return result;
}

//**************************************************************************************************************
// 
//**************************************************************************************************************
void DashboardClient::whitespace(char c) {
	 
	//Serial.print(thisDashboardID);
	//Serial.println(" whitespace");
}

//**************************************************************************************************************
// 
//**************************************************************************************************************
void DashboardClient::startDocument() {
	itemCount = 0;       // at the beginning, we have no items (other than our header)
	thisItem = headItem; // set out linked list to the first (header) item
    //
	//	Serial.println(" start document");
}

//**************************************************************************************************************
// 
//**************************************************************************************************************
void DashboardClient::key(String key) {
#include <vector>
	currentKey = String(key);
	if (key == "dashboard_id") {
		if (!foundNewObject) {
			Serial.println("Warning: dashboard id found at position other than start of object!");
		}
		//else { Serial.println("ID OK!"); }
	}
	foundNewObject = false;
	//Serial.print(thisDashboardID);
	//Serial.println(" key: " + key);
}

//**************************************************************************************************************
// 
//**************************************************************************************************************
void DashboardClient::value(String value) {
//	Serial.println("value: " + value);
	if (currentKey == "dashboard_id") {
		thisDashboardID = value.toInt();
		//std::vector<int> array;
		//DashboardItemArray.push_back(thisDashboardID);
		itemCount++;
		if (thisItem->next == NULL) {
			//Serial.print("Creating item ");
			//Serial.println(itemCount);
			nextItem = new DashboardItem;
			thisItem->next = nextItem;
		}
		// else { Serial.println("Item already exists"); }
		thisItem->itemID = itemCount;
		thisItem->dashboard_id = thisDashboardID;
	}
	if (currentKey == "dashboard_short_summary") {
		thisItem->dashboard_short_summary = value;
	}
	if (currentKey == "current_value") {
		thisItem->current_value = value;
	}
	if (currentKey == "units") {
		thisItem->units = value;
	}
	
}

//**************************************************************************************************************
// 
//**************************************************************************************************************
void DashboardClient::endArray() {
	//Serial.print(thisDashboardID);
	//Serial.println(" end array. ");
}

//**************************************************************************************************************
// 
//**************************************************************************************************************
void DashboardClient::endObject() {
	foundNewObject = false;
	thisItem = thisItem->next; // or thisItem = NextItem

	//Serial.print(thisDashboardID);
	//Serial.println(" end object. ");
}

//**************************************************************************************************************
// 
//**************************************************************************************************************
void DashboardClient::endDocument() {
	thisItem = headItem;

	//Serial.print(thisDashboardID);
	//Serial.println(" end document. ");

	//while (thisItem != NULL) {
	//	Serial.print("Found: ID=");
	//	Serial.print(thisItem->dashboard_id);
	//	Serial.print(" text = ");
	//	Serial.println(thisItem->dashboard_short_summary);
	//	thisItem = thisItem->next;
	//}
}

//**************************************************************************************************************
// 
//**************************************************************************************************************
void DashboardClient::startArray() {
	//Serial.print(thisDashboardID);
	//Serial.println(" start array. ");
}

//**************************************************************************************************************
// 
//**************************************************************************************************************
void DashboardClient::startObject() {
	foundNewObject = true;
	//Serial.print(thisDashboardID);
	//Serial.println(" start object. ");
}
