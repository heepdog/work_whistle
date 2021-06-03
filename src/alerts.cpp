#include "alerts.h"
#define ARDUINOJSON_ENABLE_COMMENTS 1

#include "ArduinoJson.h"
#include "Arduino.h"


Alert::Alert(){

}

Alert::~Alert(){}

Alert::Alert(const Alert* copy){
    id = copy->id;
    setTime(&(copy->time));
    duration = copy->duration;
    tone = copy->tone;

}
Alert::Alert(int id, const String* time, int duration, const AlertTone tone){
        setId(id);
        const String timeString = (const String)*time;
        setTime(&timeString);
        setDuration(duration);
        setTone(tone);
}

Alert::Alert(const JsonObject jsonAlert){
    setId(jsonAlert[FPSTR(ID)]);
    const String timeString = jsonAlert[FPSTR(TIME)];
    setTime(&timeString);
    // Serial.println(jsalert["time"].as<char*>());
    setDuration(jsonAlert[FPSTR(DURATION)]);

    setTone(strcmp(jsonAlert[FPSTR(TONE)],"Pulse")==0?PULSE:SINGLE);
}

int Alert::setId(int id){
    this->id = id;
    return true;
}

int Alert::getId(){
    return this->id;
}
int Alert::setTime(const String* time){
    this->time =  *time;
    int colon_index = time->indexOf(":");
    this->minutes_in_day = 60 * time->substring(0,colon_index).toInt();
    this->minutes_in_day += time->substring(colon_index+1).toInt();
    return true;
}
int Alert::setDuration(int duration){
    this->duration = duration;
    return true;
}
int Alert::setTone(const AlertTone tone){
    this->tone = tone;
    return true;
}
int Alert::getDuration(){
    return this->duration;
}

int Alert::getMinutes(){
    return minutes_in_day;
}

int Alert::operator<(const Alert RHSAlert)  {

    return (this->minutes_in_day < RHSAlert.minutes_in_day);
}
int Alert::operator>(const Alert RHSAlert)  {
    return (this->minutes_in_day > RHSAlert.minutes_in_day);
}
int Alert::operator==(const Alert RHSAlert)  {
    return (this->minutes_in_day == RHSAlert.minutes_in_day);
}
int Alert::operator==(const char* RHSAlert)  {
    return (strcmp(this->getTime()->c_str(),RHSAlert) == 0);
}

AlertTone Alert::getTone(){
   return this->tone;
}
String Alert::getToneName(){
   return this->tone?"Pulse":"Single";
}
const String* Alert::getTime(){
    return &this->time;
}

int Alert::get_minutes_in_day(){
    return minutes_in_day;
}


Schedule::Schedule(){
        alertCount = 0;
        id = 0;
}

Schedule::Schedule(const JsonObject  jsonalerts){ 

    id = jsonalerts[FPSTR(ID)].as<int>();
    if (id == 0) {id = vectorAlerts.size();}
    name = jsonalerts[FPSTR(NAME)].as<String>();
    int number_of_alerts = jsonalerts[FPSTR(ALERTS)].size();
    for(int i= 0; i < number_of_alerts; i++){
        Alert tmp = Alert(jsonalerts[FPSTR(ALERTS)][i].as<JsonObject>());
        if(tmp.getId() == 0 ) tmp.setId(5);
        addAlert(tmp);
    }
    // std::sort(vectorAlerts.begin(),vectorAlerts.end(), AlertSort);

}
bool Schedule::addAlert(const String* time, int duration, AlertTone tone){
    Alert tmp;
    tmp.setTime(time);
    tmp.setDuration(duration);
    tmp.setTone(tone);
    tmp.setId(vectorAlerts.size() + 1);
    // vectorAlerts.push_back(Alert(vectorAlerts.size()+1 , time, duration, tone));
    Serial.println(*time);
    return addAlert(tmp);
    // vectorAlerts.push_back(tmp);
    // std::sort(vectorAlerts.begin(),vectorAlerts.end(), AlertSort);
    // alertCount++;

    

}
bool Schedule::addAlert(Alert nextAlert){
    if(!hasAlertAtTime( nextAlert.getTime()->c_str())){
        return false;
    }
    vectorAlerts.push_back(nextAlert);
    std::sort(vectorAlerts.begin(),vectorAlerts.end(), AlertSort);
    alertCount++;
    return true;


}
bool Schedule::removeAlert(int index){
    // alerts[index] = Null ;
    if(0 <= index && index < (int) vectorAlerts.size()){
        vectorAlerts.erase(vectorAlerts.begin() + index);
        return true;
    }

    return false;
}

bool Schedule::removeAlert(const char* alertName){

    if (!hasAlertAtTime(alertName)){
        Serial.println(alertName);
        vectorAlerts.erase(std::find(vectorAlerts.begin(),vectorAlerts.end(),alertName)); 
        return true;
    }
    return false;
}
int Schedule::modifyAlert(int index, int duration, AlertTone tone){

    return 0;
   
}
const String* Schedule::GetAlertTime(int index){
     return vectorAlerts[index].getTime();
    }
AlertTone Schedule::getAlertTone(int index){ 
     return vectorAlerts[index].getTone();
    }
int Schedule::getAlertDuration(int index){
    return vectorAlerts[index].getDuration();
    }
String* Schedule::getName(){
    return &name;
}
int Schedule::validAlert(int index){
    return 0;
}
Schedule::~Schedule(){}

void Schedule::debugPrintTimes(){

    Serial.println("name: \"" + name + "\" ;  id = "  + id);
    for(size_t i = 0;i < vectorAlerts.size(); i++){
        Serial.println("TIME: " +*vectorAlerts[i].getTime()+ " " + vectorAlerts[i].get_minutes_in_day());

    }

}

void Schedule::toJSON(DynamicJsonDocument *buffer){
    //DynamicJsonDocument JsonSchedule(200);
    (*buffer)[FPSTR(NAME)] = name;
    for(size_t i = 0;i < vectorAlerts.size(); i++){
        (*buffer)[FPSTR(ALERTS)][i][FPSTR(TIME)]  = *vectorAlerts[i].getTime();
        (*buffer)[FPSTR(ALERTS)][i][FPSTR(DURATION)]  = vectorAlerts[i].getDuration();
        (*buffer)[FPSTR(ALERTS)][i][FPSTR(TONE)]  = vectorAlerts[i].getToneName();
        (*buffer)[FPSTR(ALERTS)][i][FPSTR(ID)]  = vectorAlerts[i].getId();
    }
}

void mySchedules::toJson(DynamicJsonDocument *buffer){
    for(size_t i = 0; i<schedules.size();i++){
        (*buffer)[FPSTR(SCHEDULES)][i][FPSTR(NAME)] = schedules[i].getName()->c_str();

        for(  int j = 0; j <schedules[i].GetAlertTotal(); j++){
            (*buffer)[FPSTR(SCHEDULES)][i][FPSTR(ALERTS)][j][FPSTR(TIME)] = schedules[i][j]->getTime()->c_str();
            (*buffer)[FPSTR(SCHEDULES)][i][FPSTR(ALERTS)][j][FPSTR(DURATION)] = schedules[i][j]->getDuration();
            (*buffer)[FPSTR(SCHEDULES)][i][FPSTR(ALERTS)][j][FPSTR(TONE)] = schedules[i][j]->getToneName();
            (*buffer)[FPSTR(SCHEDULES)][i][FPSTR(ALERTS)][j][FPSTR(ID)] = schedules[i][j]->getId();
        }
    }

}

void mySchedules::toJson(String *scheduleName, DynamicJsonDocument *buffer){
    // for(size_t i = 0; i<schedules.size();i++){
        int i = 0;
        auto thisSchedule = getSchedule(scheduleName->c_str());

        (*buffer)[FPSTR(SCHEDULES)][i][FPSTR(NAME)] = thisSchedule->getName()->c_str();
        thisSchedule->operator[](1);

        for(  int j = 0; j <schedules[i].GetAlertTotal(); j++){
            (*buffer)[FPSTR(SCHEDULES)][i][FPSTR(ALERTS)][j][FPSTR(TIME)] = thisSchedule->operator[](j)->getTime()->c_str();
            (*buffer)[FPSTR(SCHEDULES)][i][FPSTR(ALERTS)][j][FPSTR(DURATION)] = thisSchedule->operator[](j)->getDuration();
            (*buffer)[FPSTR(SCHEDULES)][i][FPSTR(ALERTS)][j][FPSTR(TONE)] = thisSchedule->operator[](j)->getToneName();
            (*buffer)[FPSTR(SCHEDULES)][i][FPSTR(ALERTS)][j][FPSTR(ID)] = thisSchedule->operator[](j)->getId();
        }
    }



weeklyDailyList dailyList[7];
mySchedules Schedules;
