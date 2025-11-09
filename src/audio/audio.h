#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino.h>
#include "ESP_I2S.h"
#include "esp_log.h"
#include "esp_err.h"

// Forward declarations
class Memory;

// Audio configuration constants
#define AUDIO_SAMPLE_RATE 16000
#define AUDIO_VOICE_VOLUME 80
#define AUDIO_MIC_GAIN 3
#define AUDIO_CHUNK_SIZE 2048
#define AUDIO_TEST_TONE_FREQ 440  // Hz (A4 note)
#define AUDIO_TEST_TONE_DURATION 100  // ms

/**
 * @brief Audio management class for ESP32-S3 smartwatch
 * 
 * This class handles all audio-related operations including:
 * - I2S initialization and configuration
 * - ES8311 codec initialization
 * - WAV file playback from SD card
 * - Default audio (PCM) playback
 * - Test tone generation
 */
class Audio {
private:
    I2SClass* _i2s;
    Memory* _memory;
    bool _isInitialized;
    bool _codecInitialized;
    uint8_t _volume;
    
    static const char* TAG;
    
    // Private helper methods
    esp_err_t initCodec();
    bool convertMonoToStereo(const int16_t* monoBuffer, int16_t* stereoBuffer, int numSamples);

public:
    /**
     * @brief Constructor
     * @param i2s Pointer to I2S instance
     * @param memory Pointer to Memory instance for SD card access
     */
    Audio(I2SClass* i2s, Memory* memory);
    
    /**
     * @brief Destructor
     */
    ~Audio();
    
    /**
     * @brief Initialize audio system (I2S + ES8311 codec)
     * @param bclkPin I2S bit clock pin
     * @param wsPin I2S word select pin
     * @param doPin I2S data out pin
     * @param diPin I2S data in pin
     * @param mclkPin I2S master clock pin
     * @param paPin Power amplifier enable pin
     * @return true if initialization successful
     */
    bool begin(int bclkPin, int wsPin, int doPin, int diPin, int mclkPin, int paPin);
    
    /**
     * @brief Play WAV file from SD card with mono-to-stereo conversion
     * @param wavFile Path to WAV file on SD card
     * @param continueFlag Pointer to boolean flag to control playback (set to false to stop)
     * @param isTestMode True if this is a test playback (for logging purposes)
     * @return true if playback completed successfully
     */
    bool playWavFile(const char* wavFile, bool* continueFlag, bool isTestMode = false);
    
    /**
     * @brief Play default embedded PCM audio
     * @param pcmData Pointer to PCM data array
     * @param pcmLength Length of PCM data in bytes
     * @param continueFlag Pointer to boolean flag to control playback
     */
    void playDefaultAudio(const uint8_t* pcmData, size_t pcmLength, bool* continueFlag);
    
    /**
     * @brief Set audio volume
     * @param volume Volume level (0-100)
     * @return true if successful
     */
    bool setVolume(uint8_t volume);
    
    /**
     * @brief Get current volume
     * @return Current volume level (0-100)
     */
    uint8_t getVolume() const { return _volume; }
    
    /**
     * @brief Check if audio system is initialized
     * @return true if initialized
     */
    bool isInitialized() const { return _isInitialized; }
    
    /**
     * @brief Check if codec is initialized
     * @return true if codec initialized
     */
    bool isCodecReady() const { return _codecInitialized; }
    
    /**
     * @brief Enable/disable power amplifier
     * @param enable true to enable, false to disable
     */
    void setPowerAmplifier(bool enable);

    bool generateTestTone();
};

#endif // AUDIO_H
