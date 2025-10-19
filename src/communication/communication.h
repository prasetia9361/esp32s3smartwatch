#include <Arduino.h>
#include <DNSServer.h>
#include <WiFi.h>

#include "config.h"

class Communication
{
private:
    DNSServer dns; // DNS Server config

public:
    void init();
    void loop();
    void stop();
};