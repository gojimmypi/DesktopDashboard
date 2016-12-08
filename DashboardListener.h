// DashboardListener.h

#ifndef _DASHBOARDLISTENER_h
#define _DASHBOARDLISTENER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#pragma once

#include "JsonListener.h"

class DashboardListener : public JsonListener {

public:
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

