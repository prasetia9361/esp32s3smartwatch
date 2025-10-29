#include "pin_config.h"
#include <driver/i2s.h>

#define SAMPLE_RATE 48000 
#define BYTE_RATE 128
#define I2S_SPEAKER_SD_PIN -1

i2s_pin_config_t i2sPins = {
    .bck_io_num = BCLKPIN,
    .ws_io_num = WSPIN,
    .data_out_num = DOPIN,
    .data_in_num = I2S_SPEAKER_SD_PIN // Not used
};
i2s_port_t i2sPort = I2S_NUM_0;

i2s_config_t i2s_sirine_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 1)
    .sample_rate = SAMPLE_RATE,
#else
    .sample_rate = (int)sample_rate,
#endif
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
#else
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S),
#endif
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 2,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 1)
    .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT
#endif
  };

i2s_config_t i2s_speaker_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 1)
        .sample_rate = SAMPLE_RATE,
#else
        .sample_rate = (int)sample_rate,
#endif
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 2, 0)
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
#else
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S),
#endif
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 2,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 1)
        .bits_per_chan = I2S_BITS_PER_CHAN_DEFAULT
#endif
    };

int16_t *samples = nullptr;