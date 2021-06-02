#ifndef ALERTS_H
#define ALERTS_H
#define ARDUINOJSON_ENABLE_COMMENTS 1

#include "ArduinoJson.h"
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
        String getToneName();
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
        Schedule(String name){this->alertCount = 0; this->id = 0; this->name = name;}
        Schedule(JsonObject const jsonAlert);
        int getId(){return id;};
        int GetAlertTotal(){return vectorAlerts.size();};
        void setId(int newID){id = newID;};
        bool addAlert(Alert);
        bool addAlert(const String* time, int duration, AlertTone tone);
        bool removeAlert(int index);
        bool removeAlert(const char* alertName);
        int modifyAlert(int index, int duration, AlertTone tone);
        const String* GetAlertTime(int index);
        AlertTone getAlertTone(int index);
        int getAlertDuration(int index); 
        String* getName();
        int validAlert(int index);
        void debugPrintTimes();
        void toJSON(DynamicJsonDocument *buffer);
        int operator== (const char* RHS){ return strcmp(name.c_str(),RHS)==0;}
        ~Schedule();
        int hasAlertAtTime(const char* currentTime){
            return std::find(vectorAlerts.begin(),vectorAlerts.end(),currentTime)==vectorAlerts.end();
        };
        Alert*  getAlertAtTime(const char* currentTime){
            return &(*std::find(vectorAlerts.begin(),vectorAlerts.end(),currentTime));
        };
        std::vector<Alert>::iterator operator[](const int rhs){return vectorAlerts.begin()+ rhs;}
    private:
        int alertCount;
        int id;
        String name;
        // a vector of all the alerts
        std::vector<Alert> vectorAlerts;
        
};


class mySchedules{
  public:
  std::vector<Schedule> schedules;
  std::vector<Schedule>::iterator operator[](const char* schdName){
      return getSchedule(schdName);
    //   std::vector<Schedule>::iterator returnValue= schedules.begin();
    //   size_t index = 0;
    //   while(index < schedules.size()){
    //       if (strcmp(schedules[index].getName()->c_str(),test)==0){
    //           return schedules.begin()+index;
    //           }
    //           index++;
    //   };c schdNameschdName  
    //     return (std::vector<Schedule>::iterator)NULL;

      };
  std::vector<Schedule>::iterator getSchedule(const char* schdName){
    return std::find<std::vector<Schedule>::iterator>(schedules.begin(),schedules.end(),schdName);

  }
  std::vector<Schedule>::iterator operator[](const int test){return schedules.begin()+test;};
  bool addSchedule(Schedule data){
      if (HasName(data.getName()->c_str())) return false;
      if (data.getId() == 0) data.setId(schedules.size()+1);
      schedules.push_back(data);
      return true;};
  void Print(){for( size_t i = 0; i < schedules.size();i++ ){schedules[i].debugPrintTimes();}}
  void toJson(DynamicJsonDocument *buffer);
  void toJson(String *scheduleName, DynamicJsonDocument *buffer);
  bool HasName(const char * searchname){
      return std::find<std::vector<Schedule>::iterator>(schedules.begin(),schedules.end(),searchname) != schedules.end();
  }

  bool DeleteSchedule(const char* schedName){
    std::vector<Schedule>::iterator del = getSchedule(schedName);
    if(del != schedules.end()){
    for (int i = del->GetAlertTotal()-1; i >= 0; i--){
        del->removeAlert(i);
    }
    del->debugPrintTimes();
    schedules.erase(del);
    return true;
    }
    return false;
  }

};
extern mySchedules Schedules;

class ScheduleItems{
    private:
    String name;
    bool isrepeating;

    public:
    String* getName() {return &name;};
    bool getRepeating() {return isrepeating;}; 
    void setName(const String newname){this->name = newname; };
    void setRepeating(const bool repeat);

    ScheduleItems(){name= ""; isrepeating = 0;};
    ScheduleItems(JsonObject const jsonAlert){
        setName(jsonAlert[F("Name")]); 
        isrepeating = jsonAlert[F("Repeat")];
    };
    void print(){Serial.printf("name: %s, repeating %d\n", name.c_str(), isrepeating);}

};

struct weeklyDailyList{

    std::vector<ScheduleItems> list;
    size_t listsize=0;
    String dayName;
    
    void AddSchedule(ScheduleItems item){list.push_back(item);listsize++;};
    void AddSchedule(JsonObject const jsonAlert){list.push_back(ScheduleItems(jsonAlert));listsize++;};
    void print(){Serial.print(F("Next List -> "));Serial.println(dayName); for (size_t i = 0; i < listsize; i++){Serial.print("\t");list[i].print();}}
    int hasAlarm(const char* minutes){
        for(size_t listnumber = 0; listnumber < list.size(); listnumber++){
            const char* scheduleName = list[listnumber].getName()->c_str();
            if(Schedules.HasName(scheduleName)){
                if (!Schedules[scheduleName]->hasAlertAtTime(minutes)){
                    return true;
                }
            };
        }
        return 0;
    };
    Alert* getAlarm(const char* minutes){
        for(size_t listnumber = 0; listnumber < list.size(); listnumber++){
            const char* scheduleName = list[listnumber].getName()->c_str();
            if(Schedules.HasName(scheduleName)){
                if (!Schedules[scheduleName]->hasAlertAtTime(minutes)){
                    return Schedules[scheduleName]->getAlertAtTime(minutes);
                }
            };
        }
        return (Alert*)0;
    };


};

extern  weeklyDailyList dailyList[7];

#endif