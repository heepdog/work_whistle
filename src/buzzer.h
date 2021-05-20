#include "Arduino.h"

class buzzer{

    public:
        buzzer(int pin):pinNumber(pin){

            invertOut = false; setup();
            };
        buzzer(int pin, bool invert):pinNumber(pin){
            invertOut = invert; 
            setup();
            };
        void buzzerOn(int secondsOn, int mode){
            // Serial.printf("\nturning on buzzer for %d Pin: %d \n", + secondsOn, pinNumber);
            count = 0;
            delay = (secondsOn*100);
            if (mode == AlertTone::SINGLE){
                timeON = millis()+delay;
            }
            else{
                timeON = millis()+ delay/pulses+1; 
            }
                digitalWrite(pinNumber,!invertOut);
                on=true;
            
            };
        void update(){
            if( (count < pulses)  && (timeON < millis())) {
                if(on){
                    // Serial.printf("\n\nturning off buzzer on pin: %d \n", pinNumber);
                    digitalWrite(pinNumber,invertOut);
                    timeON = millis()+delay;
                    on = false;
                    count++;
                }
                else{
                    digitalWrite(pinNumber,!invertOut);
                    timeON = millis()+delay;
                    on = true;

                }
            };

        };
        
        

    private:
        void setup(){
            pinMode(pinNumber,OUTPUT);
            digitalWrite(pinNumber,invertOut);
            timeON = 0;
            on = false;
            pulses = 3;
        };
        bool on;
        unsigned long timeON;
        bool invertOut;
        //count the number of pulses we have sounded
        int count;
        //time on is divided by pulses + 1
        int pulses;
        int delay;

        const int pinNumber;




};