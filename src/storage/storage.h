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
        char setFile[64];
        char startTime[6]; // HH:MM
        char endTime[6];   // HH:MM
    };
    Config config;

    bool loadConfig();

public:
    bool init();
    bool saveWifi(const char *wifiAP, const char *password);
    bool saveSchedule(const char *start, const char *end);
    char *getGifFile() { return config.setFile; }
    char *getWifiAp() { return config.wifiAp; }
    char *getWifiPassword() { return config.wifiPassword; }
    const char *getStartTime() { return config.startTime; }
    const char *getEndTime() { return config.endTime; }
};

extern Storage STORAGE; // Global storage instance

#endif // STORAGE_H