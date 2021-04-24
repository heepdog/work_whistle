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
////////////////////////////////////////////////////////
#define DEBUG
#ifdef DEBUG
#define debug(x) Serial.println(x);
#else
#define debug(x)
#endif

// #define SSID            F("heatmor-guest")
// #define SSIDPWD         F("")
#define SSID            F("heppners-2")
#define SSIDPWD         F("Cannotcrackit")

// StaticJsonDocument<2500> doc;
// DynamicJsonDocument doc(1024);

buzzer speaker = buzzer(2,true);


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
  // filea.close();
  // StaticJsonDocument<2500> doc;
  DynamicJsonDocument doc(filesize*2);

  // File file = LittleFS.open(F("/config.json"),"r");
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
  // Schedules[newName]->addAlert(&time1,3,AlertTone::SINGLE);

  
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, SSIDPWD);

  setup_timezones();
  ClearScreen();

}


timeval tv;
tm* tm_tm_now;
time_t tnow;
void testTime();
int GetNextAlarm(int, const char*);
bool addedTime = false;

void loop() {

  speaker.update();

  static time_t nextMinute = 0;
  static time_t nextSecond = 0;
  tnow = time(nullptr);
  tm_tm_now = localtime(&tnow);
  int currentDay = tm_tm_now->tm_wday;
  int currentMinute = tm_tm_now->tm_min + tm_tm_now->tm_hour*60;
  char charTime[6];
  sprintf(charTime,"%2d:%02d",tm_tm_now->tm_hour, tm_tm_now->tm_min);

  //test add alert 2 mins after starting
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
    // Schedules["Morning overtime"]->debugPrintTimes();
    // Schedules[1]->debugPrintTimes();
    // Schedules.Print();

		Serial.printf("Current Day: %d\n",currentDay);
		Serial.printf("Current Minute: %d\n",currentMinute);
    Serial.printf("Char Time: %s\n",charTime);
    
    uint32_t free;
    uint16_t max;
    uint8_t frag;
    ESP.getHeapStats(&free, &max, &frag);

    Serial.printf("free: %5d - max: %5d - frag: %3d%% <- \n\r", free, max, frag);
    // gettimeofday(&tv, nullptr);

    // for( int i = 0 ; i < 7; i++){
    //   dailyList[i].print();
    // }
    dailyList[currentDay].print();

    Serial.print(asctime (localtime (&tnow)));
    if(GetNextAlarm(currentDay, charTime)){
      speaker.buzzerOn(5,1);
    }


    MoveCursorToLine(1);

  }

 	if( tnow >= nextSecond ){
     nextSecond = tnow + 1;
    //  delay (1000);
    // String printTime = asctime (localtime (&tnow));
    // printTime.trim();
    MoveCursorToLine(1);
    ClearLineAtCursor();

    // Serial.print(asctime (localtime (&tnow)));
    Serial.print(ctime (&tnow));
    // Serial.print(Schedules.HasName("Main"));
    // Serial.print(Schedules.HasName("toast"));
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
//     Serial.println(ESP.getFreeHeap());void
//     uint32_t free;
//     uint16_t max;
//     uint8_t frag;
//     ESP.getHeapStats(&free, &max, &frag);

//     Serial.printf("free: %5d - max: %5d - frag: %3d%% <- ", free, max, frag);
//     Serial.println();    //tnow = time(nullptrvoid);
    // printTm ((const char*)F("   localtime"), localtime (&tnow));
    // Serial.println();
    // printf ((const char*)F(" local asctime: %s"), asctime (localtime (&tnow)));	// print formated local time
    // Serial.println();


//   }
//   }
// }

int GetNextAlarm(int day, const char* minutes){

  // dailyList[day].print();
  for(size_t listnumber = 0; listnumber < dailyList[day].list.size(); listnumber++){
    const char* scheduleName = dailyList[day].list[listnumber].getName()->c_str();
    if(Schedules.HasName(scheduleName)){
      if (!Schedules[scheduleName]->alertAtTime(minutes)){
        return true;
      }

    };
  }

  return 0;

}