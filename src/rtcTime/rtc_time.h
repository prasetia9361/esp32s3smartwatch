// rtc_time.h
// Simple wrapper around RTClib for getting and setting time.
// Provides: begin(), setTime(), getTime() (DateTime), helpers for epoch and formatted string.

#ifndef RTC_TIME_H
#define RTC_TIME_H

#include <Arduino.h>
#include <Wire.h>
#include "SensorPCF85063.hpp"

class RtcTime
{
public:
    // Initialize RTC hardware. Returns true if RTC responded.
    bool begin(uint8_t addr, int sda, int scl);

    // Returns true if RTC date/time seems valid.
    bool isValid() const;

    // Set time. All parameters are required (24h format). Returns true on success.
    bool setTime(int year, int month, int day, int hour, int minute, int second);

    // Get current DateTime from RTC (falls back to compile time if invalid & not yet set).
    RTC_DateTime getTime();

    // Seconds since Unix epoch (UTC).
    uint32_t epoch();

    // ISO8601 formatted string (YYYY-MM-DDTHH:MM:SS).
    String iso8601();

    String printTime();

    String printDate();

    // Parse HH:MM string to minutes since midnight (returns -1 if invalid)
    static int parseHHMM(const char *t);

    // Returns true if current RTC time is inside [start,end)  schedule.
    // If start==end => always active. Handles overnight windows (start > end).
    bool isWithinSchedule(const char *startHHMM, const char *endHHMM);

private:
    SensorPCF85063 *rtc = nullptr; // Using DS3231 (common with RTClib). Adapt if different module.
    bool started = false;
};

extern RtcTime RTC_TIME; // Global instance convenience.

#endif // RTC_TIME_H
