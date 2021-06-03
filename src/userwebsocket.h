#ifndef USERWEBSOCKET_H
#define USERWEBSOCKET_H
#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "utils.h"
#include "alerts.h"

#include <LittleFS.h>

// Web server running on port 80
AsyncWebServer server(80);
// Web socket
AsyncWebSocket ws("/ws");

int saveSchedules(DynamicJsonDocument *, const char *);


void sendTime(){
  ws.pingAll();
  // ws.ping(1);
}


void notifyClient(int status) {
  // char buffer[10];
  // PIN_TO_JSON_STATUS(D0,buffer);
    // ws.textAll(buffer);
    ws.printfAll( PIN_TO_JSON_STATUS(D4));
    ws.printfAll( PIN_TO_JSON_STATUS(D3));
    ws.printfAll( PIN_TO_JSON_STATUS(D2));
    ws.printfAll( PIN_TO_JSON_STATUS(D1));
    ws.printfAll( PIN_TO_JSON_STATUS(D5));


}

void handlingIncomingData(void *arg, uint8_t *data, size_t len, AsyncWebSocketClient *client) {

  #define debugprintcommand(x)  CSFromCursorDown(); Serial.print("got \"");Serial.print(FPSTR(x));Serial.println("\"");


  AwsFrameInfo *info = (AwsFrameInfo*)arg;

  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

    static const char JSONDATAKEY[] PROGMEM = "Data";
    static const char JSONCOMMANDKEY[] PROGMEM  = "Command";
    static const char JSONPINKEY[] PROGMEM = "Pin";
    static const char GETDAILYSCHEDULES[] PROGMEM = "GetDailySchedules";
    static const char DAILYSCHEDULES[] PROGMEM = "DailySchedules";
    static const char GETSCHEDULES[] PROGMEM = "GetSchedules";
    static const char RESULT[] PROGMEM = "Result";
    static const char ALERTTIME[] PROGMEM = "AlertTime";
    static const char DELETEALERT[] PROGMEM = "DeleteAlert";
    // const char * const EditAlert("EditAlert");
    // const char * const EditSchedule("EditSchedule");
    // const char * const RemoveSchedule("RemoveSchedule");
    static const char NEWSCHEDULE[] PROGMEM = "NewSchedule";
    static const char DELETESCHEDULE[] = "DeleteSchedule";
    static const char ADDALERT[] PROGMEM = "AddAlert";
    // const char * const AddSchedule("AddSchedule");
    // const char * const SchedulesFile("schedules.json");
    const char * const DigitalCommand("DigitalWrite");

    DynamicJsonDocument response(2000);
    // JsonObject &response = jsonBuffer.createOLbject();
    // char buffer[1000];

    Serial.printf("data: %.*s\n",len, data);

    // Json object for the request from the connected client
    DynamicJsonDocument command(500);
    deserializeJson(command,data);
    if(command[FPSTR(JSONCOMMANDKEY)].as<String>() == DigitalCommand ){
      String pinNumber = command[FPSTR(JSONPINKEY)].as<String>();
      int value = command[FPSTR(JSONDATAKEY)].as<int>();

      if (CMP_DEFN(D4, pinNumber))
        digitalWrite(D4, value);
      else if (CMP_DEFN(D3, pinNumber))
        digitalWrite(D3, value);
      else if (CMP_DEFN(D2, pinNumber))
        digitalWrite(D2, value);
      else if (CMP_DEFN(D1, pinNumber))
        digitalWrite(D1, value);
      else if (CMP_DEFN(LED_BUILTIN, pinNumber))
        digitalWrite(LED_BUILTIN, value);
    }
    
    else if(command[FPSTR(JSONCOMMANDKEY)].as<String>() == FPSTR(GETDAILYSCHEDULES) ){
      debugprintcommand(GETDAILYSCHEDULES);
      response[FPSTR(JSONCOMMANDKEY)] = FPSTR(DAILYSCHEDULES);

      for (int i = 0 ; i < 7; i++){
          for( size_t j = 0 ; j < dailyList[i].listsize; j++){
            response[dailyList[i].dayName.c_str ()][j] = dailyList[i].list[j].getName()->c_str();
          }
      }

          
      // serializeJson(response,buffer);
      // client->printf(buffer);
    }
    
    else if(command[FPSTR(JSONCOMMANDKEY)].as<String>() == FPSTR(GETSCHEDULES) ){
      debugprintcommand(GETSCHEDULES);
      Schedules.toJson(&response);
      response[FPSTR(JSONCOMMANDKEY)] = FPSTR(GETSCHEDULES);
    }

    else if(command[FPSTR(JSONCOMMANDKEY)].as<String>() == FPSTR(DELETEALERT)){
      debugprintcommand(DELETEALERT);
      String alertTime = command[FPSTR(ALERT)];
      String scheduleName = command[FPSTR(SCHEDULE)];
      response[FPSTR(RESULT)]= Schedules[scheduleName.c_str()]->removeAlert(alertTime.c_str());
      if (response[FPSTR(RESULT)] == 1){
        saveSchedules(&response, SchedulesFile);
        response[FPSTR(JSONCOMMANDKEY)] = F("DeleteAlertSuccess");
      } else{
          response[FPSTR(JSONCOMMANDKEY)] = F("DeleteAlertError");
          response[FPSTR(ALERTTIME)] = alertTime;
          response[FPSTR(SCHEDULE)] = scheduleName;
      }
    }
    
    // Add Alert To Schedule
    else if(command[FPSTR(JSONCOMMANDKEY)].as<String>() == FPSTR(ADDALERT) ){
      debugprintcommand(ADDALERT);

      String scheduleName= command[FPSTR(SCHEDULE)];
      String AlertTime = command[FPSTR(ALERTTIME)];
      response[FPSTR(RESULT)] = Schedules[scheduleName.c_str()]->addAlert(&AlertTime,15,AlertTone::SINGLE);
      if(response[FPSTR(RESULT)] == 1){
        saveSchedules(&response, SchedulesFile);
        response[FPSTR(JSONCOMMANDKEY)] = FPSTR(GETSCHEDULES);
      } else{
        response[FPSTR(JSONCOMMANDKEY)] = F("AddAlertError");
        response[FPSTR(SCHEDULE)] = scheduleName;
        response[FPSTR(ALERTTIME)] = AlertTime;
      }
      

      // Schedules.toJson(&scheduleName,&response);
    }

    else if(command[FPSTR(JSONCOMMANDKEY)].as<String>() == FPSTR(NEWSCHEDULE) ){
     debugprintcommand(NEWSCHEDULE);

      String scheduleName = command[FPSTR(NAME)].as<String>();
      response[FPSTR(RESULT)] = Schedules.addSchedule( Schedule( scheduleName ) );
      if(response[FPSTR(RESULT)] == 1){
        saveSchedules(&response, SchedulesFile);
        response[FPSTR(JSONCOMMANDKEY)] = FPSTR(GETSCHEDULES);

      } else{
        response[FPSTR(JSONCOMMANDKEY)] = F("NewScheduleError");
        response[FPSTR(SCHEDULE)] = scheduleName;
      }

    }

    else if (command[FPSTR(JSONCOMMANDKEY)].as<String>() == FPSTR(DELETESCHEDULE)){
      debugprintcommand(DELETESCHEDULE);
      String scheduleName = command[FPSTR(NAME)].as<String>();
      response[FPSTR(RESULT)] =  Schedules.DeleteSchedule(scheduleName.c_str());
      if(response[FPSTR(RESULT)] == 1){
        saveSchedules(&response, SchedulesFile);
        response[FPSTR(JSONCOMMANDKEY)] = FPSTR(GETSCHEDULES);
      } else{
        response[FPSTR(JSONCOMMANDKEY)] = F("DeleteScheduleError");
        response[FPSTR(SCHEDULE)] = scheduleName;
      }

    }


    {// Create buffer for serialized json, and send to Websocket client 
      response.shrinkToFit();
      size_t bufferLength = measureJson(response)+1;
      // Serial.println("Creating buffer");
      //creates buffer for sending datat through the websocket
      void *buffer = malloc(bufferLength);
      if (buffer){
        // Serial.println("Serealizing Json");
        serializeJson(response,buffer, bufferLength);
        if(client){
          // Serial.println("Sending Json");
          client->text((char *)buffer);
          // Serial.println("done sending json");
        } else{
          ws.textAll((char *)buffer);
        }
      }
      free(buffer);
      response.clear();
      response.shrinkToFit();
    }
  }
}

// Takes Pointer to a json document and saves it to filename
int saveSchedules(DynamicJsonDocument *json, const char* filename){
  Schedules.toJson(json);
  File schedulejson = LittleFS.open(filename,"w");
  serializeJsonPretty(*json,schedulejson);
  schedulejson.close();
  return 1;
}

// Callback for incoming event
void onEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, 
             void * arg, uint8_t *data, size_t len){
   switch(type) {
      case WS_EVT_CONNECT:
        Serial.printf("Client connected: \n\tClient id:%u\n\tClient IP:%s\n", 
             client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
         Serial.printf("Client disconnected:\n\tClient id:%u\n", client->id());
         break;
      case WS_EVT_DATA:
         handlingIncomingData(arg, data, len, client);
         break;
      case WS_EVT_PONG:
          Serial.printf("Pong:\n\tClient id:%u\n", client->id());
          break;
      case WS_EVT_ERROR:
          Serial.printf("Error:\n\tClient id:%u\n", client->id());
          break;     
   }
  
}

// Sets the asyncweb server to handle and serve files
// This is wehere the static and event handlers are 
// connected to the  webserver
void setupServer(){

  ws.onEvent(onEvent);
  server.addHandler(&ws);

  server.serveStatic("/",LittleFS,"/www/");
  server.serveStatic("debug/",LittleFS,"/");

  server.begin();
}


#endif