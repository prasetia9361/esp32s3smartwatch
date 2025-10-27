// #include <Arduino.h>
#ifndef CLIENTSERVER_H
#define CLIENTSERVER_H

#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include "../memory/memory.h"
#include "FS.h"
#include "SD.h"
class Memory;
class clientServer {
   private:
    Memory memory;
    AsyncWebServer server;
    String fileNameDelete;
    String fileNameOpening;
    String getlist;
    void indexHtml();
    void opening();
    void upload();
    void delet();
    void list();
    void notFound();
    /* data */
   public:
    clientServer() : server(80){}
    void begin();
    String getFileOpening() { return fileNameOpening; };
    // void getDataRequest();
};

#endif