// GlobalDefine.h

#ifndef _GLOBALDEFINE_h
#define _GLOBALDEFINE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


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
static const char* DASHBOARD_APP = "/theDashboardApplicationPath/";
static const char* DASHBOARD_HOST = "mydashboardhost.com";
static const char* DASHBOARD_KEY = "XYZZY";
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

// #define SCREEN_DEBUG // when defined, display low level screen debug info 
// #define SCREEN_DATA_DEBUG // when defined, print screen data also to serial terminal
#define JSON_DEBUG // when defined, display JSON debug info 
#define WIFI_DEBUG // when defined, display WiFi debug info 
#define SERIAL_SCREEN_DEBUG // when defined, display screen messages to serial port
#define HTTP_DEBUG // when defined, display WiFi debug info 
#define DEBUG_SEPARATOR "***********************************"
#define TIMER_DEBUG // when defined, display diagnostic timer info
#define HEAP_DEBUG // when defined, display diagnostic heap info
#define HARDWARE_DEBUG

// TODO replace WIFI_CLIENT_CLASS with actual type in code was BearSSL confirmed to be functioning properly

#define USE_TLS_SSL // when defined, JSON data will use SSL
#define WIFI_CLIENT_CLASS WiFiClient  // the name of the WiFi class may vary depending on (1) architecture and (2) using TLS or not

#undef FOUND_BOARD
#ifdef ARDUINO_ARCH_ESP8266
    #undef  WIFI_CLIENT_CLASS
    #ifdef USE_TLS_SSL
        #define WIFI_CLIENT_CLASS BearSSL::WiFiClientSecure // BearSSL :: WiFiClientSecure
    #else
        #define THE_CLIENT_TYPE WiFiClient // no TLS 
    #endif // USE_TLS_SSL

    //#define WIFI_CLIENT_CLASS  WiFiClientSecure //  WiFiClientSecure
    #define FOUND_BOARD ESP8266
#endif

#ifdef ARDUINO_ARCH_ESP32
// #define WIFI_CLIENT_CLASS BearSSL::WiFiClientSecure // BearSSL :: WiFiClientSecure not supprted in ESP32 ?
#undef  WIFI_CLIENT_CLASS
#define WIFI_CLIENT_CLASS  WiFiClientSecure //  WiFiClientSecure default
#define FOUND_BOARD ESP32
#endif

#ifndef FOUND_BOARD
#pragma message(Reminder "Error Target hardware not defined !")
#endif // ! FOUND_BOARD



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

// static const char * DEFAULT_DEBUG_MESSAGE = ""; // when using the default (this empty string), the respective debug message will use the default text
												//static String DEFAULT_DEBUG_MESSAGE = ""; // when using the default (this empty string), the respective debug message will use the default text
												//********************************************************
												// some optional Serial.print() statements...
												//********************************************************
#ifdef WIFI_DEBUG
#define WIFI_DEBUG_PRINT(string)           (Serial.print(string))
#define WIFI_DEBUG_PRINTLN(string)         (Serial.println(string))
#endif

#ifndef WIFI_DEBUG
#define WIFI_DEBUG_PRINT(string)           ((void)0)
#define WIFI_DEBUG_PRINTLN(string)         ((void)0)
#endif

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

//static char * HEAP_DEBUG_MSG = "Heap = ";
//#define HEAP_DEBUG_PRINT(string)           (Serial.print  ( (string == DEFAULT_DEBUG_MESSAGE) ? (HEAP_DEBUG_MSG + (String)ESP.getFreeHeap()) : string ) )
//#define HEAP_DEBUG_PRINTLN(string)         (Serial.println( (string == DEFAULT_DEBUG_MESSAGE) ? (HEAP_DEBUG_MSG + (String)ESP.getFreeHeap()) : string ) )
//#define HEAP_DEBUG_PRINTF(string,uint32_t) (Serial.printf (  string,uint32_t)                                                   )

#define DEFAULT_DEBUG_MESSAGE DefaultDebugMessage()
#define SET_HEAP_MESSAGE(thisStr)	       (setHeapMsg(thisStr))
#define HEAP_DEBUG_PRINT(thisStr)          (Serial.print  (  DefaultDebugMessage().compareTo(thisStr)  ? (getHeapMsg() + (String)ESP.getFreeHeap()) : thisStr ) )
#define HEAP_DEBUG_PRINTLN(thisStr)        (Serial.println(  DefaultDebugMessage().compareTo(thisStr)  ? (getHeapMsg() + (String)ESP.getFreeHeap()) : thisStr ) )
#define HEAP_DEBUG_PRINTF(string,uint32_t) (Serial.printf (  string,uint32_t)                                                  )
#endif

#ifndef HEAP_DEBUG
static const char *  HEAP_DEBUG_MSG = "";
#define SET_HEAP_MESSAGE(string)		   ((void)0)
#define HEAP_DEBUG_PRINT(string)           ((void)0)
#define HEAP_DEBUG_PRINTF(string)          ((void)0)
#define HEAP_DEBUG_PRINTLN(string)         ((void)0)
#endif

 
	void setHeapMsg(String str);

	String getHeapMsg();

	String DefaultDebugMessage();

 


#endif // _GLOBALDEFINE_h

