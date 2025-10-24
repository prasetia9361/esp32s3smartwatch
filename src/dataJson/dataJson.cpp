#include "dataJson.h"

dataJson::dataJson() {}
void dataJson::setup(dataSpiffs *_spiffs, clientServer *_server) {
    spiffs = _spiffs;
    server = _server;
    writeFile("/config.json");
}

void dataJson::writeFile(const char *path) {
    Serial.printf("Writing file: %s\r\n", path);
    // JsonDocument doc;

    // const char *fileMusik;
    message_ = "/" + server->getFileOpening();
    mp3 = message_.c_str();
    doc["musikOpening"] = mp3;
    // setter
    file = spiffs->writeJson(path);
    if (!file) {
        Serial.println("- failed to open file for writing");
    }
    serializeJson(doc, file);
    Serial.println(mp3);
    file.close();
}

const char *dataJson::readFile(const char *path) {
    // const char *fileMusik;
    // JsonDocument docRead;
    // seter
    fileRead = spiffs->readFileJson(path);
    if (!fileRead || fileRead.isDirectory()) {
        Serial.println("- failed to open file for reading");
        // return;
    }
    char _data[128];
    fileRead.readBytes(_data, fileRead.size());
    DeserializationError error = deserializeJson(docRead, _data);
    if (error) {
        Serial.print("deserializeJson() returned ");
    }
    fileMusik = docRead["musikOpening"];
    Serial.println(fileMusik);
    return fileMusik;
}
