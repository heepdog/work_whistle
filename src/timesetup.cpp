#include "timesetup.h"


const char *TZstr = "CST+6CDT,M3.2.0/2,M11.1.0/2"; 
bool cbtime_set = false;
timeval cbtime;


void time_is_set(void) { 
        gettimeofday(&cbtime, NULL);
        cbtime_set = true;
        Serial.print("\e[20;0H");
        Serial.println("------------------ settimeofday() was called ------------------");
        Serial.print("\e[0:0H");
    }

void setup_timezones(void){

    settimeofday_cb(time_is_set);


    configTime(0, 0, "pool.ntp.org");
    
    setenv ("TZ", TZstr, 1);
    tzset();
}
