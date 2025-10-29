#include "eez-flow.h"
#include "vars.h"

using namespace eez;

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