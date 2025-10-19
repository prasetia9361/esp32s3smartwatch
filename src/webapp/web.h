#ifndef WEB_H
#define WEB_H

#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>

#include "config.h"

class Webapp
{
private:
    AsyncWebServer server;
    AsyncJsonResponse *respose;
    const char *apidata = "{}";
    char tmp[12];

    void indexPage();
    void api();

public:
    Webapp() : server(HTTP_PORT), respose(nullptr) {}
    void init();
    AsyncWebServer *getServer();
};

#endif // WEB_H