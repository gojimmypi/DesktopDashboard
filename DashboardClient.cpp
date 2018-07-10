// 
// 
// 
#include "DashboardClient.h"
#include "GlobalDefine.h"

const int MIN_HEAP_LIMIT = 3000; // Set a limit on how much heap space can be used for our linked list of objects

DashboardClient::DashboardClient() {

}

//**************************************************************************************************************
// 
//**************************************************************************************************************
void DashboardClient::open() {
	// this is a common syntax, to "open" something for reading
    // Note there's always at least one item, even if it says there's no data; we have something to display
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
		result = thisItem->current_value_display;
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
	itemCount = 1;       // at the beginning, we may have no data items (other than our header)
	thisItem = headItem; // set out linked list to the first (header) item
    //
	//	Serial.println(" start document");
}

//**************************************************************************************************************
// 
//**************************************************************************************************************
void DashboardClient::key(String key) {
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
        JSON_DEBUG_PRINT("Looking at item DashboardID = ");
        JSON_DEBUG_PRINTLN(thisDashboardID);
        if (thisItem == NULL) {
            JSON_DEBUG_PRINT("ERROR: thisItem is null!");
            return; 
        }
		if (thisItem->next == NULL) {
			if (ESP.getFreeHeap() > MIN_HEAP_LIMIT) {
                JSON_DEBUG_PRINT("Creating item #");
                JSON_DEBUG_PRINT(itemCount);
                JSON_DEBUG_PRINT(" for ID = ");
                JSON_DEBUG_PRINTLN(thisDashboardID);
                HEAP_DEBUG_PRINTLN(DEFAULT_DEBUG_MESSAGE);
                nextItem = new DashboardItem;
                itemCount++;
                thisItem->itemID = itemCount;
                thisItem->dashboard_id = thisDashboardID;
                JSON_DEBUG_PRINTLN("Creating item complete.");
                thisItem->next = nextItem;
            }
			else
			{
				HEAP_DEBUG_PRINT("Warning: Low memory detected!");
                JSON_DEBUG_PRINTLN(" Item not created.");
                thisItem->dashboard_long_summary = "Low memory";
				thisItem->dashboard_short_summary = "Low memory";
				thisItem->current_value_display = (String)ESP.getFreeHeap();
				thisItem->current_value = (String)ESP.getFreeHeap();
                thisItem->units = "Bytes Free";
                thisItem->next = NULL; // there's no next item when we are out of memory!
                nextItem = NULL;
                this->endObject(); // force end of object
                this->endDocument(); // end end of document
                return;
			}
		}
		else { 
            nextItem = thisItem->next;
            JSON_DEBUG_PRINT("Next item already exists. itemID =");
            JSON_DEBUG_PRINT(nextItem->itemID);
            JSON_DEBUG_PRINT("; dashboard id = ");
            JSON_DEBUG_PRINTLN(nextItem->dashboard_id);
        }
	}
    JSON_DEBUG_PRINT("Checking currentKey =");
    JSON_DEBUG_PRINTLN(currentKey);
	if (currentKey == "dashboard_short_summary") {
		thisItem->dashboard_short_summary = value;
	}
	if (currentKey == "current_value_display") {
		thisItem->current_value_display = value;
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
	thisItem = thisItem->next; // or thisItem = NextItem; note that at the end, the next item is Null

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
