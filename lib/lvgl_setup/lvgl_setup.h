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

#endif