// GlobalDefine.h

#pragma once
#ifndef _GLOBALDEFINE_h
#define _GLOBALDEFINE_h

//*******************************************************************************************************************************************
// Begin user config
//*******************************************************************************************************************************************
// My config is stored in myPrivateSettings.h file 
// if you choose not to use such a file, set this to false:
#define USE_myPrivateSettings true

// Note the two possible file name string formats.
#if USE_myPrivateSettings == true 
#include "/workspace-git/myPrivateSettings.h"
#else
#pragma message(Reminder "Settings needed !")
// create your own myPrivateSettings.h, or update the following lines:
static const char* WIFI_SSID = "my-wifi-SSID"
static const char* WIFI_PWD = "my-WiFi-PASSWORD"

static const char* DASHBOARD_DEFAULT_DATA = "sampledata.json";
static const char* DASHBOARD_PATH = "/theDataPath/";
static const char* DASHBOARD_APP  = "/theDashboardApplicationPath/";
static const char* DASHBOARD_HOST = "mydashboardhost.com";
static const char* DASHBOARD_KEY  = "XYZZY";
static const char* DASHBOARD_HOST_THUMBPRINT = "35 85 74 EF 67 35 A7 CE 40 69 50 F3 C0 F6 80 CF 80 3B 2E 19";
// will build:  http://mydashboardhost.com/theDashboardApplicationPath/
//      and:    http://mydashboardhost.com/theDataPath/
// 
// we will fetch data from a JSON file called http://mydashboardhost.com/theDataPath/XYZZYMMAABBCCDDEEFF
// where MMAABBCCDDEEFF is this device's HEX MAC Address, with no spaces, dashes, or commas
#endif
//*******************************************************************************************************************************************
// End user config
//*******************************************************************************************************************************************



#define USE_TLS_SSL // when defined, JSON data will use SSL
// #define SCREEN_DEBUG // when defined, display low level screen debug info 
// #define SCREEN_DATA_DEBUG // when defined, print screen data also to serial terminal
#define JSON_DEBUG // when defined, display JSON debug info 
#define WIFI_DEBUG // when defined, display WiFi debug info 
#define SERIAL_SCREEN_DEBUG // when defined, display screen messages to serial port
#define HTTP_DEBUG // when defined, display WiFi debug info 
#define DEBUG_SEPARATOR "***********************************"
#define TIMER_DEBUG // when defined, display diagnostic timer info
#define HEAP_DEBUG
#define HARDWARE_DEBUG

// Statements like:
// #pragma message(Reminder "Fix this problem!")
// Which will cause messages like:
// C:\Source\Project\main.cpp(47): Reminder: Fix this problem!
// to show up during compiles. Note that you can NOT use the
// words "error" or "warning" in your reminders, since it will
// make the IDE think it should abort execution. You can double
// click on these messages and jump to the line in question.
//
// see https://stackoverflow.com/questions/5966594/how-can-i-use-pragma-message-so-that-the-message-points-to-the-filelineno
//
#define Stringize( L )     #L 
#define MakeString( M, L ) M(L)
#define $Line MakeString( Stringize, __LINE__ )
#define Reminder __FILE__ "(" $Line ") : Reminder: "

//********************************************************
// some optional Serial.print() statements...
//********************************************************
#ifdef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string)           (Serial.print(string))
#define HTTP_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string)           ((void)0)
#define HTTP_DEBUG_PRINTLN(string)         ((void)0)
#endif

//********************************************************
#ifdef SCREEN_DEBUG
#define SCREEN_DEBUG_PRINT(string)         (Serial.print(string))
#define SCREEN_DEBUG_PRINTLN(string)       (Serial.println(string))
#endif

#ifndef SCREEN_DEBUG
#define SCREEN_DEBUG_PRINT(string)         ((void)0)
#define SCREEN_DEBUG_PRINTLN(string)       ((void)0)
#endif

//********************************************************
#ifdef SCREEN_DATA_DEBUG
#define SCREEN_DATA_DEBUG_PRINT(string)    (Serial.print(string))
#define SCREEN_DATA_DEBUG_PRINTLN(string)  (Serial.println(string))
#endif

#ifndef SCREEN_DATA_DEBUG
#define SCREEN_DATA_DEBUG_PRINT(string)    ((void)0)
#define SCREEN_DATA_DEBUG_PRINTLN(string)  ((void)0)
#endif

//********************************************************
#ifdef JSON_DEBUG
#define JSON_DEBUG_PRINT(string)           (Serial.print(string))
#define JSON_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef JSON_DEBUG
#define JSON_DEBUG_PRINT(string)           ((void)0)
#define JSON_DEBUG_PRINTLN(string)         ((void)0)
#endif

//********************************************************
#ifdef TIMER_DEBUG
#define TIMER_DEBUG_PRINT(string)           (Serial.print(string))
#define TIMER_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef TIMER_DEBUG
#define TIMER_DEBUG_PRINT(string)           ((void)0)
#define TIMER_DEBUG_PRINTLN(string)         ((void)0)
#endif

//********************************************************
#ifdef HEAP_DEBUG
#define HEAP_DEBUG_PRINT(string)           (Serial.print(string))
#define HEAP_DEBUG_PRINTF(string,uint32_t) (Serial.printf(string,uint32_t))
#define HEAP_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef HEAP_DEBUG
#define HEAP_DEBUG_PRINT(string)           ((void)0)
#define HEAP_DEBUG_PRINTF(string)          ((void)0)
#define HEAP_DEBUG_PRINTLN(string)         ((void)0)
#endif


#endif // _GLOBALDEFINE_h

