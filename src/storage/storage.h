#ifndef STORAGE_H
#define STORAGE_H

#include <ArduinoJson.h>
#include <SPIFFS.h>

#include "config.h"

class Storage
{
private:
    struct Config
    {
        char wifiAp[64];
        char wifiPassword[64];
        char ssidSTA[64];
        char passSTA[64];
        char setFile[64];
        char startTime[6]; // HH:MM
        char endTime[6];   // HH:MM
        char startTime2[6]; // HH:MM
        char endTime2[6];   // HH:MM
        char startTime3[6]; // HH:MM
        char endTime3[6];   // HH:MM
    };
    Config config;

    bool loadConfig();

public:
    bool init();
    bool saveWifi(const char *wifiAP, const char *password);
    bool saveWifiSTA(const char *wifiSTA, const char *password);
    bool saveSchedule(const char *start, const char *end, int scheduleIndex = 1);
    
    // Helper functions for backward compatibility
    bool saveSchedule2(const char *start, const char *end) { return saveSchedule(start, end, 2); }
    bool saveSchedule3(const char *start, const char *end) { return saveSchedule(start, end, 3); }
    char *getGifFile() { return config.setFile; }
    char *getWifiAp() { return config.wifiAp; }
    char *getWifiPassword() { return config.wifiPassword; }
    char *getSsidSTA() { return config.ssidSTA; }
    char *getPassSTA() { return config.passSTA; }
    const char *getStartTime() { return config.startTime; }
    const char *getEndTime() { return config.endTime; }
    const char *getStartTime2() { return config.startTime2; }
    const char *getEndTime2() { return config.endTime2; }
    const char *getStartTime3() { return config.startTime3; }
    const char *getEndTime3() { return config.endTime3; }
};

extern Storage STORAGE; // Global storage instance

#endif // STORAGE_H