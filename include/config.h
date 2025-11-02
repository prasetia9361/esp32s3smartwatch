#pragma once
#include <Arduino.h>
#define WEBSOCKET_PATH "/ws"
#define HTTP_PORT 80
#define SERIAL_BAUDRATE 115200
#define SERIAL_TIMEOUT 1000
#define SSDID_AP "KiBo Alarm"
#define SSDID_AP_PASSWORD "kibolab123"
// WiFi Configuration
#define WIFI_SSID "wefee"
#define WIFI_PASSWORD "wepaywefee"
// Fallback WiFi Configuration
#define WIFI_FALLBACK_SSID "wefee"
#define WIFI_FALLBACK_PASSWORD "wepaywefee"
#define SSID_HOSTNAME "blink-epilepsy"
#define SSID_CHANNEL 11
#define DNS_HOSTNAME "blink-epilepsy.local"
#define NTP_TIMEZONE "WIB-7"

// NTP Server Configuration
#define NTP_SERVER1 "id.pool.ntp.org"
#define NTP_SERVER2 "id.pool.ntp.org"
#define NTP_SERVER3 "time.google.com"

// NTP Update Interval (in milliseconds)
#define NTP_UPDATE_INTERVAL 3600000  // 1 hour

