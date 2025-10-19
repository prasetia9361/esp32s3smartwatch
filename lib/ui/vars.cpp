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
  return flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_HH_MM, hhmm);
}

void setCurrentDate(const char* datetime){
  return flow::setGlobalVariable(FLOW_GLOBAL_VARIABLE_CURRENT_DATE, datetime);
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