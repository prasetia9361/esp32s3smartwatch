#include "clientServer.h"
#include <WiFi.h>
// comunication::comunication(dataSpiffs &Spiffs) : _Spiffs(Spiffs) {}

void clientServer::begin() {
    server.begin();
    indexHtml();
    opening();
    upload();
    delet();
    list();
    notFound();
    // server.onNotFound(notFound);
    // return true;
}
void clientServer::indexHtml() {
    // Serial.println(F("[Webserver] Setting up page index"));

    const String localUrl = "http://192.168.7.2";
    // Handler untuk captive portal yang mengarahkan ke halaman utama
    auto redirectRoot = [this, localUrl](AsyncWebServerRequest *request) {
        request->redirect(localUrl);
    };
    server.on("/generate_204", HTTP_GET, redirectRoot);        // Android
    server.on("/fwlink", HTTP_GET, redirectRoot);              // Windows
    server.on("/hotspot-detect.html", HTTP_GET, redirectRoot); // Apple
    server.on("/library/test/success.html", HTTP_GET, redirectRoot);
    server.on("/connecttest.txt", HTTP_GET, redirectRoot); // Windows 10
    server.on("/ncsi.txt", HTTP_GET, redirectRoot);        // Windows NCSI
    server.on("/success.txt", HTTP_GET, redirectRoot);     // Kindle / misc

    // Handler untuk menyajikan index.html di rute root
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
            memory.writeFile(("/" + filename).c_str(), data, len);
        });
}
void clientServer::delet() {
    server.on("/delete", HTTP_GET, [this](AsyncWebServerRequest *request) {
        // String fileNameDelete;

        if (request->hasParam("filename")) {
            fileNameDelete = request->getParam("filename")->value();
            // seter
            if (memory.fileExists(("/" + fileNameDelete).c_str())) {
                memory.deleteFile(("/" + fileNameDelete).c_str());
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
            200, "application/json", memory.listDir("/",  1));  // setter
        request->send(response);
    });
}

void clientServer::notFound() {
    const String localUrl = "http://192.168.7.2";
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
