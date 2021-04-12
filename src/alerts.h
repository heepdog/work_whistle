// #include <StandardCplusplus.h>
#include "ArduinoJson.h"
// #include <set>
#include <Arduino.h>

enum AlertTone{ PULSE, SINGLE };

class Alert{
    public:
        Alert();
        Alert(const Alert* copy);
        Alert(int id, const String* time, int duration, const AlertTone tone);
        Alert(JsonObject jsonAlert);
        ~Alert();
        const String* getTime(); // returns the char 24:00 time of the alert 24hr
        int getHours();  // returns the interger of the alert
        int getMinutes(); // returs the minute of the alert
        int getDuration();  // sets how long the alert should last
        AlertTone getTone();  //sets how the alert tone should sound
        int setId(int id); 
        int setTime(const String* time);
        int setDuration(int duration);
        int setTone(const AlertTone tone);
        int operator<(const Alert testAlert) ;
        int operator>(const Alert testAlert) ;
        int operator==(const Alert testAlert) ;
        int get_minutes_in_day();
        


    private:
        int id;
        String time;
        int duration;
        AlertTone tone;
        int minutes_in_day;


    
};

struct {
    bool operator()(Alert a, Alert b) { return a < b; }
} alertbefore;


class Schedule{
    public:
        Schedule();
        Schedule(JsonObject const jsonAlert);

        int addAlert(Alert);
        int addAlert(const String* time, int durration, AlertTone tone);
        int removeAlert(int index);
        int modifyAlert(int index, int durration, AlertTone tone);
        const String* GetAlertTime(int index);
        AlertTone getAlertTone(int index);
        int getAlertDuration(int index);
        String* getName();
        int validAlert(int index);
        void debugPrintTimes();
        ~Schedule();
        
    private:
        int alertCount;
        int id;
        String name;
        // Alert alerts[ 15*sizeof( Alert )];
        std::vector<Alert> vectorAlerts;
        // std::array<Alert,15> arrayAlerts;
        //use std::sort for the array
        

};


struct {
  std::vector<Schedule> schedules;
  std::vector<Schedule>::iterator operator[](const char* test){return schedules.begin();};
  std::vector<Schedule>::iterator operator[](const int test){return schedules.begin()+test;};
  void addSchedule(Schedule data){schedules.push_back(data);};
}Schedules;
