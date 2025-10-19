#include "web.h"
#include "rtcTime/rtc_time.h"
#include "index_page.h"
#include "settings_page.h"
#include "storage/storage.h"
#include <WiFi.h>

AsyncWebServer *Webapp::getServer() { return &server; }

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not Found");
};

void Webapp::init()
{
    server.begin();
    server.onNotFound(notFound);
    indexPage();
    api();
}

void Webapp::indexPage()
{
    Serial.println(F("[Webserver] Setting up page index"));

    auto redirectRoot = [](AsyncWebServerRequest *request)
    {
        request->redirect(String("http://") + WiFi.softAPIP().toString() + "/");
    };
    server.on("/generate_204", HTTP_GET, redirectRoot);        // Android variant
    server.on("/fwlink", HTTP_GET, redirectRoot);              // Windows
    server.on("/hotspot-detect.html", HTTP_GET, redirectRoot); // Apple
    server.on("/library/test/success.html", HTTP_GET, redirectRoot);
    server.on("/connecttest.txt", HTTP_GET, redirectRoot); // Windows 10
    server.on("/ncsi.txt", HTTP_GET, redirectRoot);        // Windows NCSI
    server.on("/success.txt", HTTP_GET, redirectRoot);     // Kindle / misc
    server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                AsyncWebServerResponse *response = request->beginResponse(200, "text/html", (const char *)PAGE_index_html);
                  response->addHeader("Cache-Control", "no-cache");
                  request->send(response); });

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  AsyncWebServerResponse *response = request->beginResponse(200, "text/html", (const char *)PAGE_index_html);
                  response->addHeader("Cache-Control", "no-cache");
                  request->send(response); });

    server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  AsyncWebServerResponse *response = request->beginResponse(200, "text/html", (const char *)PAGE_settings_html);
                  response->addHeader("Cache-Control", "no-cache");
                  request->send(response); });

    // Additional captive portal detection endpoints redirect to root
}

static inline int64_t days_from_civil(int64_t y, unsigned m, unsigned d) {
    y -= m <= 2;
    const int64_t era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);         // [0,399]
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1; // [0,365]
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;        // [0,146096]
    return era * 146097 + static_cast<int64_t>(doe) - 719468;
}

static inline int64_t datetime_to_epoch(int year, int month, int day, int hour, int minute, int second) {
    int64_t days = days_from_civil(year, month, day);
    return days * 86400LL + (int64_t)hour * 3600 + (int64_t)minute * 60 + (int64_t)second;
}


void Webapp::api()
{
    Serial.println(F("[Webserver] Setting up time API"));

    // GET /api/time -> {"iso":"YYYY-MM-DDTHH:MM:SS","epoch":1234567890}
    server.on("/api/time", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        RTC_DateTime dt = RTC_TIME.getTime();
        char iso[21];

        snprintf(iso, sizeof(iso), "%04d-%02d-%02dT%02d:%02d:%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
        // hitung epoch manual karena SensorPCF85063 RTC_DateTime tidak punya unixtime()
        int64_t epoch = datetime_to_epoch(dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);

        char body[80];
        snprintf(body, sizeof(body), "{\"iso\":\"%s\",\"epoch\":%lu}", iso, (long long)epoch);
        request->send(200, "application/json", body); });

    // POST /api/time?year=2025&month=8&day=16&hour=12&minute=34&second=56
    server.on("/api/time", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        int params = request->params();
        int year=-1, month=-1, day=-1, hour=-1, minute=-1, second=-1;
        for (int i = 0; i < params; i++)
        {
            const AsyncWebParameter *p = request->getParam(i);
            const String &name = p->name();
            int val = p->value().toInt();
            if (name == "year") year = val;
            else if (name == "month") month = val;
            else if (name == "day") day = val;
            else if (name == "hour") hour = val;
            else if (name == "minute") minute = val;
            else if (name == "second") second = val;
        }
        if (year < 2000 || month < 1 || month > 12 || day < 1 || day > 31 || hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59)
        {
            request->send(400, "application/json", "{\"error\":\"invalid parameters\"}");
            return;
        }
        bool ok = RTC_TIME.setTime(year, month, day, hour, minute, second);
        if (!ok)
        {
            request->send(500, "application/json", "{\"error\":\"failed set time\"}");
            return;
        }
        request->send(200, "application/json", "{\"status\":\"ok\"}"); });

    // GET /api/wifi -> {"ssid":"...","password":"..."}
    server.on("/api/wifi", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                  const String live = WiFi.softAPSSID();
                  char body[220];
                  snprintf(body, sizeof(body), "{\"ssid\":\"%s\",\"live\":\"%s\",\"password\":\"%s\"}", STORAGE.getWifiAp(), live.c_str(), STORAGE.getWifiPassword());
                  request->send(200, "application/json", body); });

    // POST /api/wifi ssid=..&password=..
    server.on("/api/wifi", HTTP_POST, [](AsyncWebServerRequest *request)
              {
                  const char *ssid = nullptr; const char *pwd = nullptr;
                  for (int i=0;i<request->params();++i){
                      const AsyncWebParameter *p = request->getParam(i);
                      if(p->name()=="ssid") ssid = p->value().c_str();
                      else if(p->name()=="password") pwd = p->value().c_str();
                  }
                  if(!ssid){ request->send(400, "application/json", "{\"error\":\"missing ssid\"}"); return; }
                  if(!pwd) pwd=""; // allow empty password
                  if(strlen(ssid) > 63 || strlen(pwd) > 63){ request->send(400, "application/json", "{\"error\":\"too long\"}"); return; }
                  if(strlen(pwd) > 0 && strlen(pwd) < 8){ request->send(400, "application/json", "{\"error\":\"pwd_len\"}"); return; }
                  if(!STORAGE.saveWifi(ssid, pwd)) { request->send(500, "application/json", "{\"error\":\"save failed\"}"); return; }
                  // Restart AP so change takes effect without full reboot
                  WiFi.softAPdisconnect(true);
                  delay(100);
                  bool secure = strlen(pwd) >= 8;
                  WiFi.mode(WIFI_AP);
                  bool started = secure ? WiFi.softAP(ssid, pwd, SSID_CHANNEL, false, 4) : WiFi.softAP(ssid);
                  if(!started){
                      request->send(200, "application/json", "{\"status\":\"ok\",\"apRestart\":\"failed\"}");
                      return;
                  }
                  request->send(200, "application/json", "{\"status\":\"ok\",\"apRestart\":\"success\"}"); });

    // GET /api/schedule -> {"start":"HH:MM","end":"HH:MM"}
    server.on("/api/schedule", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        char body[64];
        snprintf(body, sizeof(body), "{\"start\":\"%s\",\"end\":\"%s\"}", STORAGE.getStartTime(), STORAGE.getEndTime());
        request->send(200, "application/json", body); });
    // POST /api/schedule start=HH:MM&end=HH:MM
    server.on("/api/schedule", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        const char *start=nullptr; const char *end=nullptr;
        for(int i=0;i<request->params();++i){
            const AsyncWebParameter *p=request->getParam(i);
            if(p->name()=="start") start=p->value().c_str();
            else if(p->name()=="end") end=p->value().c_str();
        }
        if(!start || !end){ request->send(400, "application/json", "{\"error\":\"missing params\"}"); return; }
        if(!STORAGE.saveSchedule(start,end)){ request->send(400, "application/json", "{\"error\":\"invalid\"}"); return; }
        request->send(200, "application/json", "{\"status\":\"ok\"}"); });
}