/*
  NTP-TZ-DST
  NetWork Time Protocol - Time Zone - Daylight Saving Time
  This example shows how to read and set time,
  and how to use NTP (set NTP0_OR_LOCAL1 to 0 below)
  or an external RTC (set NTP0_OR_LOCAL1 to 1 below)
  TZ and DST below have to be manually set
  according to your local settings.
  This example code is in the public domain.
*/

#include <ESP8266WiFi.h>
#include "timesetup.h"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "utils.h"
#include "alerts.h"
#include "buzzer.h"
#include "userwebsocket.h"

#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
////////////////////////////////////////////////////////
#define DEBUG
#ifdef DEBUG
#define debug(x) Serial.println(x);
#else
#define debug(x)
#endif

#define SSID            "kms-secure"
#define SSIDPWD         "wearecamo2016"
#define SSID2           "heppners-2"
#define SSIDPWD2        "Cannotcrackit"

// StaticJsonDocument<2500> doc;
// DynamicJsonDocument doc(1024);

// set buzzer port 
buzzer led = buzzer(LED_BUILTIN,true);
buzzer speaker = buzzer(D3,false);
ESP8266WiFiMulti wifiMulti;


void setup() {
  Serial.begin(115200);
  Serial.println(F("\nstart program"));
  Serial.println();

  LittleFSConfig cfg;
  cfg.setAutoFormat(false);
  LittleFS.setConfig(cfg);

  LittleFS.begin();

  File file = LittleFS.open(F("/config.json"),"r");
  size_t filesize = file.size();
  DynamicJsonDocument doc(filesize*2);
  deserializeJson(doc, file);
  file.close();
  doc.shrinkToFit();

  int numberSchedules = doc[F("Schedules")].size();

  for( int i = 0; i < numberSchedules; i++){
    Schedules.addSchedule(Schedule(doc[F("Schedules")][i].as<JsonObject>()));
  }

  for( int day = 0 ; day < 7; day++){
    dailyList[day].dayName =  doc[F("days")][day][F("name")].as<String>();
    for( size_t item = 0; item < doc[F("days")][day][F("schedule")].size(); item++){
      dailyList[day].AddSchedule(doc[F("days")][day][F("schedule")][item].as<JsonObject>());
    }
  }

  char* newName = (char*)"New Schedule";
  
  String time1 = "1:00";
  String time2 = "14:00";
  Schedules.addSchedule(Schedule(newName));
  Schedules[newName]->addAlert(&time2,3,AlertTone::SINGLE);

  wifiMulti.addAP(SSID,SSIDPWD);
  wifiMulti.addAP(SSID2,SSIDPWD2);
  while(wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(250);
    Serial.print('.');
  };
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(SSID, SSIDPWD);

  setup_timezones();

  ArduinoOTA.setHostname("Timeclock");
  ArduinoOTA.onStart([](){Serial.println("Starting OTA Programming");});
  ArduinoOTA.onEnd([](){Serial.println("OTA Programming has finished");});
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  ClearScreen();

  ws.onEvent(onEvent);
  server.addHandler(&ws);

  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send_P(200, "text/html", index_html, NULL);
  // });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS,"/index.html");
  });

  server.begin();

  pinMode(D6, OUTPUT);

  digitalWrite(D6, LOW);


}


timeval tv;
tm* tm_tm_now;
time_t tnow;
void testTime();
bool addedTime = false;

void loop() {

  ArduinoOTA.handle();

  speaker.update();
  led.update();

  static time_t nextMinute = 0;
  static time_t nextSecond = 0;
  tnow = time(nullptr);
  tm_tm_now = localtime(&tnow);
  int currentDay = tm_tm_now->tm_wday;
  int currentMinute = tm_tm_now->tm_min + tm_tm_now->tm_hour*60;
  char charTime[6];
  sprintf(charTime,"%2d:%02d",tm_tm_now->tm_hour, tm_tm_now->tm_min);

  //for testing add alert 1 mins after starting
  if (cbtime_set && !addedTime){

    char charAlert[6];
    sprintf(charAlert,"%2d:%02d",tm_tm_now->tm_hour, tm_tm_now->tm_min+1);
    String alertingTime = charAlert;
    debug("adding " + alertingTime);
    Schedules["Main"]->addAlert(&alertingTime,3,AlertTone::SINGLE);
    addedTime = true;
  }


  if((localtime(&tnow)->tm_sec == 0) && (tnow >= nextMinute)){
    nextMinute = tnow + 60;


    MoveCursorToLine(2);
    CSFromCursorDown();

		Serial.printf("Current Day: %d\n",currentDay);
		Serial.printf("Current Minute: %d\n",currentMinute);
    Serial.printf("Char Time: %s\n",charTime);
    
    uint32_t free;
    uint16_t max;
    uint8_t frag;
    ESP.getHeapStats(&free, &max, &frag);

    Serial.printf("free: %5d - max: %5d - frag: %3d%% <- \n\r", free, max, frag);
    dailyList[currentDay].print();

    Serial.print(WiFi.localIP());

    if(dailyList[currentDay].hasAlarm(charTime)){
      speaker.buzzerOn(5,1);
      led.buzzerOn(1,1);
    }


    MoveCursorToLine(1);






  }

 	if( tnow >= nextSecond ){
    nextSecond = tnow + 1;
    MoveCursorToLine(1);
    ClearLineAtCursor();

    Serial.print(ctime (&tnow));
  }

}

