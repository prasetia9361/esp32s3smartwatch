#include "communication.h"
#include "storage/storage.h"

void Communication::init()
{
    Serial.println(F("[Wifi] Setting up"));

    WiFi.softAPsetHostname(SSID_HOSTNAME);
    WiFi.enableAP(true);

    // Load SSID & password from storage (fallback to defaults if empty)
    const char *ssid = STORAGE.getWifiAp();
    const char *pass = STORAGE.getWifiPassword();
    if (!ssid || strlen(ssid) == 0)
        ssid = SSDID_AP;
    if (!pass)
        pass = "";

    bool secure = strlen(pass) >= 8 && strlen(pass) <= 63; // WPA2 limits
    if (!secure && strlen(pass) > 0)
    {
        Serial.println(F("[Wifi] Stored password length invalid (<8); starting OPEN AP"));
    }
    WiFi.mode(WIFI_AP);
    while (!(secure ? WiFi.softAP(ssid, pass, SSID_CHANNEL, false, 4)
                    : WiFi.softAP(ssid)))
    {
        Serial.println(F("[Wifi] Failed to start WiFi AP, retrying in 1 second..."));
        delay(1000);
    }
    WiFi.softAPsetHostname(SSID_HOSTNAME);

    WiFi.config(IPAddress(4, 3, 2, 1), IPAddress(4, 3, 2, 1),
                IPAddress(255, 255, 255, 0), IPAddress(4, 3, 2, 1));

    Serial.printf("[Wifi] SSID: %s\n[Wifi] Ip Address: %s\n[Wifi] Started successfully\n",
                  ssid, WiFi.localIP().toString().c_str());

    Serial.println(F("[DNS] Setting up"));
    WiFi.softAPConfig(IPAddress(4, 3, 2, 1), IPAddress(4, 3, 2, 1),
                      IPAddress(255, 255, 255, 0));

    // Note: encryption type automatic based on password; if open no encryption.
    dns.setTTL(3600); // default TTL

    dns.start(53, "*", WiFi.localIP());
}
void Communication::loop()
{
    dns.processNextRequest();
    delay(100);
}

void Communication::stop()
{
    WiFi.softAPdisconnect(true);
    WiFi.enableAP(false);
    dns.stop();
    Serial.println(F("[Wifi] Access Point stopped"));
}