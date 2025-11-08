#include "audio.h"
#include "../memory/memory.h"
#include "es8311.h"
#include "pin_config.h"
#include <Wire.h>
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

const char* Audio::TAG = "Audio";

// External I2C mutex (defined in main.cpp)
extern SemaphoreHandle_t i2c_mutex;

Audio::Audio(I2SClass* i2s, Memory* memory) 
    : _i2s(i2s), 
      _memory(memory), 
      _isInitialized(false),
      _codecInitialized(false),
      _volume(AUDIO_VOICE_VOLUME) {
}

Audio::~Audio() {
    // Cleanup if needed
    setPowerAmplifier(false);
}

esp_err_t Audio::initCodec() {
    ESP_LOGI(TAG, "Initializing ES8311 codec...");
    
    // Check I2C device presence
    if (i2c_mutex != NULL && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        Wire.beginTransmission(0x18);  // ES8311_ADDRRES_0
        byte error = Wire.endTransmission();
        xSemaphoreGive(i2c_mutex);
        
        if (error != 0) {
            ESP_LOGE(TAG, "ES8311 not found at I2C address 0x18");
            return ESP_FAIL;
        }
    } else {
        ESP_LOGE(TAG, "Failed to acquire I2C mutex");
        return ESP_FAIL;
    }
    
    // Create ES8311 handle
    es8311_handle_t es_handle = es8311_create(0, ES8311_ADDRRES_0);
    if (!es_handle) {
        ESP_LOGE(TAG, "ES8311 create failed");
        return ESP_FAIL;
    }
    
    // Configure clock
    const es8311_clock_config_t es_clk = {
        .mclk_inverted = false,
        .sclk_inverted = false,
        .mclk_from_mclk_pin = true,
        .mclk_frequency = AUDIO_SAMPLE_RATE * 256,
        .sample_frequency = AUDIO_SAMPLE_RATE
    };

    // Initialize codec
    esp_err_t err = es8311_init(es_handle, &es_clk, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ES8311 init failed: %s", esp_err_to_name(err));
        return err;
    }
    
    // Configure sample frequency
    err = es8311_sample_frequency_config(es_handle, es_clk.mclk_frequency, es_clk.sample_frequency);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Sample frequency config failed: %s", esp_err_to_name(err));
        return err;
    }
    
    // Configure microphone (disable for playback only)
    err = es8311_microphone_config(es_handle, false);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Microphone config failed: %s", esp_err_to_name(err));
        return err;
    }
    
    // Set volume
    err = es8311_voice_volume_set(es_handle, _volume, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Volume set failed: %s", esp_err_to_name(err));
        return err;
    }
    
    // Set microphone gain
    err = es8311_microphone_gain_set(es_handle, (es8311_mic_gain_t)AUDIO_MIC_GAIN);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Microphone gain set failed: %s", esp_err_to_name(err));
        return err;
    }
    
    ESP_LOGI(TAG, "ES8311 codec initialized successfully (Volume: %d)", _volume);
    _codecInitialized = true;
    return ESP_OK;
}

bool Audio::generateTestTone() {
    const int testDuration = AUDIO_SAMPLE_RATE / 10;  // 100ms
    int16_t* testTone = (int16_t*)malloc(testDuration * 2 * sizeof(int16_t));
    
    if (!testTone) {
        ESP_LOGE(TAG, "Failed to allocate test tone buffer");
        return false;
    }
    
    // Generate 440Hz sine wave (A4 note)
    for (int i = 0; i < testDuration; i++) {
        float sample = sin(2.0 * PI * AUDIO_TEST_TONE_FREQ * i / AUDIO_SAMPLE_RATE) * 10000;
        testTone[i * 2] = testTone[i * 2 + 1] = (int16_t)sample;
    }
    
    _i2s->write((uint8_t*)testTone, testDuration * 2 * sizeof(int16_t));
    free(testTone);
    
    ESP_LOGI(TAG, "Test tone played (440Hz, 100ms)");
    return true;
}

bool Audio::convertMonoToStereo(const int16_t* monoBuffer, int16_t* stereoBuffer, int numSamples) {
    if (!monoBuffer || !stereoBuffer) {
        return false;
    }
    
    for (int i = 0; i < numSamples; i++) {
        int16_t sample = monoBuffer[i];
        stereoBuffer[i * 2] = sample;      // Left channel
        stereoBuffer[i * 2 + 1] = sample;  // Right channel
    }
    
    return true;
}

bool Audio::begin(int bclkPin, int wsPin, int doPin, int diPin, int mclkPin, int paPin) {
    ESP_LOGI(TAG, "Initializing audio system...");
    
    // Enable power amplifier
    pinMode(paPin, OUTPUT);
    digitalWrite(paPin, HIGH);
    ESP_LOGI(TAG, "Power amplifier enabled");
    
    // Configure I2S pins
    _i2s->setPins(bclkPin, wsPin, doPin, diPin, mclkPin);
    
    // Initialize I2S
    if (!_i2s->begin(I2S_MODE_STD, AUDIO_SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, 
                     I2S_SLOT_MODE_STEREO, I2S_STD_SLOT_BOTH)) {
        ESP_LOGE(TAG, "I2S initialization failed");
        return false;
    }
    
    ESP_LOGI(TAG, "I2S initialized (Sample rate: %d Hz)", AUDIO_SAMPLE_RATE);
    _isInitialized = true;
    
    // Initialize codec
    esp_err_t codec_result = initCodec();
    if (codec_result != ESP_OK) {
        ESP_LOGW(TAG, "Codec init failed, continuing anyway");
        _codecInitialized = false;
    }
    
    // Play test tone if codec is ready
    // if (_codecInitialized) {
    //     vTaskDelay(pdMS_TO_TICKS(200));
    //     generateTestTone();
    //     vTaskDelay(pdMS_TO_TICKS(200));
    // }
    
    ESP_LOGI(TAG, "Audio system initialized successfully");
    return true;
}

bool Audio::playWavFile(const char* wavFile, bool* continueFlag, bool isTestMode) {
    if (!_isInitialized) {
        ESP_LOGE(TAG, "Audio system not initialized");
        return false;
    }
    
    if (!_memory) {
        ESP_LOGE(TAG, "Memory instance not set");
        return false;
    }
    
    if (!wavFile || !continueFlag) {
        ESP_LOGE(TAG, "Invalid parameters");
        return false;
    }
    
    // Get WAV file data size
    int32_t wavDataSize = _memory->getWavDataSize(wavFile);
    if (wavDataSize <= 0) {
        ESP_LOGE(TAG, "Invalid WAV file: %s", wavFile);
        return false;
    }
    
    ESP_LOGI(TAG, "Playing %s: %s (%d bytes)", 
             isTestMode ? "test file" : "WAV file", wavFile, wavDataSize);
    
    // Allocate buffers
    uint8_t* audioBuffer = (uint8_t*)malloc(AUDIO_CHUNK_SIZE);
    int16_t* stereoBuffer = (int16_t*)malloc(AUDIO_CHUNK_SIZE * 2);
    
    if (!audioBuffer || !stereoBuffer) {
        ESP_LOGE(TAG, "Memory allocation failed");
        if (audioBuffer) free(audioBuffer);
        if (stereoBuffer) free(stereoBuffer);
        return false;
    }
    
    size_t offset = 0;
    size_t totalWritten = 0;
    bool success = true;
    
    // Stream WAV file
    while (*continueFlag && offset < (size_t)wavDataSize) {
        size_t bytesToRead = min((size_t)(wavDataSize - offset), (size_t)AUDIO_CHUNK_SIZE);
        int32_t bytesRead = _memory->readWavChunk(wavFile, audioBuffer, offset, bytesToRead);
        
        if (bytesRead > 0) {
            // Convert mono to stereo
            int numSamples = bytesRead / 2;
            int16_t* monoBuffer = (int16_t*)audioBuffer;
            
            if (!convertMonoToStereo(monoBuffer, stereoBuffer, numSamples)) {
                ESP_LOGE(TAG, "Mono to stereo conversion failed");
                success = false;
                break;
            }
            
            // Write to I2S
            size_t bytesWritten = _i2s->write((uint8_t*)stereoBuffer, numSamples * 2 * sizeof(int16_t));
            
            if (bytesWritten > 0) {
                offset += bytesRead;
                totalWritten += bytesWritten;
            } else {
                ESP_LOGW(TAG, "I2S write stalled, retrying...");
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        } else {
            ESP_LOGW(TAG, "Read error at offset %d", offset);
            success = false;
            break;
        }
        
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    
    // Cleanup
    free(audioBuffer);
    free(stereoBuffer);
    
    if (success && *continueFlag) {
        ESP_LOGI(TAG, "Playback complete (%d bytes written)", totalWritten);
    } else if (!*continueFlag) {
        ESP_LOGI(TAG, "Playback stopped by user (%d bytes written)", totalWritten);
    }
    
    return success;
}

void Audio::playDefaultAudio(const uint8_t* pcmData, size_t pcmLength, bool* continueFlag) {
    if (!_isInitialized) {
        ESP_LOGE(TAG, "Audio system not initialized");
        return;
    }
    
    if (!pcmData || !continueFlag) {
        ESP_LOGE(TAG, "Invalid parameters");
        return;
    }
    
    ESP_LOGI(TAG, "Playing default audio (%d bytes)", pcmLength);
    
    size_t offset = 0;
    size_t totalWritten = 0;
    
    while (*continueFlag && offset < pcmLength) {
        size_t bytesToWrite = min((size_t)(pcmLength - offset), (size_t)AUDIO_CHUNK_SIZE);
        size_t bytesWritten = _i2s->write((uint8_t*)pcmData + offset, bytesToWrite);
        
        if (bytesWritten > 0) {
            offset += bytesWritten;
            totalWritten += bytesWritten;
        } else {
            ESP_LOGW(TAG, "I2S write stalled, retrying...");
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    
    if (*continueFlag) {
        ESP_LOGI(TAG, "Default audio playback complete (%d bytes)", totalWritten);
    } else {
        ESP_LOGI(TAG, "Default audio stopped by user (%d bytes)", totalWritten);
    }
}

bool Audio::setVolume(uint8_t volume) {
    if (volume > 100) {
        ESP_LOGW(TAG, "Volume clamped to 100 (was %d)", volume);
        volume = 100;
    }
    
    _volume = volume;
    
    if (_codecInitialized) {
        // Note: This is a simplified version. In a complete implementation,
        // you would need to store the es8311_handle and call es8311_voice_volume_set
        ESP_LOGI(TAG, "Volume set to %d (codec update requires handle storage)", volume);
        return true;
    } else {
        ESP_LOGW(TAG, "Codec not initialized, volume will be applied on next init");
        return false;
    }
}

void Audio::setPowerAmplifier(bool enable) {
    // Note: PA pin should be stored in class members for proper implementation
    // For now, this is a placeholder
    ESP_LOGI(TAG, "Power amplifier %s", enable ? "enabled" : "disabled");
}
