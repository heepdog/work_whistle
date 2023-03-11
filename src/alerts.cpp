#include "alerts.h"
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
    setId(jsonAlert["id"]);
    const String timeString = jsonAlert["time"];
    setTime(&timeString);
    // Serial.println(jsalert["time"].as<char*>());
    setDuration(jsonAlert["duration"]);

    setTone(strcmp(jsonAlert["tone"],"pulse")==0?PULSE:SINGLE);
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

    id = jsonalerts["id"].as<int>();
    if (id == 0) {id = vectorAlerts.size();}
    name = jsonalerts["name"].as<String>();
    int number_of_alerts = jsonalerts["alerts"].size();
    for(int i= 0; i < number_of_alerts; i++){
        Alert tmp = Alert(jsonalerts["alerts"][i].as<JsonObject>());
        if(tmp.getId() == 0 ) tmp.setId(5);
        addAlert(tmp);
    }
    // std::sort(vectorAlerts.begin(),vectorAlerts.end(), AlertSort);

}
int Schedule::addAlert(const String* time, int durration, AlertTone tone){
    Alert tmp;
    tmp.setTime(time);
    tmp.setDuration(durration);
    tmp.setTone(tone);
    tmp.setId(vectorAlerts.size() + 1);
    // vectorAlerts.push_back(Alert(vectorAlerts.size()+1 , time, durration, tone));
    Serial.println(*time);
    addAlert(tmp);
    // vectorAlerts.push_back(tmp);
    // std::sort(vectorAlerts.begin(),vectorAlerts.end(), AlertSort);
    // alertCount++;

    return 1;

}
int Schedule::addAlert(Alert nextAlert){

    vectorAlerts.push_back(nextAlert);
    std::sort(vectorAlerts.begin(),vectorAlerts.end(), AlertSort);
    alertCount++;

    return 1;

}
int Schedule::removeAlert(int index){
    // alerts[index] = Null ;
    vectorAlerts.erase(vectorAlerts.begin() + index);

    return 0;


}
int Schedule::modifyAlert(int index, int durration, AlertTone tone){

    return 0;
   
}
const String* Schedule::GetAlertTime(int index){
    // return alerts[index].getTime();
    return vectorAlerts[index].getTime();
    }
AlertTone Schedule::getAlertTone(int index){ 
    // return alerts[index].getTone();
    return vectorAlerts[index].getTone();
    }
int Schedule::getAlertDuration(int index){
    // return alerts[index].getDuration();
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