#include "contoll.h"
#include "rtcTime/rtc_time.h"
#include "storage/storage.h"

void Control::init()
{
    // pinMode(PIN_SR501, INPUT);
    // pinMode(PIN_BUZZER, OUTPUT);
    // digitalWrite(PIN_BUZZER, LOW);
    // digitalWrite(PIN_BUZZER, HIGH);
    Serial.println(F("[Control] Initialized pins"));
}

void Control::loop()
{
    unsigned long nowMs = millis();
    bool activeWindow = isScheduleActive();
    if (activeWindow)
    {
        alarmUntil = nowMs + 6000UL; // extend 6 seconds from last motion
    }
    bool alarm = activeWindow && ((long)(nowMs - alarmUntil) < 0);

    if (nowMs - lastStatusPrint > 1000 || alarm != lastActive)
    {
        lastStatusPrint = nowMs;
        lastActive = alarm;
        unsigned long remaining = 0;
        if (alarm && alarmUntil > nowMs)
            remaining = (alarmUntil - nowMs + 999) / 1000; // seconds rounding up
        Serial.printf("[Control] Window:%s Alarm:%s Remaining:%lus Start:%s End:%s\n",
                      activeWindow ? "ON" : "OFF",
                      alarm ? "TRIGGER" : "IDLE",
                      remaining,
                      STORAGE.getStartTime(), STORAGE.getEndTime());
    }
}
