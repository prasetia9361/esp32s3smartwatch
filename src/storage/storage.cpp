#include "storage.h"

Storage STORAGE; // definition

bool Storage::init()
{
    Serial.println("[SPIFFS] Initializing Storage");
    if (!SPIFFS.begin(true))
    {
        Serial.println(F("[SPIFFS] Fail Begin SPIFFS"));
        return false;
    }
    loadConfig();
    Serial.println(F("[SPIFFS] Success Begin SPIFFS"));
    return true;
}

bool Storage::loadConfig()
{
    if (!SPIFFS.exists("/config.json"))
    {
        File file = SPIFFS.open("/config.json", FILE_WRITE);
        if (!file)
        {
            Serial.println(F("[SPIFFS] Fail Open SPIFFS for writing"));
            return false;
        }

        JsonDocument docConfig;
        docConfig["wifiAp"] = SSDID_AP;
        docConfig["wifiPassword"] = SSDID_AP_PASSWORD;
        docConfig["setFile"] = "";
        docConfig["start"] = "21:00";
        
        docConfig["end"] = "00:45";

        if (serializeJson(docConfig, file) == 0)
        {
            Serial.println(F("[SPIFFS] Fail Serialize Json"));
            file.close();
            return false;
        }

        file.close();
        Serial.println(F("[SPIFFS] Created default config file"));

        strlcpy(config.wifiAp, SSDID_AP, sizeof(config.wifiAp));
        strlcpy(config.wifiPassword, SSDID_AP_PASSWORD, sizeof(config.wifiPassword));
        strlcpy(config.setFile, "", sizeof(config.setFile));
        strlcpy(config.startTime, "21:00", sizeof(config.startTime));
        strlcpy(config.endTime, "00:45", sizeof(config.endTime));

        return true;
    }

    File file = SPIFFS.open("/config.json", FILE_READ);
    if (!file)
    {
        Serial.println(F("[SPIFFS] Fail Open SPIFFS for reading"));
        return false;
    }

    JsonDocument docConfig;
    DeserializationError error = deserializeJson(docConfig, file);
    file.close();

    if (error)
    {
        Serial.println(F("[SPIFFS] Fail Deserialize Json"));
        return false;
    }

    strlcpy(config.wifiAp, docConfig["wifiAp"] | SSDID_AP, sizeof(config.wifiAp));
    strlcpy(config.wifiPassword, docConfig["wifiPassword"] | SSDID_AP_PASSWORD, sizeof(config.wifiPassword));
    strlcpy(config.setFile, docConfig["setFile"] | "", sizeof(config.setFile));
    strlcpy(config.startTime, docConfig["start"] | "21:00", sizeof(config.startTime));
    strlcpy(config.endTime, docConfig["end"] | "00:45", sizeof(config.endTime));

    Serial.println(F("[SPIFFS] Success Load Config"));
    return true;
}

bool Storage::saveWifi(const char *wifiAP, const char *password)
{
    File file = SPIFFS.open("/config.json", FILE_WRITE);
    if (!file)
    {
        Serial.println(F("[SPIFFS] Fail Open SPIFFS"));
        return false;
    }

    JsonDocument docConfig;
    docConfig["wifiAp"] = wifiAP;
    docConfig["wifiPassword"] = password;
    docConfig["setFile"] = config.setFile;
    docConfig["start"] = config.startTime;
    docConfig["end"] = config.endTime;

    if (serializeJson(docConfig, file) == 0)
    {
        file.close();
        return false;
    }

    strlcpy(config.wifiAp, wifiAP, sizeof(config.wifiAp));
    strlcpy(config.wifiPassword, password, sizeof(config.wifiPassword));

    file.close();
    return true;
}

bool Storage::saveSchedule(const char *start, const char *end)
{
    Serial.println("[SPIFFS] saveSchedule called");
    Serial.print("[SPIFFS] start: ");
    Serial.println(start ? start : "NULL");
    Serial.print("[SPIFFS] end: ");
    Serial.println(end ? end : "NULL");
    
    // validate format HH:MM
    auto valid = [](const char *t) -> bool
    {
        if (!t) {
            Serial.println("[SPIFFS] Time is NULL");
            return false;
        }
        if (strlen(t) != 5) {
            Serial.printf("[SPIFFS] Invalid length: %d (expected 5)\n", strlen(t));
            return false;
        }
        if (t[2] != ':') {
            Serial.println("[SPIFFS] Missing colon at position 2");
            return false;
        }
        int h = (t[0] - '0') * 10 + (t[1] - '0');
        int m = (t[3] - '0') * 10 + (t[4] - '0');
        if (h < 0 || h > 23 || m < 0 || m > 59) {
            Serial.printf("[SPIFFS] Invalid time values - h:%d, m:%d\n", h, m);
            return false;
        }
        return true;
    };
    
    if (!valid(start)) {
        Serial.println("[SPIFFS] Start time validation failed");
        return false;
    }
    if (!valid(end)) {
        Serial.println("[SPIFFS] End time validation failed");
        return false;
    }

    Serial.println("[SPIFFS] Time validation passed, opening file...");
    File file = SPIFFS.open("/config.json", FILE_WRITE);
    if (!file) {
        Serial.println("[SPIFFS] Failed to open config.json for writing");
        return false;
    }
    
    Serial.println("[SPIFFS] Creating JSON document...");
    JsonDocument docConfig;
    docConfig["wifiAp"] = config.wifiAp;
    docConfig["wifiPassword"] = config.wifiPassword;
    docConfig["setFile"] = config.setFile;
    docConfig["start"] = start;
    docConfig["end"] = end;
    
    if (serializeJson(docConfig, file) == 0)
    {
        Serial.println("[SPIFFS] JSON serialization failed");
        file.close();
        return false;
    }
    file.close();
    
    strlcpy(config.startTime, start, sizeof(config.startTime));
    strlcpy(config.endTime, end, sizeof(config.endTime));
    
    Serial.println("[SPIFFS] Schedule saved successfully");
    return true;
}