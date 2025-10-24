#include "clientServer.h"
// comunication::comunication(dataSpiffs &Spiffs) : _Spiffs(Spiffs) {}
void clientServer::setup(Memory *_memory) {
    memory = _memory;
    indexHtml();
    opening();
    upload();
    delet();
    list();
    notFound();
    init();
}
void clientServer::indexHtml() {
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html");
    });
}

void clientServer::opening() {
    server.on("/opening", HTTP_GET, [this](AsyncWebServerRequest *request) {
        // String fileNameOpening;
        char array[20];
        if (request->hasParam("filename")) {
            fileNameOpening = request->getParam("filename")->value();
            // getter

            //  _json.writeFile("/config.json", fileNameOpening);
            request->send(200, "text/plain",
                          "File berhasil berhasil menjadi opening.");
        } else {
            request->send(400, "text/plain",
                          "Parameter filename tidak ditemukan.");
        }
    });
}
void clientServer::upload() {
    server.on(
        "/upload", HTTP_POST,
        [this](AsyncWebServerRequest *request) {
            // request->send(200, "text/plain", "File uploaded successfully");
            request->send(SPIFFS, "/upload.html", "text/html");
        },
        [this](AsyncWebServerRequest *request, String filename, size_t index,
               uint8_t *data, size_t len, bool final) {
            // setter
            memory->writeFile(("/" + filename).c_str(), data, len);
        });
}
void clientServer::delet() {
    server.on("/delete", HTTP_GET, [this](AsyncWebServerRequest *request) {
        // String fileNameDelete;

        if (request->hasParam("filename")) {
            fileNameDelete = request->getParam("filename")->value();
            // seter
            if (memory->fileExists(("/" + fileNameDelete).c_str())) {
                memory->deleteFile(("/" + fileNameDelete).c_str());
                request->send(200, "text/plain",
                              "File " + fileNameDelete + " berhasil dihapus.");
            } else {
                request->send(404, "text/plain",
                              "File " + fileNameDelete + " tidak ditemukan.");
            }
        } else {
            request->send(400, "text/plain",
                          "Parameter filename tidak ditemukan.");
        }
    });
}
void clientServer::list() {
    server.on("/list", HTTP_GET, [this](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(
            200, "application/json", memory->listDir("/",  0));  // setter
        request->send(response);
    });
}
void clientServer::notFound() {
    const String localUrl = "http://192.168.7.1";
    server.onNotFound([this, localUrl](AsyncWebServerRequest *request) {
        request->redirect(localUrl);
        Serial.print("onnotfound ");
        Serial.print(
            request->host());  // This gives some insight into whatever was
                               // being requested on the serial monitor
        Serial.print(" ");
        Serial.print(request->url());
        Serial.print(" sent redirect to " + localUrl + "\n");
    });
}
void clientServer::init() { server.begin(); }
