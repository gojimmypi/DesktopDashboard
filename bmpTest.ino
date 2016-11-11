#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#include "debughandler.h"
#include "settings.h"

#define RUNLED 4 //GPIO 4
#define BACKLIGHT 5 //GPIO 5
#define BLINKTIME 250 //ms
#define WDTTIME WDTO_0MS //8sec
#define CONNECTTIMEOUT 20 //x500ms
#define COMTIMEOUT 2000 //ms
#define CONNECTRETRY 5 //times
#define UPDATEURL "http://b203.dk/lcds/firmware.bin"
#define VERSIONURL "http://b203.dk/lcds/firmwareversion"

extern struct SettingsStruct settings;

MDNSResponder mdns;

ESP8266WebServer server(80);
HTTPClient http;
unsigned long blinkTime,GETtimer;
bool softAP = false;
int numSsid;
uint8_t connectRetryCount;


void setup() {

  connectRetryCount = 0;
  numSsid = -1;
  
  Serial.begin(115200);
  delay(10);
  Serial.println('\n');
    
  lcd_init();
  initConfig();
  debug_init();

  debugPrint("WiFiLCD\nFW v");
  debugPrint(FWVERSION);
  debugPrintln("\n");

  pinMode(BACKLIGHT,OUTPUT);
  digitalWrite(BACKLIGHT,HIGH);
  
  if(settings.runLED)
  {
    pinMode(RUNLED,OUTPUT);
    digitalWrite(RUNLED,LOW);
  }
  
  connectWifi();

  checkUpdate();
    
  server_init();
  debugPrintln("HTTP server started");
 
}
    

void loop() {
  if (WiFi.status() != WL_CONNECTED && !softAP)
  {
    connectWifi();
  }
    
  analogWrite(BACKLIGHT,settings.backlight);
  
  if(settings.runLED && millis()>blinkTime)
  {
    blinkTime += BLINKTIME;
    if(digitalRead(RUNLED))
      digitalWrite(RUNLED,LOW);
    else
      digitalWrite(RUNLED,HIGH);
  }
  
//  yield();
  server.handleClient();
  
  if(settings.autoGET && millis()>GETtimer)
  {
    GETtimer += settings.getInterval*1000;
    delay(100);
    
    Serial.print("Requesting URL: ");
    Serial.println(settings.url);
    http.begin(settings.url);
    int httpCode = http.GET();
         
    if(httpCode == HTTP_CODE_OK)
    {
      WiFiClient * stream = http.getStreamPtr();
      if(bmpReadHeader(*stream))
     {
       Serial.println("BMP header accepted. Drawing file");
       bmpDraw(*stream);
     }
     else
       Serial.print("BMP header rejected!!");
    }
    else
    {
      char buf[32];
      sprintf(buf,"Failed, error: %s\n", http.errorToString(httpCode).c_str());
      debugPrintln(buf,ILI9341_RED);
    }
  http.end();
  }
}

void checkUpdate()
{
  if(WiFi.status() == WL_CONNECTED) {
        debugPrintln("Checking for \nfirmware update...");
        HTTPClient http;
        http.begin(VERSIONURL);
        String payload = "";
        if(http.GET() == HTTP_CODE_OK) {
          payload = http.getString();
          payload.trim();
        }
        if (payload.equals(FWVERSION)) {
          debugPrintln("Up to date!");
          return;
        }

        char buf[64];
        sprintf(buf,"\nUpdating to v%s..",payload.c_str());
        debugPrintln(buf);
        
        t_httpUpdate_return ret = ESPhttpUpdate.update(UPDATEURL);

        if(ret == HTTP_UPDATE_FAILED){
          sprintf(buf,"HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          debugPrintln(buf);
        }
    }  
}

void connectWifi()
{
 
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(200);
  debugPrintln("Scanning WiFi...");
  numSsid = WiFi.scanNetworks();
  delay(200);
  if(settings.APmode)
  {
    createAP(settings.ssid,settings.password);
    return;
  }
  
  bool APfound = false;
  
  Serial.println("Found:");
  for(int thisNet = 0; thisNet < numSsid; thisNet++) 
  {
    Serial.println(WiFi.SSID(thisNet) + "(" + WiFi.RSSI(thisNet) +")" );
    if(WiFi.SSID(thisNet).compareTo(settings.ssid) == 0)
      APfound = true;
  }
      
  if(!APfound)
  {
    debugPrint(settings.ssid,ILI9341_RED);
    debugPrintln(" Not found!",ILI9341_RED);
    createAP("WiFiLCD Recovery",0);
    return;
  }

  while(WiFi.status() != WL_CONNECTED && connectRetryCount++ < CONNECTRETRY)
  {
    debugPrintln("Connecting to:");
    debugPrintln(settings.ssid);
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(settings.ssid, settings.password);
    
    uint8_t cnt = 0;
    while (WiFi.status() != WL_CONNECTED && cnt < CONNECTTIMEOUT) {
      ESP.wdtFeed();
      delay(500);
      debugPrint(".");
      cnt++;
    }
    if(cnt >= CONNECTTIMEOUT)
    {
      debugPrintln("Error connecting",ILI9341_RED);
    }
  }
  if(WiFi.status() != WL_CONNECTED)
  {
    createAP("WiFiLCD Recovery",0);
    return;
  }
  
  debugPrintln("");
  debugPrintln("WiFi connected");  
  debugPrintln("IP address: ");
  char ip[17];
  sprintf(ip,"%d.%d.%d.%d\n",WiFi.localIP()[0],WiFi.localIP()[1],WiFi.localIP()[2],WiFi.localIP()[3]);
  debugPrintln(ip);
  if (mdns.begin(settings.name)) 
  {
    debugPrintln("mDNS started");  
    char name[16];
    sprintf(name,"%s.local\n",settings.name);
    debugPrintln(name);
  }
}

void createAP(const char*  ssid, const char* password)
{
  WiFi.disconnect();
  delay(200);
  softAP=true;
  debugPrintln("Creating softAP:");
  debugPrintln(ssid);
  WiFi.mode(WIFI_AP);
  if(password == 0)
    WiFi.softAP(ssid);
  else
    WiFi.softAP(ssid, password);
 
  debugPrintln("IP address: ");
  char ip[17];
  sprintf(ip,"%d.%d.%d.%d",WiFi.softAPIP()[0],WiFi.softAPIP()[1],WiFi.softAPIP()[2],WiFi.softAPIP()[3]);
  debugPrintln(ip);
}



