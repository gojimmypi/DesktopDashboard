# DesktopDashboard

Desktop Dashboard using ESP8266 and ILI9341 display, as seen here:

https://www.youtube.com/watch?v=TmvaU6EQsAc

What's a Desktop Dashboard? Well, I work as a Senior Software Engineer. The management staff wanted a "better, easier" method for viewing key business metrics. Although I've created SQL SSAS OLAP cubes using XLS as a front end, many web-based Crystal Reports, and an entire internal web site... well, there's still a desire for an even simpler dashboard. This solution? No login. No computer. No fussing with a phone. Just something that sits on a desk and displays dashboard business metrics.

I was inspired by the weather station:

https://github.com/squix78/esp8266-weather-station

and decided to take the idea further than just weather. My first prototype was met with "wow, that's cool - but the display is so small". So I moved on to the ILI9341 display.

The plan is to add staff productivity, budget status, staff counts, even IT security metrics. All of this will be sent via user-specific JSON to the Desktop Dashboard device. Think of it as a desktop clock, with business-relevant news!

The one issue with the ESP8266 - is that with the low price tag, it has limited resources. It appears that even something as simple as jpg decompression is beyond the scope of possibilities. So I wrote an image conversion utility to take a variety  of image formats and convert them to the simplest BMP:

https://github.com/gojimmypi/imageConvert2BMP

This version incorprates my "Visitor WiFi" library, that programmatically presses the "I accept the terms and conditions" button on a redirect page, before actually granting access to the internet:

https://github.com/gojimmypi/VisitorWiFi-ESP8266

Included in that library are a variety of useful functions for getting HTML tag values, parsing headers, reading query strings, and more.

Note that if you download the entire project and compile this with Visual Micro add-in for Visual Studio, you'll likely need to edit the DesktopDashboard.vcxproj file, replacing all the instances of C:\Users\gojimmypi... with C:\Users\YourLoginName  (does anyone know of a more graceful way to share Visual Studio projects on github?). Hopefully your Arduino libs are also installed by default at a location like:

C:\Users\YourLoginName\AppData\Local\arduino15\packages\esp8266\

This project is developed with the VisualMicro add-in for Visual Studio 2017 with the Arduino IDE supporting libraries.

In Arduino IDE: File - Preferences Additional Board Manager 

Enter http://arduino.esp8266.com/stable/package_esp8266com_index.json 

In Arduino IDE: Tools - Board - Board Manager. 

  Install package: ESP8266 by ESP8266 Community 

In Visual Studio, vMicro - Board - NodeMCU 0.9
(may need to Visual Studio Menu: vMicro - General - Rescan Toolchains and Libraries)
  

Libraries needed (and how to install them):

 https://github.com/squix78/json-streaming-parser
 Until the library becomes available in the Arduino IDE library manager you'll have to do a bit more work by hand.

  1.Download this library : https://github.com/squix78/json-streaming-parser/archive/master.zip

  2.Rename master.zip to json - streaming - parser.zip

  3.Open the zip file in the Arduino IDE from menu Sketch > Include Library > Add ZIP Library...


Also needed (installed typically in Arduino IDE menu Sketch > Include Library > Manage Libraries)

  Adafruit ILI9341 display library (1.0.2 original; 1.0.11 confirmed still working)

  Adafruit GFX Library (1.1.5 original; 1.2.3 confirmed still working)

When updating libraries, don't forget: Visual Studio Menu: vMicro - General - Rescan Toolchains and Libraries

To compile: vMicro  - uploader - port (enter your COMx port, see Device Manager)

You may need to manually edit DesktopDashboard.vcxproj and replace any instances of gojimmypi with your login name in the library paths.

Application settings are found in two files:

In the application, there is a [GlobalDefine.h](https://github.com/gojimmypi/DesktopDashboard/blob/master/GlobalDefine.h) that contains compiler-specific settings.

In c:\workspace-git\ it looks for a file called myPrivateSettings.h with these values:
```
// myPrivateSettings.h

#ifndef _MYPRIVATESETTINGS_h
#define _MYPRIVATESETTINGS_h

static const char* WIFI_SSID = "value";
static const char* WIFI_PWD = "value!";

static const char* DASHBOARD_DEFAULT_DATA = "/path/sampledata.json";
static const char* DASHBOARD_PATH = "/DashboardDataDog/";
static const char* DASHBOARD_APP = "/myDashbaord/";
static const char* DASHBOARD_HOST = "fqdn.com";
static const char* DASHBOARD_KEY = "value";


// TimeClient settings
const float UTC_OFFSET = -7;

// Wunderground Settings
const boolean IS_METRIC = false;
const String WUNDERGRROUND_API_KEY = "value";
const String WUNDERGRROUND_LANGUAGE = "EN";
const String WUNDERGROUND_COUNTRY = "CA";
const String WUNDERGROUND_CITY = "value";

//Thingspeak Settings
const String THINGSPEAK_CHANNEL_ID = "value";
const String THINGSPEAK_API_READ_KEY = "value";
#endif
```
More coming soon... (dev)

* Note that wunderground is no longer giving away. See [this link](https://twitter.com/thingpulse/status/1006419150057689089)
