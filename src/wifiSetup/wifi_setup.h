#pragma once
#include <DNSServer.h>
#include <WiFi.h>
class WifiSetup {
   private:
    DNSServer dnsServer;

    const char *ssid1 = "KiBo Alarm";
    const char *password1 = "kibolab123";
    bool mode = false;
    String macAddress = WiFi.macAddress();

   public:
    // void begin();
    WifiSetup();
    // void begin();
    void setupWiFiAP();
    void loopDns();
    void setupWiFiSTA(const String& ssid, const String& pass);
    void connectAP();
    void connectSTA();
    // String getMac() { return macAddress; }
    void disconnectAP();
    void disconnectSTA();
    bool isConnected() { return mode; }
    bool reconnect() { return WiFi.reconnect(); }
    void disconnect();
};
