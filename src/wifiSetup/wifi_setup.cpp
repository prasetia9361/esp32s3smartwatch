// wifi_setup.cpp
#include "wifi_setup.h"
#include <WiFi.h>

WiFiSetup::WiFiSetup(const char* ssid, const char* password) {
    this->ssid = ssid;
    this->password = password;
    this->fallback_ssid = nullptr;
    this->fallback_password = nullptr;
    this->has_fallback = false;
}

WiFiSetup::WiFiSetup(const char* ssid, const char* password, const char* fallback_ssid, const char* fallback_password) {
    this->ssid = ssid;
    this->password = password;
    this->fallback_ssid = fallback_ssid;
    this->fallback_password = fallback_password;
    this->has_fallback = true;
}

void WiFiSetup::connect() {
    // Try primary network first
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to primary WiFi: ");
    Serial.println(ssid);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nPrimary WiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        return;
    }
    
    // If primary fails and fallback exists, try fallback
    if (has_fallback) {
        Serial.println("\nPrimary WiFi failed, trying fallback...");
        WiFi.begin(fallback_ssid, fallback_password);
        Serial.print("Connecting to fallback WiFi: ");
        Serial.println(fallback_ssid);
        
        attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nFallback WiFi connected!");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("\nBoth WiFi networks failed to connect!");
        }
    } else {
        Serial.println("\nWiFi connection failed!");
    }
}

bool WiFiSetup::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiSetup::disconnect(){
    WiFi.disconnect();
}