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
        int getId();
        int setId(int id); 
        int setTime(const String* time);
        int setDuration(int duration);
        int setTone(const AlertTone tone);
        int operator<(const Alert testAlert) ;
        int operator>(const Alert testAlert) ;
        int operator==(const Alert testAlert) ;
        int operator==(const char* searchTime) ;
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
} AlertSort;


class Schedule{
    public:
        Schedule();
        Schedule(JsonObject const jsonAlert);
        int getId(){return id;};
        void setId(int newID){id = newID;};
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
        int operator== (const char* RHS){ return strcmp(name.c_str(),RHS)==0;}
        ~Schedule();
        int alertAtTime(const char* currentTime){
            return std::find(vectorAlerts.begin(),vectorAlerts.end(),currentTime)==vectorAlerts.end();
            };
        
    private:
        int alertCount;
        int id;
        String name;
        // Alert alerts[ 15*sizeof( Alert )];
        std::vector<Alert> vectorAlerts;
        // std::array<Alert,1<std::vector<Schedule>,
};


struct {
  std::vector<Schedule> schedules;
  std::vector<Schedule>::iterator operator[](const char* schdName){
      return std::find<std::vector<Schedule>::iterator>(schedules.begin(),schedules.end(),schdName);
    //   std::vector<Schedule>::iterator returnValue= schedules.begin();
    //   size_t index = 0;
    //   while(index < schedules.size()){
    //       if (strcmp(schedules[index].getName()->c_str(),test)==0){
    //           return schedules.begin()+index;
    //           }
    //           index++;
    //   };
    //     return (std::vector<Schedule>::iterator)NULL;

      };
  std::vector<Schedule>::iterator operator[](const int test){return schedules.begin()+test;};
  bool addSchedule(Schedule data){
      if (HasName(data.getName()->c_str())) return false;
      if (data.getId() == 0) data.setId(schedules.size()+1);
      schedules.push_back(data);
      return true;};
  void Print(){for( size_t i = 0; i < schedules.size();i++ ){schedules[i].debugPrintTimes();}}
  bool HasName(const char * searchname){
      return std::find<std::vector<Schedule>::iterator>(schedules.begin(),schedules.end(),searchname) != schedules.end();

  }

}Schedules;

class ScheduleItems{
    private:
    String name;
    bool isrepeating;

    public:
    String* getName() {return &name;};
    bool getRepeating() {return isrepeating;}; 
    void setName(const String* newname){this->name = *newname; };
    void setRepeating(const bool repeat);

    ScheduleItems(){name= ""; isrepeating = 0;};
    ScheduleItems(JsonObject const jsonAlert){
        name = jsonAlert["Name"].as<String>(); 
        isrepeating = jsonAlert["repeat"];
    };
    void print(){Serial.printf("name: %s, repeating %d\n", name.c_str(), isrepeating);}

};

struct{

    // std::vector<ScheduleItems> list;
    ScheduleItems list[10];
    size_t listsize=0;

    // void AddSchedule(ScheduleItems item){list.push_back(item);};
    // void AddSchedule(JsonObject const jsonAlert){list.push_back(ScheduleItems(jsonAlert));};
    // void print(){for (size_t i = 0; i < list.size(); i++){list[i].print();}}
    
    void AddSchedule(ScheduleItems item){list[listsize] = item;listsize++;};
    void AddSchedule(JsonObject const jsonAlert){list[listsize] = (ScheduleItems(jsonAlert));listsize++;};
    void print(){Serial.println("Next List -> "); for (size_t i = 0; i < listsize; i++){list[i].print();}}


}dailyList[7];

