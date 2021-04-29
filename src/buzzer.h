#include "Arduino.h"

class buzzer{
    const int pinNumber;

    public:
        buzzer(int pin):pinNumber(pin){

            invertOut = false; setup();
            };
        buzzer(int pin, bool invert):pinNumber(pin){
            invertOut = invert; 
            setup();
            };
        void buzzerOn(int secondsOn, int mode){
            Serial.printf("\nturning on buzzer for %d Pin: %d \n", + secondsOn, pinNumber);
            timeON = millis()+(secondsOn*1000);
            digitalWrite(pinNumber,!invertOut);
            on=true;
            };
        void update(){
            if( on && (timeON < millis()) ){
                Serial.printf("\n\nturning off buzzer on pin: %d \n", pinNumber);
                digitalWrite(pinNumber,invertOut);
                timeON = 0;
                on = false;
            };
        };
        
        

    private:
        void setup(){
            pinMode(pinNumber,OUTPUT);
            digitalWrite(pinNumber,invertOut);
            timeON = 0;
            on = false;
        };
        bool on;
        unsigned long timeON;
        bool invertOut;



};