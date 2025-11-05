#ifndef CLIENTSERVER_H
#define CLIENTSERVER_H

#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include "../memory/memory.h"
#include "FS.h"
#include "SD.h"

// Configuration constants
#define WEB_SERVER_PORT 80
#define MAX_UPLOAD_SIZE (3 * 1024 * 1024)  // 3MB
#define MAX_FILES_COUNT 50

class Memory;

class clientServer {
private:
    Memory *_memory;
    AsyncWebServer server;
    String fileNameDelete;
    String fileNameOpening;
    String getlist;
    
    // Private methods for route handlers
    void indexHtml();
    void opening();
    void upload();
    void delet();
    void list();
    void notFound();

public:
    clientServer(Memory *memory) : _memory(memory), server(WEB_SERVER_PORT) {}
    
    // Initialize and start web server
    void begin();
    
    // Getters and setters for selected file
    String getFileSelected() { return fileNameOpening; }
    bool setFileSelected(String filename) { 
        if (filename.length() > 0 && filename.indexOf("..") == -1) {
            fileNameOpening = filename; 
            return true;
        }
        return false;
    }
};

#endif