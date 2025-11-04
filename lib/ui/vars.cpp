#include "eez-flow.h"
#include "vars.h"

using namespace eez;

// Cache untuk schedule time
static char cachedStartTime[6] = "21:00";
static char cachedEndTime[6] = "00:45";
static bool scheduleInitialized = false;

bool switchWiFiAP(){
    return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_IS_WI_FI_AP).getBoolean();
}

bool switchWiFi(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_IS_WIFI).getBoolean();
}

bool saveDate(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_SAVE_TIME).getBoolean();
}

void setTimeHHMM(const char* hhmm){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_HH_MM, hhmm);
}

void setCurrentDate(const char* datetime){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_CURRENT_DATE, datetime);
}

uint16_t *getDateTime(){
    static uint16_t timeBuffer[9];
    auto value = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_TIME_INFO_ARRAY);
    auto array = value.getArray();
    if (!array) {
        for (int i = 0; i < 9; ++i) timeBuffer[i] = 0;
        return timeBuffer;
    }
    uint16_t count = array->arraySize;
    if (count > 9) count = 9;
    for (uint16_t i = 0; i < count; ++i) {
        timeBuffer[i] = array->values[i].getInt32();
    }
    for (uint16_t i = count; i < 9; ++i) {
        timeBuffer[i] = 0;
    }
    return timeBuffer;
}

void setDateTime(uint16_t* dt, size_t len){
    if (dt == nullptr || len < 6) return;
    eez::ArrayOfInteger flowArray(6);
    for (int i = 0; i < 6; ++i) {
        flowArray.at(i, dt[i]);
    }
    flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_TIME_INFO_ARRAY, flowArray.value);
}

void chargeState(bool isCharge){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_IS_CHARGE, isCharge);
}

int32_t getBatteryPersentage(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_BT).getInt32();
}

void setBT(int32_t persent){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_BT, persent);
}

void setStep(int32_t step){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_FOOTSTEP, step);
}
int isStep(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_ISSTEP).getInt();
}

bool getSaveAlarm(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_SAVE_ALARM).getBoolean();
}

void setSaveAlarm(bool data){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_SAVE_ALARM, data);
}

bool gethidenAlaram(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_HIDDERALARM).getBoolean();
}


const char *getStartTime(){
  const char *time = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_START_TIME).getString();

  
  // Validasi format HH:MM
  if (time && strlen(time) == 5 && time[2] == ':') {
    // Update cache
    strncpy(cachedStartTime, time, 5);
    cachedStartTime[5] = '\0';
    return time;
  }
  
  return cachedStartTime;
}

const char *getEndTime(){
  const char *time = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_END_TIME).getString();

  // Validasi format HH:MM
  if (time && strlen(time) == 5 && time[2] == ':') {
    // Update cache
    strncpy(cachedEndTime, time, 5);
    cachedEndTime[5] = '\0';
    return time;
  }
  
  return cachedEndTime;
}

void timeAlarm(int32_t *data, size_t len ){
  if (data == nullptr || len < 2) return;
  eez::ArrayOfInteger flowArray(2);
  for (int i = 0; i < 2; ++i) {
      flowArray.at(i, data[i]);
  }
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_H, flowArray.value);
}

void setScheduleTime(const char *startTime, const char *endTime) {
  
  if (startTime && strlen(startTime) == 5 && startTime[2] == ':') {
    // Update cache
    strncpy(cachedStartTime, startTime, 5);
    cachedStartTime[5] = '\0';
    
    flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_START_TIME, startTime);
    
    // Verifikasi apakah benar-benar tersimpan
    const char *verify = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_START_TIME).getString();
  } else {
    strncpy(cachedStartTime, "21:00", 5);
    cachedStartTime[5] = '\0';
    flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_START_TIME, "21:00");
  }
  
  if (endTime && strlen(endTime) == 5 && endTime[2] == ':') {
    // Update cache
    strncpy(cachedEndTime, endTime, 5);
    cachedEndTime[5] = '\0';
    
    flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_END_TIME, endTime);

    
    // Verifikasi apakah benar-benar tersimpan
    const char *verify = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_END_TIME).getString();

  } else {
    strncpy(cachedEndTime, "00:45", 5);
    cachedEndTime[5] = '\0';
    flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_END_TIME, "00:45");
  }
  
  scheduleInitialized = true;
}

void fileAudio(const char **data, size_t len){
  if(data == nullptr) return;
  eez::ArrayOfString flowArray(len);
  for (size_t i = 0; i < len; i++)
  {
    flowArray.at(i, data[i]);
  }
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_FILE_AUDIO, flowArray.value);
}

bool playAlarm(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_PLAY_ALARM).getBoolean();
}

void setPlayAlarm(bool data){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_PLAY_ALARM, data);
}

bool playAudio(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_PLAY_AUDIO).getBoolean();
}

const char *getFileAudioSelected(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_AUDIO_SELESTED).getString();
}

bool getSetAlarm(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_SET_ALARAM).getBoolean();
}

const char *getNama(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_NAMA).getString();
}

int32_t getUsia(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_USIA).getInt32();
}

int32_t getBB(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_BB).getInt32();
}

bool getCalculate(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_IS_CALCULATE).getBoolean();
}

void setCalculate(bool data){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_IS_CALCULATE, data);
}

void sethasilHitung(const char* hasil){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_HASIL_HITUNG, hasil);
}


