#ifndef MEMORY_H
#define MEMORY_H

#include <ArduinoJson.h>
#include <SD_MMC.h>
#include <SPI.h>
#include <FS.h>
class Memory {
public:
    Memory();
    bool begin(int clkPin, int cmdPin, int dataPin);
    String listDir(const char *dirname, uint8_t levels);
    String listDirJson(const char *dirname); // <-- Tambahkan fungsi baru ini
    String getCardInfo();

    // Write complete buffer to a file (overwrites existing)
    bool writeFile(const char* path, const uint8_t* buf, size_t size);

    // Append buffer to an existing file (creates if not exist)
    bool appendFile(const char* path, const uint8_t* buf, size_t size);

    // Write audio data from a Stream (e.g. incoming data) to file.
    // totalBytes: expected total bytes to write (0 = write until stream exhausted)
    bool writeAudioFromStream(const char* path, Stream& src, size_t totalBytes = 0, size_t chunkSize = 1024);

    // Open a file for reading, returns a File object. Check with 'if (file)'
    File openFile(const char* path);

    // Stream a file's content to a destination Stream (e.g. Serial, WiFiClient, Audio decoder)
    bool streamFile(const char* path, Stream& dest, size_t chunkSize = 1024);

    // Delete a specific file from SD card
    bool deleteFile(const char* path);

    // Check if a file exists
    bool fileExists(const char* path);

    // Read a JSON audio file and deserialize it into a sample buffer.
    // Returns the number of samples read, or -1 on error.
    int read(const char* path, int16_t *samples, int maxSamples);

private:
    Stream* _debugStream;
    bool _isInitialized;
};

#endif // MEMORY_H