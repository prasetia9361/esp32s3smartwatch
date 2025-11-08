#ifndef LVGL_SETUP_H
#define LVGL_SETUP_H

#include <lvgl.h>
#include "Arduino_GFX_Library.h"
#include "Arduino_DriveBus_Library.h"
#include "pin_config.h"
#include "HWCDC.h"

// Deklarasi eksternal untuk objek Serial USB, digunakan untuk logging.
extern HWCDC USBSerial;

// Fungsi untuk menginisialisasi LVGL, driver layar, dan driver input.
void lvgl_init();
// Fungsi handler LVGL yang harus dipanggil secara berkala di loop utama.
void lvgl_handler();

// Fungsi untuk mengatur kecerahan layar (0-255)
void setDisplayBrightness(uint8_t brightness);

// Fungsi untuk mendapatkan kecerahan saat ini
uint8_t getDisplayBrightness();

// Fungsi untuk mengatur kecerahan default (yang akan dikembalikan setelah wake up)
void setDefaultBrightness(uint8_t brightness);

// Fungsi untuk menangani auto-sleep (dipanggil dari loop utama)
void handleAutoSleep();

// Fungsi untuk reset timer sleep (dipanggil saat ada interaksi user)
void resetSleepTimer();

// Fungsi untuk mengatur timeout sleep dalam milidetik (default: 120000 = 2 menit)
void setSleepTimeout(uint32_t timeoutMs);

// Fungsi untuk mendapatkan status apakah layar sedang sleep
bool isDisplaySleeping();

#endif