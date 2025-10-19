// wifi_setup.h
#ifndef WIFI_SETUP_H
#define WIFI_SETUP_H

#include <Arduino.h>

class WiFiSetup {
public:
    WiFiSetup(const char* ssid, const char* password);
    WiFiSetup(const char* ssid, const char* password, const char* fallback_ssid, const char* fallback_password);
    void connect();
    bool isConnected();
    void disconnect();
private:
    const char* ssid;
    const char* password;
    const char* fallback_ssid;
    const char* fallback_password;
    bool has_fallback;
};

#endif