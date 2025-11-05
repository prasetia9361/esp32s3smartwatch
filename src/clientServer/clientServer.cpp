#include "clientServer.h"
#include <WiFi.h>
#include "esp_log.h"

static const char *TAG_WEB = "WebServer";

// Configuration constants
static const size_t MAX_FILE_SIZE = 3 * 1024 * 1024; // 3MB
static const size_t MAX_FILES = 50;
static const char* ALLOWED_EXTENSIONS[] = {".wav", ".WAV"};

void clientServer::begin() {
    ESP_LOGI(TAG_WEB, "Starting web server...");
    indexHtml();
    notFound();
    opening();
    upload();
    delet();
    list();
    server.begin();
    ESP_LOGI(TAG_WEB, "Web server started successfully");
}
void clientServer::indexHtml() {
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
        if (!request->hasParam("filename")) {
            ESP_LOGW(TAG_WEB, "Opening request missing filename parameter");
            request->send(400, "text/plain", "Parameter filename tidak ditemukan.");
            return;
        }
        
        fileNameOpening = request->getParam("filename")->value();
        
        // Validate file exists
        String fullPath = "/" + fileNameOpening;
        if (!_memory->fileExists(fullPath.c_str())) {
            ESP_LOGW(TAG_WEB, "Opening file not found: %s", fileNameOpening.c_str());
            request->send(404, "text/plain", "File tidak ditemukan.");
            return;
        }
        
        ESP_LOGI(TAG_WEB, "Opening file set to: %s", fileNameOpening.c_str());
        request->send(200, "text/plain", "File berhasil menjadi opening.");
    });
}
void clientServer::upload() {
    server.on(
        "/upload", HTTP_POST,
        [this](AsyncWebServerRequest *request) {
            request->send(SPIFFS, "/upload.html", "text/html");
        },
        [this](AsyncWebServerRequest *request, String filename, size_t index,
               uint8_t *data, size_t len, bool final) {
            
            // Validate filename
            if (index == 0) {
                // Security: prevent path traversal
                if (filename.indexOf("..") != -1 || filename.indexOf("/") != -1) {
                    ESP_LOGE(TAG_WEB, "Invalid filename (path traversal attempt): %s", filename.c_str());
                    request->send(400, "text/plain", "Invalid filename");
                    return;
                }
                
                // Validate file extension
                bool validExtension = false;
                for (const char* ext : ALLOWED_EXTENSIONS) {
                    if (filename.endsWith(ext)) {
                        validExtension = true;
                        break;
                    }
                }
                
                if (!validExtension) {
                    ESP_LOGE(TAG_WEB, "Invalid file type: %s", filename.c_str());
                    request->send(400, "text/plain", "Only .wav files allowed");
                    return;
                }
                
                // Check file count limit
                if (_memory->totalFile() >= MAX_FILES) {
                    ESP_LOGE(TAG_WEB, "Maximum file limit reached");
                    request->send(507, "text/plain", "Storage limit reached");
                    return;
                }
                
                ESP_LOGI(TAG_WEB, "Upload started: %s", filename.c_str());
            }
            
            // Check file size limit
            if (index + len > MAX_FILE_SIZE) {
                ESP_LOGE(TAG_WEB, "File too large: %s (%d bytes)", filename.c_str(), index + len);
                request->send(413, "text/plain", "File too large (max 3MB)");
                return;
            }
            
            String filepath = "/" + filename;
            
            if (index == 0) {
                // First chunk: create new file
                if (!_memory->writeFile(filepath.c_str(), data, len)) {
                    ESP_LOGE(TAG_WEB, "Failed to write file: %s", filename.c_str());
                    request->send(500, "text/plain", "Write failed");
                    return;
                }
            } else {
                // Subsequent chunks: append to file
                if (!_memory->appendFile(filepath.c_str(), data, len)) {
                    ESP_LOGE(TAG_WEB, "Failed to append file: %s", filename.c_str());
                    request->send(500, "text/plain", "Append failed");
                    return;
                }
            }
            
            if (final) {
                ESP_LOGI(TAG_WEB, "Upload complete: %s (%d bytes)", filename.c_str(), index + len);
            }
        });
}
void clientServer::delet() {
    server.on("/delete", HTTP_GET, [this](AsyncWebServerRequest *request) {
        if (!request->hasParam("filename")) {
            ESP_LOGW(TAG_WEB, "Delete request missing filename parameter");
            request->send(400, "text/plain", "Parameter filename tidak ditemukan.");
            return;
        }

        fileNameDelete = request->getParam("filename")->value();
        
        // Security: prevent path traversal
        if (fileNameDelete.indexOf("..") != -1 || fileNameDelete.startsWith("/")) {
            ESP_LOGE(TAG_WEB, "Invalid filename (path traversal attempt): %s", fileNameDelete.c_str());
            request->send(400, "text/plain", "Invalid filename");
            return;
        }
        
        String fullPath = "/" + fileNameDelete;
        
        if (_memory->fileExists(fullPath.c_str())) {
            if (_memory->deleteFile(fullPath.c_str())) {
                ESP_LOGI(TAG_WEB, "File deleted: %s", fileNameDelete.c_str());
                request->send(200, "text/plain", "File " + fileNameDelete + " berhasil dihapus.");
            } else {
                ESP_LOGE(TAG_WEB, "Failed to delete file: %s", fileNameDelete.c_str());
                request->send(500, "text/plain", "Gagal menghapus file.");
            }
        } else {
            ESP_LOGW(TAG_WEB, "File not found: %s", fileNameDelete.c_str());
            request->send(404, "text/plain", "File " + fileNameDelete + " tidak ditemukan.");
        }
    });
}
void clientServer::list() {
    server.on("/list", HTTP_GET, [this](AsyncWebServerRequest *request) {
        String jsonList = _memory->listDirJson("/");
        
        if (jsonList == "[]") {
            ESP_LOGW(TAG_WEB, "File list is empty or SD card error");
        }
        
        AsyncWebServerResponse *response = request->beginResponse(
            200, "application/json", jsonList);
        response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
        response->addHeader("Pragma", "no-cache");
        response->addHeader("Expires", "0");
        request->send(response);
    });
}

void clientServer::notFound() {
    const String localUrl = "http://192.168.7.2";
    server.onNotFound([this, localUrl](AsyncWebServerRequest *request) {
        request->redirect(localUrl);
    });
}
