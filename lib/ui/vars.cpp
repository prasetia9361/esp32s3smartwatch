#include "eez-flow.h"
#include "vars.h"
#include "esp_log.h"

#if LVGL_VERSION_MAJOR >= 9
#include "lvgl/lvgl.h"
#else
#include "lvgl.h"
#endif

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
    if (dt == nullptr || len < 6) {
        ESP_LOGE("VARS", "Invalid setDateTime parameters");
        return;
    }
    
    // Log memory before allocation
    #if LVGL_VERSION_MAJOR >= 9
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    ESP_LOGI("VARS", "LVGL Memory before setDateTime - Free: %d, Used: %d", 
             mon.free_size, mon.total_size - mon.free_size);
    #endif
    
    eez::ArrayOfInteger flowArray(6);
    for (int i = 0; i < 6; ++i) {
        flowArray.at(i, dt[i]);
    }
    flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_TIME_INFO_ARRAY, flowArray.value);
    
    // Log memory after allocation
    #if LVGL_VERSION_MAJOR >= 9
    lv_mem_monitor(&mon);
    ESP_LOGI("VARS", "LVGL Memory after setDateTime - Free: %d, Used: %d", 
             mon.free_size, mon.total_size - mon.free_size);
    #endif
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

bool getHiddenAlarm2(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_HIDDENALARM2).getBoolean();
}

bool getHiddenAlarm3(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_HIDDENALARM3).getBoolean();
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

const char *getStartTime2(){
  const char *time = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_START_TIME2).getString();

  
  // Validasi format HH:MM
  if (time && strlen(time) == 5 && time[2] == ':') {
    // Update cache
    strncpy(cachedStartTime, time, 5);
    cachedStartTime[5] = '\0';
    return time;
  }
  
  return cachedStartTime;
}

const char *getStartTime3(){
  const char *time = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_START_TIME3).getString();

  
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

const char *getEndTime2(){
  const char *time = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_END_TIME2).getString();

  // Validasi format HH:MM
  if (time && strlen(time) == 5 && time[2] == ':') {
    // Update cache
    strncpy(cachedEndTime, time, 5);
    cachedEndTime[5] = '\0';
    return time;
  }
  
  return cachedEndTime;
}

const char *getEndTime3(){
  const char *time = flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_END_TIME3).getString();

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
  if (data == nullptr || len < 2) {
    ESP_LOGE("VARS", "Invalid timeAlarm parameters");
    return;
  }
  
  // Log memory before allocation
  #if LVGL_VERSION_MAJOR >= 9
  lv_mem_monitor_t mon;
  lv_mem_monitor(&mon);
  ESP_LOGI("VARS", "LVGL Memory before timeAlarm - Free: %d, Used: %d", 
           mon.free_size, mon.total_size - mon.free_size);
  #endif
  
  eez::ArrayOfInteger flowArray(2);
  for (int i = 0; i < 2; ++i) {
      flowArray.at(i, data[i]);
  }
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_H, flowArray.value);
  
  // Log memory after allocation
  #if LVGL_VERSION_MAJOR >= 9
  lv_mem_monitor(&mon);
  ESP_LOGI("VARS", "LVGL Memory after timeAlarm - Free: %d, Used: %d", 
           mon.free_size, mon.total_size - mon.free_size);
  #endif
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
  // Validasi input - cegah crash jika data tidak valid
  if(data == nullptr || len == 0) {
    // Set array kosong jika tidak ada data
    eez::ArrayOfString emptyArray(0);
    flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_FILE_AUDIO, emptyArray.value);
    return;
  }
  
  // Validasi setiap elemen array
  bool allValid = true;
  for (size_t i = 0; i < len; i++) {
    if (data[i] == nullptr) {
      allValid = false;
      break;
    }
  }
  
  if (!allValid) {
    // Set array kosong jika ada elemen yang invalid
    eez::ArrayOfString emptyArray(0);
    flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_FILE_AUDIO, emptyArray.value);
    return;
  }
  
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

void setPlayAlarm(bool data, int index){
  if (index == 0)
  {
    flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_PLAY_ALARM, data);
  }else if (index == 1)
  {
    flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_PLAY_ALARM2, data);
  }else if (index == 2)
  {
    flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_PLAY_ALARM3, data);
  }
}

bool playAlarm2(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_PLAY_ALARM2).getBoolean();
}

void setPlayAlarm2(bool data){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_PLAY_ALARM2, data);
}

bool playAlarm3(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_PLAY_ALARM3).getBoolean();
}

void setPlayAlarm3(bool data){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_PLAY_ALARM3, data);
}

int playAudio(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_PLAY_AUDIO).getInt();
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

bool getClosePopup(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_CLOSE_POPUP).getBoolean();
}

void setClosePopup(bool data){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_CLOSE_POPUP, data);
}

int32_t getIndexAlarm(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_ALARMKE).getInt32();
}

int32_t getCairan(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_CAIRAN).getInt32();
}

void setCairan(int data){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_CAIRAN, data);
}

int32_t getCairanTotal(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_CAIRAN_TOTAL).getInt32();
}

void setCairanTotal(int data){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_CAIRAN_TOTAL, data);
}

const char *getSsidSTA(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_SSID_STA).getString();
}

const char *getPassSTA(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_PASSS_STA).getString();
}

const char *getSsidAP(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_SSID_AP).getString();
}

const char *getPassAP(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_PASS_AP).getString();
}

void setSsidSTA(const char* ssid){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_SSID_STA, ssid);
}

void setPassSTA(const char* pass){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_PASSS_STA, pass);
}

void setSsidAP(const char* ssid){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_SSID_AP, ssid);
}

void setPassAP(const char* pass){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_PASS_AP, pass);
}

bool getSaveWifiSTA(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_SAVE_WIFI_STA).getBoolean();
}

void setSavewifiSTA(bool data){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_SAVE_WIFI_STA, data);
}

void setSavewifiAP(bool data){
  flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_SAVE_WIFI_AP, data);
}

bool getSaveWifiAP(){
  return flow::getGlobalVariable(FLOW_GLOBAL_VARIABLE_SAVE_WIFI_AP).getBoolean();
}