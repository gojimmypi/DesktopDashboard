// 
// 
// 
#include "GlobalDefine.h"

#include "timeHelper.h"


// see https://www.epochconverter.com/
#define RTC_TEST 1537304154 // = Monday 13 November 2017 17:07:05 UTC

// *******************************************************************************************
void setupLocalTime()
// *******************************************************************************************
{
	time_t now;
	now = time(nullptr);
	Serial.print("Initial time:"); Serial.println(now);
	Serial.println(ctime(&now));

	int myTimezone = 8;
	int dst = 0;

	if (now <= 1500000000) {
		// try to set network time
		struct tm timeinfo;
		configTime(myTimezone * 3600, dst * 0, "pool.ntp.org", "time.nist.gov");
		Serial.println("\nWaiting for time");
		while (!time(nullptr)) {
			Serial.print(".");
			delay(1000);
		}
	}

	now = time(nullptr);
	Serial.print("Next time:");  Serial.println(now);
	Serial.println(ctime(&now));

	if (now <= 1500000000) {
		// set to RTC text value
		timeval tv = { RTC_TEST, 0 };
		timezone tz = { 0 , 0 };
		settimeofday(&tv, &tz);
		setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 3);
		tzset();
	}

	now = time(nullptr);
	Serial.println("Final time:");  Serial.println(now);
	Serial.println(ctime(&now));
}
