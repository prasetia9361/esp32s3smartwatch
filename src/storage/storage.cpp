#include "storage.h"
#include "esp_log.h"

static const char *TAG_STORAGE = "Storage";

Storage STORAGE;

bool Storage::init() {
    ESP_LOGI(TAG_STORAGE, "Initializing SPIFFS...");
    
    if (!SPIFFS.begin(true)) {
        ESP_LOGE(TAG_STORAGE, "SPIFFS begin failed");
        return false;
    }
    
    loadConfig();
    ESP_LOGI(TAG_STORAGE, "SPIFFS initialized successfully");
    return true;
}

bool Storage::loadConfig() {
    if (!SPIFFS.exists("/config.json")) {
        ESP_LOGI(TAG_STORAGE, "Config file not found, creating default...");
        
        File file = SPIFFS.open("/config.json", FILE_WRITE);
        if (!file) {
            ESP_LOGE(TAG_STORAGE, "Failed to create config file");
            return false;
        }

        JsonDocument docConfig;
        docConfig["wifiAp"] = SSDID_AP;
        docConfig["wifiPassword"] = SSDID_AP_PASSWORD;
        docConfig["setFile"] = "";
        docConfig["start"] = "07:00";
        docConfig["end"] = "07:45";
        docConfig["start2"] = "12:00";
        docConfig["end2"] = "12:05";
        docConfig["start3"] = "16:00";
        docConfig["end3"] = "16:02";


        if (serializeJson(docConfig, file) == 0) {
            ESP_LOGE(TAG_STORAGE, "Failed to serialize config");
            file.close();
            return false;
        }

        file.close();

        strlcpy(config.wifiAp, SSDID_AP, sizeof(config.wifiAp));
        strlcpy(config.wifiPassword, SSDID_AP_PASSWORD, sizeof(config.wifiPassword));
        strlcpy(config.setFile, "", sizeof(config.setFile));
        strlcpy(config.startTime, "07:00", sizeof(config.startTime));
        strlcpy(config.endTime, "07:45", sizeof(config.endTime));
        strlcpy(config.startTime2, "12:00", sizeof(config.startTime2));
        strlcpy(config.endTime2, "12:05", sizeof(config.endTime2));
        strlcpy(config.startTime3, "16:00", sizeof(config.startTime3));
        strlcpy(config.endTime3, "16:02", sizeof(config.endTime3));

        ESP_LOGI(TAG_STORAGE, "Default config created");
        return true;
    }

    File file = SPIFFS.open("/config.json", FILE_READ);
    if (!file) {
        ESP_LOGE(TAG_STORAGE, "Failed to open config file for reading");
        return false;
    }

    JsonDocument docConfig;
    DeserializationError error = deserializeJson(docConfig, file);
    file.close();

    if (error) {
        ESP_LOGE(TAG_STORAGE, "Failed to parse config JSON: %s", error.c_str());
        return false;
    }

    strlcpy(config.wifiAp, docConfig["wifiAp"] | SSDID_AP, sizeof(config.wifiAp));
    strlcpy(config.wifiPassword, docConfig["wifiPassword"] | SSDID_AP_PASSWORD, sizeof(config.wifiPassword));
    strlcpy(config.setFile, docConfig["setFile"] | "", sizeof(config.setFile));
    strlcpy(config.startTime, docConfig["start"] | "21:00", sizeof(config.startTime));
    strlcpy(config.endTime, docConfig["end"] | "00:45", sizeof(config.endTime));
    strlcpy(config.startTime2, docConfig["start2"] | "12:00", sizeof(config.startTime2));
    strlcpy(config.endTime2, docConfig["end2"] | "12:05", sizeof(config.endTime2));
    strlcpy(config.startTime3, docConfig["start3"] | "16:00", sizeof(config.startTime3));
    strlcpy(config.endTime3, docConfig["end3"] | "16:02", sizeof(config.endTime3));

    ESP_LOGI(TAG_STORAGE, "Config loaded - Schedule: %s - %s", config.startTime, config.endTime);
    return true;
}

bool Storage::saveWifi(const char *wifiAP, const char *password)
{
    // Read current config to preserve all schedule data
    File readFile = SPIFFS.open("/config.json", FILE_READ);
    JsonDocument docConfig;
    if (readFile) {
        deserializeJson(docConfig, readFile);
        readFile.close();
    }

    // Update WiFi credentials only
    docConfig["wifiAp"] = wifiAP;
    docConfig["wifiPassword"] = password;
    docConfig["setFile"] = config.setFile;
    
    // Preserve all schedule data
    docConfig["start"] = config.startTime;
    docConfig["end"] = config.endTime;
    docConfig["start2"] = config.startTime2;
    docConfig["end2"] = config.endTime2;
    docConfig["start3"] = config.startTime3;
    docConfig["end3"] = config.endTime3;

    File writeFile = SPIFFS.open("/config.json", FILE_WRITE);
    if (!writeFile) {
        ESP_LOGE(TAG_STORAGE, "Failed to open config.json for writing");
        return false;
    }

    if (serializeJson(docConfig, writeFile) == 0) {
        ESP_LOGE(TAG_STORAGE, "JSON serialization failed");
        writeFile.close();
        return false;
    }

    // Update internal config
    strlcpy(config.wifiAp, wifiAP, sizeof(config.wifiAp));
    strlcpy(config.wifiPassword, password, sizeof(config.wifiPassword));

    writeFile.close();
    ESP_LOGI(TAG_STORAGE, "WiFi config saved: %s", wifiAP);
    return true;
}

bool Storage::saveSchedule(const char *start, const char *end, int scheduleIndex) {
    // Validate schedule index
    if (scheduleIndex < 1 || scheduleIndex > 3) {
        ESP_LOGE(TAG_STORAGE, "Invalid schedule index: %d (must be 1-3)", scheduleIndex);
        return false;
    }
    
    // Validate format HH:MM
    auto valid = [](const char *t) -> bool {
        if (!t) {
            ESP_LOGE(TAG_STORAGE, "Time is NULL");
            return false;
        }
        if (strlen(t) != 5) {
            ESP_LOGE(TAG_STORAGE, "Invalid time length: %d (expected 5)", strlen(t));
            return false;
        }
        if (t[2] != ':') {
            ESP_LOGE(TAG_STORAGE, "Missing colon at position 2");
            return false;
        }
        int h = (t[0] - '0') * 10 + (t[1] - '0');
        int m = (t[3] - '0') * 10 + (t[4] - '0');
        if (h < 0 || h > 23 || m < 0 || m > 59) {
            ESP_LOGE(TAG_STORAGE, "Invalid time values - h:%d, m:%d", h, m);
            return false;
        }
        return true;
    };
    
    if (!valid(start) || !valid(end)) {
        ESP_LOGE(TAG_STORAGE, "Time validation failed");
        return false;
    }

    // Read current config to preserve other values
    File readFile = SPIFFS.open("/config.json", FILE_READ);
    JsonDocument docConfig;
    if (readFile) {
        deserializeJson(docConfig, readFile);
        readFile.close();
    }

    // Set common fields (preserve existing values)
    docConfig["wifiAp"] = config.wifiAp;
    docConfig["wifiPassword"] = config.wifiPassword; 
    docConfig["setFile"] = config.setFile;
    
    // Set schedule-specific fields based on index
    String startKey, endKey;
    char *targetStart, *targetEnd;
    
    switch(scheduleIndex) {
        case 1:
            startKey = "start";
            endKey = "end";
            targetStart = config.startTime;
            targetEnd = config.endTime;
            break;
        case 2:
            startKey = "start2";
            endKey = "end2";
            targetStart = config.startTime2;
            targetEnd = config.endTime2;
            break;
        case 3:
            startKey = "start3";
            endKey = "end3";
            targetStart = config.startTime3;
            targetEnd = config.endTime3;
            break;
    }
    
    docConfig[startKey] = start;
    docConfig[endKey] = end;

    // Write updated config
    File writeFile = SPIFFS.open("/config.json", FILE_WRITE);
    if (!writeFile) {
        ESP_LOGE(TAG_STORAGE, "Failed to open config.json for writing");
        return false;
    }
    
    if (serializeJson(docConfig, writeFile) == 0) {
        ESP_LOGE(TAG_STORAGE, "JSON serialization failed");
        writeFile.close();
        return false;
    }
    writeFile.close();
    
    // Update internal config
    strlcpy(targetStart, start, 6);
    strlcpy(targetEnd, end, 6);
    
    ESP_LOGI(TAG_STORAGE, "Schedule%d saved: %s - %s", scheduleIndex, start, end);
    return true;
}

