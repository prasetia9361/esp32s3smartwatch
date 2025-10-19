#pragma once
#include "config.h"
#include "rtcTime/rtc_time.h"
#include "storage/storage.h"
#include <Arduino.h>

class Control
{
public:
    void init();
    void loop();

private:
    bool isScheduleActive() { return RTC_TIME.isWithinSchedule(STORAGE.getStartTime(), STORAGE.getEndTime()); }
    unsigned long lastStatusPrint = 0;
    bool lastActive = false;
    unsigned long alarmUntil = 0; // millis timestamp until which alarm stays active
};
