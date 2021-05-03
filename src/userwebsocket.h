
#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "utils.h"

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

void handlingIncomingData(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {

    const char * const JsonDataKey("data");
    const char * const JsonCommandKey("command");
    const char * const JsonPinKey("pin");
    const char * const DigitalCommand("DigitalWrite");
    
    Serial.printf("data: %.*s\n",len, data);
    // DynamicJsonDocument command(len);
    StaticJsonDocument<200> command;
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
    
    notifyClient('1');
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
         handlingIncomingData(arg, data, len);
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

  server.serveStatic("/",LittleFS,"/www/").setDefaultFile("index.html");

  server.begin();
}