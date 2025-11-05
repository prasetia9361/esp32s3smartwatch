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

    int32_t totalFile();

    // Stream a file's content to a destination Stream (e.g. Serial, WiFiClient, Audio decoder)
    bool streamFile(const char* path, Stream& dest, size_t chunkSize = 1024);

    // Delete a specific file from SD card
    bool deleteFile(const char* path);

    // Check if a file exists
    bool fileExists(const char* path);

    // Read a JSON audio file and deserialize it into a sample buffer.
    // Returns the number of samples read, or -1 on error.
    int read(const char* path, int16_t *samples, int maxSamples);

    // Read WAV file and return file size (excluding 44-byte header)
    // Returns data size in bytes, or -1 on error
    int32_t getWavDataSize(const char* path);
    
    // Read WAV audio data in chunks (skips 44-byte header automatically)
    // offset: byte offset from start of audio data (not including header)
    // Returns actual bytes read, or -1 on error
    int32_t readWavChunk(const char* path, uint8_t* buffer, size_t offset, size_t length);

private:
    Stream* _debugStream;
    bool _isInitialized;
};

#endif // MEMORY_H