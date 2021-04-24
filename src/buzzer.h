#include "Arduino.h"

class buzzer{
    public:
        buzzer(int pin){ invertOut = false; setup(pin);};
        buzzer(int pin, bool invert){invertOut = invert; setup(pin);}
        void buzzerOn(int secondsOn, int mode){timeON = millis()+(secondsOn*1000);digitalWrite(pinNumber,!invertOut);on=true;};
        void update(){ if( on && (timeON < millis()) ){digitalWrite(pinNumber,invertOut);}};
        
        

    private:
        void setup(int pin){pinNumber = pin; pinMode(pinNumber,OUTPUT);digitalWrite(pinNumber,invertOut);}
        int pinNumber;
        bool on;
        unsigned long timeON;
        bool invertOut;



};