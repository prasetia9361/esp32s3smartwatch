#include <Arduino.h>
#ifndef DATAJSON_H
#define DATAJSON_H
#include <ArduinoJson.h>

#include "../clientServer/clientServer.h"
#include "../dataSpiffs/dataSpiffs.h"

class clientServer;
class dataSpiffs;
// #include "FS.h"
class dataJson {
   private:
    clientServer *server;
    dataSpiffs *spiffs;
    const char *fileMusik;
    JsonDocument docRead;
    File fileRead;
    JsonDocument doc;
    String message_;
    const char *mp3;
    File file;
    void writeFile(const char *path);

   public:
    dataJson();
    void setup(dataSpiffs *_spiffs, clientServer *_server);
    const char *readFile(const char *path);
};
#endif