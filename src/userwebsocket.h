#ifndef USERWEBSOCKET_H
#define USERwEBsOCKET_H
#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "utils.h"
#include "alerts.h"

// Web server running on port 80
AsyncWebServer server(80);
// Web socket
AsyncWebSocket ws("/ws");

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
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

    const char * const JsonDataKey("data");
    const char * const JsonCommandKey("command");
    const char * const JsonPinKey("pin");
    const char * const DigitalCommand("DigitalWrite");
    const char * const getDailySchedules("getDailySchedules");
    const char * const getSchedules("getSchedules");
    

    DynamicJsonDocument response(1600);
    char buffer[1600];

    Serial.printf("data: %.*s\n",len, data);
    // DynamicJsonDocument command(len);
    StaticJsonDocument<500> command;
    deserializeJson(command,data);
    if(command[JsonCommandKey].as<String>() == DigitalCommand ){
      String pinNumber = command[JsonPinKey].as<String>();
      int value = command[JsonDataKey].as<int>();

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
    else if(command[JsonCommandKey].as<String>() == getDailySchedules ){
      response["command"] = "DailySchedules";

      for (int i = 0 ; i < 7; i++){
          for( size_t j = 0 ; j < dailyList[i].listsize; j++){
            response[dailyList[i].dayName.c_str ()][j] = dailyList[i].list[j].getName()->c_str();
          }
      }

      response.shrinkToFit();
      
      serializeJson(response,buffer);
      client->printf(buffer);
    }
    
    else if(command[JsonCommandKey].as<String>() == getSchedules ){
      response["command"] = "GetSchedules";
      Schedules.toJson(&response);
      response.shrinkToFit();
      Serial.printf("size of json doc: %d",response.size());

      serializeJson(response,Serial);
      serializeJson(response,buffer);
      client->printf(buffer);


    }

  }

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
void setupServer(){

  ws.onEvent(onEvent);
  server.addHandler(&ws);

  server.serveStatic("/",LittleFS,"/www/");

  server.begin();
}


#endif