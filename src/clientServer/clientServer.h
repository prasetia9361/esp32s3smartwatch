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
    Memory *_memory;
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
    clientServer(Memory *memory) : _memory(memory), server(80) {}
    void begin();
    String getFileSelected() { return fileNameOpening; };
    bool setFileSelected(String filename) { 
        fileNameOpening = filename; 
        return true;
    };
    // void getDataRequest();
};

#endif