// rtc_time.cpp
#include "rtc_time.h"

RtcTime RTC_TIME;


bool RtcTime::begin(uint8_t addr, int sda, int scl)
{
    if (started){
        return true;
    }

    // Wire.begin();

    if (!rtc) {
        rtc = new SensorPCF85063();
        if (!rtc) {
            return false; // allocation failed
        }
    }

    if (!rtc->begin(Wire, addr, sda, scl))
    {
        delete rtc;
        rtc = nullptr;
        return false;
    }
    started = true;
    return true;
}

bool RtcTime::isValid() const
{
    // We cannot query lostPower() as const (library method non-const), simple heuristic: year > 2024
    RTC_DateTime now = rtc->getDateTime();
    return now.year >= 2024; // Adjust threshold if needed
}

bool RtcTime::setTime(int year, int month, int day, int hour, int minute, int second)
{
    if (year < 2000 || month < 1 || month > 12 || day < 1 || day > 31 || hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59)
        return false;
    rtc->setDateTime(year, month, day, hour, minute, second);
    return true;
}

RTC_DateTime RtcTime::getTime()
{
    // if (!started || !rtc)
    // {
    //     // return fallback DateTime instead of calling rtc methods when rtc not initialized
    //     uint16_t _year = 2025;
    //     uint8_t _month = 10;
    //     uint8_t _day = 24;
    //     uint8_t _hour = 11;
    //     uint8_t _minute = 9;
    //     uint8_t _second = 41;

    //     return RTC_DateTime(_year, _month, _day, _hour, _minute, _second);
    // }
    return rtc->getDateTime();
}

String RtcTime::iso8601()
{
    RTC_DateTime dt = getTime();
    char buf[21]; // "YYYY-MM-DDTHH:MM:SS" + null
    snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
    return String(buf);
}

String RtcTime::printTime()
{
    RTC_DateTime dt = getTime();
    char buf[7]; // "HH:MM" + null
    snprintf(buf, sizeof(buf), "%02d:%02d", dt.hour, dt.minute);
    return String(buf);
}

String RtcTime::printDate()
{
    RTC_DateTime dt = getTime();
    char buf[11]; // "DD/MM/YYYY" + null
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d", dt.day, dt.month, dt.year);
    return String(buf);
}

int RtcTime::parseHHMM(const char *t)
{
    if (!t)
        return -1;
    if (strlen(t) != 5 || t[2] != ':')
        return -1;
    int h = (t[0] - '0') * 10 + (t[1] - '0');
    int m = (t[3] - '0') * 10 + (t[4] - '0');
    if (h < 0 || h > 23 || m < 0 || m > 59)
        return -1;
    return h * 60 + m;
}

bool RtcTime::isWithinSchedule(const char *startHHMM, const char *endHHMM)
{
    int start = parseHHMM(startHHMM);
    int end = parseHHMM(endHHMM);
    if (start < 0 || end < 0)
        return true; // invalid => treat as always active
    if (start == end)
        return true; // full day
    RTC_DateTime now = getTime();
    int current = now.hour * 60 + now.minute;
    if (start < end)
        return current >= start && current < end;
    // crosses midnight
    return current >= start || current < end;
}
