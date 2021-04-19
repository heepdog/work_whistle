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
////////////////////////////////////////////////////////

// #define SSID            F("heatmor-guest")
// #define SSIDPWD         F("")
#define SSID            F("heppners-2")
#define SSIDPWD         F("Cannotcrackit")



void setup() {
  Serial.begin(115200);
  Serial.println("\nstart program");
  Serial.println();

  LittleFSConfig cfg;
  cfg.setAutoFormat(false);
  LittleFS.setConfig(cfg);

  LittleFS.begin();

  StaticJsonDocument<2500> doc;
  File file = LittleFS.open(F("/config.json"),"r");
  deserializeJson(doc, file);
  file.close();
 
  int numberSchedules = doc[F("Schedules")].size();
  
  for( int i = 0; i < numberSchedules; i++){
    Schedules.addSchedule(Schedule(doc[F("Schedules")][i].as<JsonObject>()));
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, SSIDPWD);

  setup_timezones();
  ClearScreen();

}


timeval tv;
time_t tnow;
void testTime();

void loop() {

  static time_t nextMinute = 0;
  static time_t nextSecond = 0;
  tnow = time(nullptr);

  if((localtime(&tnow)->tm_sec == 0) && (tnow >= nextMinute)){
    nextMinute = tnow + 60;
  
    MoveCursorToLine(2);
    CSFromCursorDown();
    Schedules["Morning overtime"]->debugPrintTimes();
    // Schedules[1]->debugPrintTimes();
    // Schedules.Print();

		Serial.println ();
    
    uint32_t free;
    uint16_t max;
    uint8_t frag;
    ESP.getHeapStats(&free, &max, &frag);

    Serial.printf("free: %5d - max: %5d - frag: %3d%% <- \n\r", free, max, frag);
    // gettimeofday(&tv, nullptr);
    Serial.print(asctime (localtime (&tnow)));

    MoveCursorToLine(1);

  }

 	if( tnow >= nextSecond ){
     nextSecond = tnow + 1;
    //  delay (1000);
    // String printTime = asctime (localtime (&tnow));
    // printTime.trim();
    MoveCursorToLine(1);
    ClearLineAtCursor();

    Serial.print(asctime (localtime (&tnow)));
    Serial.print(Schedules.HasName("Main"));
    Serial.print(Schedules.HasName("toast"));
   }
}

// void testTime(){
//   gettimeofday(&tv, nullptr);
//   tnow = time(nullptr);

//   // localtime / gmtime every second change
//   static time_t nextv = 0;
//   if (nextv < tv.tv_sec) {
//     nextv = tv.tv_sec+60;
// #if 0
// 		printf ("gettimeofday() tv.tv_sec : %ld\n", tv.tv_sec);
// 		printf ("time()            time_t : %ld\n", tnow);
// 		Serial.println ();
// #endif

// 		printf ((const char*)F("         ctime: %s"), ctime (&tnow));	// print formated local time
// 		printf ((const char*)F(" local asctime: %s"), asctime (localtime (&tnow)));	// print formated local time
// 		printf ((const char*)F("gmtime asctime: %s"), asctime (gmtime (&tnow)));	// print formated gm time

// 		// print gmtime and localtime tm members
// 		printTm ((const char*)F("      gmtime"), gmtime (&tnow));
// 		Serial.println ();
// 		printTm ((const char*)F("   localtime"), localtime (&tnow));
// 		Serial.println ();
// 		// printTm ("   clock", clock(&tnow));
//     Serial.println(ESP.getFreeHeap());
//     uint32_t free;
//     uint16_t max;
//     uint8_t frag;
//     ESP.getHeapStats(&free, &max, &frag);

//     Serial.printf("free: %5d - max: %5d - frag: %3d%% <- ", free, max, frag);
//     Serial.println();    //tnow = time(nullptr);
    // printTm ((const char*)F("   localtime"), localtime (&tnow));
    // Serial.println();
    // printf ((const char*)F(" local asctime: %s"), asctime (localtime (&tnow)));	// print formated local time
    // Serial.println();


//   }
//   }
// }