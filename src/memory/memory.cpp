#include "memory.h"
#include <Arduino.h>
#include "esp_log.h"

static const char *TAG_MEM = "Memory";

Memory::Memory() : _isInitialized(false) {
}

bool Memory::begin(int clkPin, int cmdPin, int dataPin) {
    SD_MMC.setPins(clkPin, cmdPin, dataPin);

    if (!SD_MMC.begin("/sdcard", true)) {
        ESP_LOGE(TAG_MEM, "SD_MMC.begin() failed");
        _isInitialized = false;
        return false;
    }

    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
        ESP_LOGE(TAG_MEM, "No SD card detected");
        _isInitialized = false;
        return false;
    }

    _isInitialized = true;
    
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    ESP_LOGI(TAG_MEM, "SD card initialized - Size: %lluMB, Type: %s", 
             cardSize, 
             cardType == CARD_MMC ? "MMC" : 
             cardType == CARD_SD ? "SD" : 
             cardType == CARD_SDHC ? "SDHC" : "Unknown");

    return true;
}

String Memory::getCardInfo() {
    if (!_isInitialized) {
        return "SD Card not initialized.\n";
    }

    String info = "SD_MMC Card Type: ";
    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_MMC) {
        info += "MMC\n";
    } else if (cardType == CARD_SD) {
        info += "SDSC\n";
    } else if (cardType == CARD_SDHC) {
        info += "SDHC\n";
    } else {
        info += "UNKNOWN\n";
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    info += "SD_MMC Card Size: " + String(cardSize) + "MB\n";
    
    return info;
}

String Memory::listDir(const char *dirname, uint8_t levels) {
    if (!_isInitialized) {
        return "SD Card not initialized.\n";
    }

    String dirContent = "Listing directory: " + String(dirname) + "\n";

    File root = SD_MMC.open(dirname);
    if (!root) {
        return "Failed to open directory\n";
    }
    if (!root.isDirectory()) {
        return "Not a directory\n";
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            String dirName = "  DIR : " + String(file.name()) + "\n";
            dirContent += dirName;
            if (levels) {
                dirContent += listDir(file.path(), levels - 1);
            }
        } else {
            String fileInfo = "  FILE: " + String(file.name()) + "  SIZE: " + String(file.size()) + "\n";
            dirContent += fileInfo;
        }
        file = root.openNextFile();
    }
    return dirContent;
}

String Memory::listDirJson(const char *dirname) {
    if (!_isInitialized) {
        ESP_LOGW(TAG_MEM, "SD card not initialized");
        return "[]";
    }

    String json = "[";
    bool firstFile = true;
    int fileCount = 0;

    File root = SD_MMC.open(dirname);
    if (!root || !root.isDirectory()) {
        ESP_LOGW(TAG_MEM, "Failed to open directory: %s", dirname);
        return "[]";
    }

    File file = root.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            if (!firstFile) {
                json += ",";
            }
            json += "\"" + String(file.name()) + "\"";
            firstFile = false;
            fileCount++;
        }
        file = root.openNextFile();
    }

    json += "]";
    ESP_LOGD(TAG_MEM, "Listed %d files from %s", fileCount, dirname);
    return json;
}

bool Memory::writeFile(const char* path, const uint8_t* buf, size_t size) {
    if (!_isInitialized) {
        ESP_LOGE(TAG_MEM, "Cannot write - SD not initialized");
        return false;
    }

    if (!buf || size == 0) {
        ESP_LOGE(TAG_MEM, "Invalid buffer or size");
        return false;
    }

    // Remove existing file to ensure overwrite
    if (SD_MMC.exists(path)) {
        if (!SD_MMC.remove(path)) {
            ESP_LOGW(TAG_MEM, "Failed to remove existing file: %s", path);
        }
    }

    File file = SD_MMC.open(path, FILE_WRITE);
    if (!file) {
        ESP_LOGE(TAG_MEM, "Failed to open file for writing: %s", path);
        return false;
    }

    size_t written = file.write(buf, size);
    file.close();

    if (written != size) {
        ESP_LOGE(TAG_MEM, "Write incomplete - expected %d, wrote %d bytes", size, written);
        return false;
    }

    ESP_LOGD(TAG_MEM, "File written: %s (%d bytes)", path, written);
    return true;
}

bool Memory::appendFile(const char* path, const uint8_t* buf, size_t size) {
    if (!_isInitialized) {
        ESP_LOGE(TAG_MEM, "Cannot append - SD not initialized");
        return false;
    }

    if (!buf || size == 0) {
        ESP_LOGE(TAG_MEM, "Invalid buffer or size");
        return false;
    }

    File file = SD_MMC.open(path, FILE_APPEND);
    if (!file) {
        // Fallback: try FILE_WRITE
        file = SD_MMC.open(path, FILE_WRITE);
        if (!file) {
            ESP_LOGE(TAG_MEM, "Failed to open file for append: %s", path);
            return false;
        }
        // Ensure we append
        file.seek(file.size());
    }

    size_t written = file.write(buf, size);
    file.close();

    if (written != size) {
        ESP_LOGE(TAG_MEM, "Append incomplete - expected %d, wrote %d bytes", size, written);
        return false;
    }

    ESP_LOGD(TAG_MEM, "File appended: %s (+%d bytes)", path, written);
    return true;
}

// Write audio from a Stream to file in chunks.
// If totalBytes == 0, writes until stream has no more data (may block).
bool Memory::writeAudioFromStream(const char* path, Stream& src, size_t totalBytes, size_t chunkSize) {
    if (!_isInitialized) {
        return false;
    }
    if (chunkSize == 0) chunkSize = 1024;
    uint8_t *buf = (uint8_t*)malloc(chunkSize);
    if (!buf) {
        return false;
    }

    // Remove existing file to overwrite
    if (SD_MMC.exists(path)) SD_MMC.remove(path);

    File file = SD_MMC.open(path, FILE_WRITE);
    if (!file) {
        free(buf);
        return false;
    }

    size_t remaining = totalBytes;
    bool success = true;

    // If totalBytes == 0 -> write until stream exhausted (use available())
    while ((totalBytes == 0 && src.available() > 0) || (totalBytes > 0 && remaining > 0)) {
        size_t toRead = (totalBytes == 0) ? min((size_t)src.available(), chunkSize) : min(chunkSize, remaining);

        // wait for data if none available but totalBytes specified may block; add small delay
        unsigned long startWait = millis();
        while (src.available() == 0 && totalBytes == 0 && (millis() - startWait) < 200) {
            delay(1);
        }

        int r = src.readBytes((char*)buf, toRead);
        if (r <= 0) {
            // nothing read (stream ended)
            break;
        }

        size_t w = file.write(buf, r);
        if (w != (size_t)r) {
            success = false;
            break;
        }

        if (totalBytes > 0) remaining -= r;
    }

    file.close();
    free(buf);

    return success;
}

// Open a file for reading. Returns an invalid File object on failure.
File Memory::openFile(const char* path) {
    if (!_isInitialized) {
        return File(); // Return an invalid File object
    }

    File file = SD_MMC.open(path, FILE_READ);
    return file;
}

int32_t Memory::totalFile() {
    if (!_isInitialized) {
        return -1;
    }

    File root = SD_MMC.open("/");
    if (!root) {
        return -1;
    }
    if (!root.isDirectory()) {
        root.close();
        return -1;
    }

    int32_t fileCount = 0;
    File file = root.openNextFile();
    while (file) {
        if (!file.isDirectory()) {
            fileCount++;
        }
        file = root.openNextFile();
    }
    root.close();

    return fileCount;
}

// Stream a file's content to a destination Stream in chunks.
bool Memory::streamFile(const char* path, Stream& dest, size_t chunkSize) {
    File file = openFile(path);
    if (!file) {
        return false; // openFile already printed an error
    }
    if (file.isDirectory()) {
        file.close();
        return false;
    }

    uint8_t *buf = (uint8_t*)malloc(chunkSize);
    if (!buf) {
        file.close();
        return false;
    }

    size_t totalRead = 0;
    while (file.available()) {
        size_t bytesRead = file.read(buf, chunkSize);
        if (bytesRead > 0) {
            size_t bytesWritten = dest.write(buf, bytesRead);
            if (bytesWritten != bytesRead) {
                free(buf);
                file.close();
                return false;
            }
            totalRead += bytesRead;
        } else {
            // End of file or read error
            break;
        }
    }

    free(buf);
    file.close();
    return true;
}

bool Memory::deleteFile(const char* path) {
    if (!_isInitialized) {
        ESP_LOGE(TAG_MEM, "Cannot delete - SD not initialized");
        return false;
    }

    if (!SD_MMC.exists(path)) {
        ESP_LOGW(TAG_MEM, "File does not exist: %s", path);
        return false;
    }

    bool success = SD_MMC.remove(path);
    
    if (success) {
        ESP_LOGI(TAG_MEM, "File deleted: %s", path);
    } else {
        ESP_LOGE(TAG_MEM, "Failed to delete file: %s", path);
    }
    
    return success;
}

// Check if a file exists
bool Memory::fileExists(const char* path) {
    if (!_isInitialized) {
        return false;
    }

    bool exists = SD_MMC.exists(path);
    
    return exists;
}

// Read a JSON audio file and deserialize it into a sample buffer.
int Memory::read(const char* path, int16_t *samples, int maxSamples) {
    if (!_isInitialized) {
        return -1; // Error: SD card not ready
    }

    File file = openFile(path);
    if (!file || file.isDirectory()) {
        return -1; // Error: Failed to open file or it's a directory
    }

    // Ukuran dokumen JSON harus cukup besar untuk menampung file.
    // Sesuaikan ukuran ini jika file Anda lebih besar.
    // Ukuran 4096 bytes cocok untuk sekitar 1000-1500 sampel integer.
    DynamicJsonDocument doc(4096);

    // Deserialize file JSON
    DeserializationError error = deserializeJson(doc, file);
    file.close(); // Tutup file setelah selesai dibaca

    if (error) {
        // Gagal mem-parsing JSON
        return -1;
    }

    // Asumsikan root dari JSON adalah sebuah array
    JsonArray array = doc.as<JsonArray>();
    if (array.isNull()) {
        return -1; // Error: JSON root bukan sebuah array
    }

    int samplesRead = 0;
    // Salin data dari JsonArray ke buffer 'samples'
    for (JsonVariant value : array) {
        // Hentikan jika buffer 'samples' sudah penuh
        if (samplesRead >= maxSamples) {
            break;
        }
        // Konversi nilai JSON ke int16_t dan simpan di buffer
        samples[samplesRead] = value.as<int16_t>();
        samplesRead++;
    }

    // Kembalikan jumlah sampel yang berhasil dibaca
    return samplesRead;
}

int32_t Memory::getWavDataSize(const char* path) {
    if (!_isInitialized) {
        ESP_LOGE(TAG_MEM, "Cannot get WAV size - SD not initialized");
        return -1;
    }

    File file = openFile(path);
    if (!file || file.isDirectory()) {
        ESP_LOGE(TAG_MEM, "Failed to open WAV file: %s", path);
        return -1;
    }

    size_t fileSize = file.size();
    file.close();

    if (fileSize < 44) {
        ESP_LOGE(TAG_MEM, "Invalid WAV file (too small): %s", path);
        return -1;
    }

    // WAV data size = total file size - 44 byte header
    int32_t dataSize = fileSize - 44;
    ESP_LOGD(TAG_MEM, "WAV data size: %d bytes (%s)", dataSize, path);
    return dataSize;
}

int32_t Memory::readWavChunk(const char* path, uint8_t* buffer, size_t offset, size_t length) {
    if (!_isInitialized) {
        ESP_LOGE(TAG_MEM, "Cannot read WAV - SD not initialized");
        return -1;
    }

    if (!buffer) {
        ESP_LOGE(TAG_MEM, "Invalid buffer pointer");
        return -1;
    }

    File file = openFile(path);
    if (!file || file.isDirectory()) {
        ESP_LOGE(TAG_MEM, "Failed to open WAV file: %s", path);
        return -1;
    }

    // Skip WAV header (44 bytes) + offset
    if (!file.seek(44 + offset)) {
        ESP_LOGE(TAG_MEM, "Failed to seek in file: %s", path);
        file.close();
        return -1;
    }

    // Read chunk
    int32_t bytesRead = file.read(buffer, length);
    file.close();

    if (bytesRead < 0) {
        ESP_LOGE(TAG_MEM, "Read error: %s", path);
        return -1;
    }

    return bytesRead;
}
