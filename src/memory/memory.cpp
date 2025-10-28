#include "memory.h"

Memory::Memory() : _isInitialized(false) {
}

bool Memory::begin(int clkPin, int cmdPin, int dataPin) {

    SD_MMC.setPins(clkPin, cmdPin, dataPin);

    if (!SD_MMC.begin("/sdcard", true)) {
        _isInitialized = false;
        return false;
    }

    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
        _isInitialized = false;
        return false;
    }

    _isInitialized = true;

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
        return "[]"; // Kembalikan array JSON kosong jika tidak terinisialisasi
    }

    String json = "[";
    bool firstFile = true;

    File root = SD_MMC.open(dirname);
    if (!root || !root.isDirectory()) {
        return "[]"; // Kembalikan array JSON kosong jika direktori tidak ada
    }

    File file = root.openNextFile();
    while (file) {
        // Hanya proses file, abaikan direktori
        if (!file.isDirectory()) {
            if (!firstFile) {
                json += ","; // Tambahkan koma sebelum elemen berikutnya
            }
            json += "\"" + String(file.name()) + "\"";
            firstFile = false;
        }
        file = root.openNextFile();
    }

    json += "]";
    return json;
}

// Write complete buffer to file (overwrite)
bool Memory::writeFile(const char* path, const uint8_t* buf, size_t size) {
    if (!_isInitialized) {
        return false;
    }

    // Remove existing file to ensure overwrite
    if (SD_MMC.exists(path)) {
        SD_MMC.remove(path);
    }

    File file = SD_MMC.open(path, FILE_WRITE);
    if (!file) {
        return false;
    }

    size_t written = file.write(buf, size);
    file.close();

    if (written != size) {
        return false;
    }

    return true;
}

// Append buffer to file (creates if not exist)
bool Memory::appendFile(const char* path, const uint8_t* buf, size_t size) {
    if (!_isInitialized) {
        return false;
    }

    File file = SD_MMC.open(path, FILE_APPEND);
    if (!file) {
        // Fallback: try FILE_WRITE (some cores map FILE_APPEND to FILE_WRITE)
        file = SD_MMC.open(path, FILE_WRITE);
        if (!file) {
            return false;
        }
        // ensure we append
        file.seek(file.size());
    }

    size_t written = file.write(buf, size);
    file.close();

    if (written != size) {
        return false;
    }

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
    if (!file) {
    } else {
    }
    return file;
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

// Delete a specific file from SD card
bool Memory::deleteFile(const char* path) {
    if (!_isInitialized) {
        return false;
    }

    // Check if file exists first
    if (!SD_MMC.exists(path)) {
        return false;
    }

    // Try to delete the file
    bool success = SD_MMC.remove(path);
    
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
