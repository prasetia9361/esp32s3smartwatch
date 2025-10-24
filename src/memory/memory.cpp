#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <FS.h>
#include <SD_MMC.h>
#include <stdio.h>  // Optional if not using printf
#include "pin_config.h"
#include "HWCDC.h"
#include "memory.h"

Memory::Memory() : _debugStream(nullptr), _isInitialized(false) {
}

bool Memory::begin(Stream& debugStream) {
    _debugStream = &debugStream;

    SD_MMC.setPins(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);

    if (!SD_MMC.begin("/sdcard", true)) {
        if (_debugStream) {
            _debugStream->println("Card Mount Failed");
        }
        _isInitialized = false;
        return false;
    }

    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
        if (_debugStream) {
            _debugStream->println("No SD_MMC card attached");
        }
        _isInitialized = false;
        return false;
    }

    _isInitialized = true;
    if (_debugStream) {
        _debugStream->println("SD Card initialized successfully.");
        _debugStream->print(getCardInfo());
    }

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
    if (_debugStream) {
        _debugStream->println("Listing directory: " + String(dirname));
    }

    File root = SD_MMC.open(dirname);
    if (!root) {
        if (_debugStream) _debugStream->println("Failed to open directory");
        return "Failed to open directory\n";
    }
    if (!root.isDirectory()) {
        if (_debugStream) _debugStream->println("Not a directory");
        return "Not a directory\n";
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            String dirName = "  DIR : " + String(file.name()) + "\n";
            if (_debugStream) _debugStream->print(dirName);
            dirContent += dirName;
            if (levels) {
                dirContent += listDir(file.path(), levels - 1);
            }
        } else {
            String fileInfo = "  FILE: " + String(file.name()) + "  SIZE: " + String(file.size()) + "\n";
            if (_debugStream) _debugStream->print(fileInfo);
            dirContent += fileInfo;
        }
        file = root.openNextFile();
    }
    return dirContent;
}

// Write complete buffer to file (overwrite)
bool Memory::writeFile(const char* path, const uint8_t* buf, size_t size) {
    if (!_isInitialized) {
        if (_debugStream) _debugStream->println("writeFile: SD not initialized");
        return false;
    }

    // Remove existing file to ensure overwrite
    if (SD_MMC.exists(path)) {
        SD_MMC.remove(path);
    }

    File file = SD_MMC.open(path, FILE_WRITE);
    if (!file) {
        if (_debugStream) _debugStream->printf("writeFile: Failed to open %s\n", path);
        return false;
    }

    size_t written = file.write(buf, size);
    file.close();

    if (written != size) {
        if (_debugStream) _debugStream->printf("writeFile: Written %u of %u bytes to %s\n", (unsigned)written, (unsigned)size, path);
        return false;
    }

    if (_debugStream) _debugStream->printf("writeFile: Wrote %u bytes to %s\n", (unsigned)written, path);
    return true;
}

// Append buffer to file (creates if not exist)
bool Memory::appendFile(const char* path, const uint8_t* buf, size_t size) {
    if (!_isInitialized) {
        if (_debugStream) _debugStream->println("appendFile: SD not initialized");
        return false;
    }

    File file = SD_MMC.open(path, FILE_APPEND);
    if (!file) {
        // Fallback: try FILE_WRITE (some cores map FILE_APPEND to FILE_WRITE)
        file = SD_MMC.open(path, FILE_WRITE);
        if (!file) {
            if (_debugStream) _debugStream->printf("appendFile: Failed to open %s\n", path);
            return false;
        }
        // ensure we append
        file.seek(file.size());
    }

    size_t written = file.write(buf, size);
    file.close();

    if (written != size) {
        if (_debugStream) _debugStream->printf("appendFile: Written %u of %u bytes to %s\n", (unsigned)written, (unsigned)size, path);
        return false;
    }

    if (_debugStream) _debugStream->printf("appendFile: Appended %u bytes to %s\n", (unsigned)written, path);
    return true;
}

// Write audio from a Stream to file in chunks.
// If totalBytes == 0, writes until stream has no more data (may block).
bool Memory::writeAudioFromStream(const char* path, Stream& src, size_t totalBytes, size_t chunkSize) {
    if (!_isInitialized) {
        if (_debugStream) _debugStream->println("writeAudioFromStream: SD not initialized");
        return false;
    }
    if (chunkSize == 0) chunkSize = 1024;
    uint8_t *buf = (uint8_t*)malloc(chunkSize);
    if (!buf) {
        if (_debugStream) _debugStream->println("writeAudioFromStream: malloc failed");
        return false;
    }

    // Remove existing file to overwrite
    if (SD_MMC.exists(path)) SD_MMC.remove(path);

    File file = SD_MMC.open(path, FILE_WRITE);
    if (!file) {
        if (_debugStream) _debugStream->printf("writeAudioFromStream: Failed to open %s\n", path);
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
            if (_debugStream) _debugStream->printf("writeAudioFromStream: write error, wrote %u of %d\n", (unsigned)w, r);
            success = false;
            break;
        }

        if (totalBytes > 0) remaining -= r;
    }

    file.close();
    free(buf);

    if (_debugStream) {
        if (success) _debugStream->printf("writeAudioFromStream: Finished writing to %s\n", path);
        else _debugStream->printf("writeAudioFromStream: Failed writing to %s\n", path);
    }

    return success;
}

// Open a file for reading. Returns an invalid File object on failure.
File Memory::openFile(const char* path) {
    if (!_isInitialized) {
        if (_debugStream) _debugStream->println("openFile: SD not initialized");
        return File(); // Return an invalid File object
    }

    File file = SD_MMC.open(path, FILE_READ);
    if (!file) {
        if (_debugStream) _debugStream->printf("openFile: Failed to open %s for reading\n", path);
    } else {
        if (_debugStream) _debugStream->printf("openFile: Opened %s\n", path);
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
        if (_debugStream) _debugStream->printf("streamFile: %s is a directory, not a file\n", path);
        file.close();
        return false;
    }

    uint8_t *buf = (uint8_t*)malloc(chunkSize);
    if (!buf) {
        if (_debugStream) _debugStream->println("streamFile: malloc failed");
        file.close();
        return false;
    }

    size_t totalRead = 0;
    while (file.available()) {
        size_t bytesRead = file.read(buf, chunkSize);
        if (bytesRead > 0) {
            size_t bytesWritten = dest.write(buf, bytesRead);
            if (bytesWritten != bytesRead) {
                if (_debugStream) _debugStream->printf("streamFile: Destination write failed. Wrote %u of %u bytes.\n", (unsigned)bytesWritten, (unsigned)bytesRead);
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

    if (_debugStream) _debugStream->printf("streamFile: Streamed %u bytes from %s\n", (unsigned)totalRead, path);

    free(buf);
    file.close();
    return true;
}

// Delete a specific file from SD card
bool Memory::deleteFile(const char* path) {
    if (!_isInitialized) {
        if (_debugStream) _debugStream->println("deleteFile: SD not initialized");
        return false;
    }

    // Check if file exists first
    if (!SD_MMC.exists(path)) {
        if (_debugStream) _debugStream->printf("deleteFile: File %s does not exist\n", path);
        return false;
    }

    // Try to delete the file
    bool success = SD_MMC.remove(path);
    
    if (success) {
        if (_debugStream) _debugStream->printf("deleteFile: Successfully deleted %s\n", path);
    } else {
        if (_debugStream) _debugStream->printf("deleteFile: Failed to delete %s\n", path);
    }
    
    return success;
}

// Check if a file exists
bool Memory::fileExists(const char* path) {
    if (!_isInitialized) {
        if (_debugStream) _debugStream->println("fileExists: SD not initialized");
        return false;
    }

    bool exists = SD_MMC.exists(path);
    
    if (_debugStream) {
        _debugStream->printf("fileExists: File %s %s\n", path, exists ? "exists" : "does not exist");
    }
    
    return exists;
}
