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
        docConfig["start"] = "21:00";
        docConfig["end"] = "00:45";

        if (serializeJson(docConfig, file) == 0) {
            ESP_LOGE(TAG_STORAGE, "Failed to serialize config");
            file.close();
            return false;
        }

        file.close();

        strlcpy(config.wifiAp, SSDID_AP, sizeof(config.wifiAp));
        strlcpy(config.wifiPassword, SSDID_AP_PASSWORD, sizeof(config.wifiPassword));
        strlcpy(config.setFile, "", sizeof(config.setFile));
        strlcpy(config.startTime, "21:00", sizeof(config.startTime));
        strlcpy(config.endTime, "00:45", sizeof(config.endTime));

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

    ESP_LOGI(TAG_STORAGE, "Config loaded - Schedule: %s - %s", config.startTime, config.endTime);
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

bool Storage::saveSchedule(const char *start, const char *end) {
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
    
    if (!valid(start)) {
        ESP_LOGE(TAG_STORAGE, "Start time validation failed");
        return false;
    }
    if (!valid(end)) {
        ESP_LOGE(TAG_STORAGE, "End time validation failed");
        return false;
    }

    File file = SPIFFS.open("/config.json", FILE_WRITE);
    if (!file) {
        ESP_LOGE(TAG_STORAGE, "Failed to open config.json for writing");
        return false;
    }
    
    JsonDocument docConfig;
    docConfig["wifiAp"] = config.wifiAp;
    docConfig["wifiPassword"] = config.wifiPassword;
    docConfig["setFile"] = config.setFile;
    docConfig["start"] = start;
    docConfig["end"] = end;
    
    if (serializeJson(docConfig, file) == 0) {
        ESP_LOGE(TAG_STORAGE, "JSON serialization failed");
        file.close();
        return false;
    }
    file.close();
    
    strlcpy(config.startTime, start, sizeof(config.startTime));
    strlcpy(config.endTime, end, sizeof(config.endTime));
    
    ESP_LOGI(TAG_STORAGE, "Schedule saved: %s - %s", start, end);
    return true;
}