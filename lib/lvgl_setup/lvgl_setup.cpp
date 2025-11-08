#include "lvgl_setup.h"
#include "lvgl_setup.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
// Periode tick LVGL dalam milidetik. LVGL menggunakan ini untuk menangani animasi dan tugas lainnya.
#define EXAMPLE_LVGL_TICK_PERIOD_MS 2

// Variabel untuk brightness management
static uint8_t currentBrightness = 200;    // Kecerahan saat ini
static uint8_t defaultBrightness = 200;    // Kecerahan default (akan dikembalikan saat wake up)
// Default timeout set to 120000 ms = 2 minutes (was incorrectly 12000)
static uint32_t sleepTimeout = 120000;     // Timeout sleep dalam ms (default: 2 menit)
static uint32_t lastInteractionTime = 0;   // Waktu terakhir ada interaksi
static bool displaySleeping = false;       // Status apakah layar sedang sleep

// Buffer untuk LVGL. LVGL akan merender ke buffer ini sebelum dikirim ke layar.
static lv_disp_draw_buf_t draw_buf;
// Ukuran buffer adalah 1/10 dari ukuran layar untuk menghemat RAM.
static lv_color_t buf[LCD_WIDTH * LCD_HEIGHT / 10];

// Inisialisasi bus data QSPI untuk komunikasi dengan layar.
// Pin yang digunakan didefinisikan dalam pin_config.h.
Arduino_DataBus *bus = new Arduino_ESP32QSPI(
  LCD_CS, LCD_SCLK, LCD_SDIO0, LCD_SDIO1,
  LCD_SDIO2, LCD_SDIO3);

// Inisialisasi driver grafis (GFX) untuk layar SH8601.
// Menggunakan bus QSPI yang telah dibuat.
Arduino_GFX *gfx = new Arduino_SH8601(bus, -1,
                                      0, false, LCD_WIDTH, LCD_HEIGHT);

// Inisialisasi bus I2C untuk komunikasi dengan kontroler sentuh.
std::shared_ptr<Arduino_IIC_DriveBus> IIC_Bus =
  std::make_shared<Arduino_HWIIC>(IIC_SDA, IIC_SCL, &Wire);

// Deklarasi forward untuk fungsi interrupt sentuh.
void Arduino_IIC_Touch_Interrupt(void);

// I2C mutex declared in main.cpp; use to protect all Wire/I2C access from tasks
extern SemaphoreHandle_t i2c_mutex;

// Inisialisasi driver untuk kontroler sentuh FT3168.
// Menggunakan bus I2C dan mendefinisikan pin interrupt serta fungsi handler-nya.
std::unique_ptr<Arduino_IIC> FT3168(new Arduino_FT3x68(IIC_Bus, FT3168_DEVICE_ADDRESS,
                                                       DRIVEBUS_DEFAULT_VALUE, TP_INT, Arduino_IIC_Touch_Interrupt));

// Fungsi Interrupt Service Routine (ISR) untuk sentuhan.
// Dipanggil ketika ada sentuhan pada layar, dan hanya mengatur sebuah flag.
void Arduino_IIC_Touch_Interrupt(void) {
  FT3168->IIC_Interrupt_Flag = true;
}

#if LV_USE_LOG != 0
// Fungsi kustom untuk mencetak log dari LVGL ke Serial USB.
// Aktif jika LV_USE_LOG diaktifkan di lv_conf.h.
void my_print(const char *buf) {
  USBSerial.printf(buf);
  USBSerial.flush();
}
#endif

// Callback untuk mengirim buffer gambar dari LVGL ke layar.
// Fungsi ini dipanggil oleh LVGL ketika sebuah area perlu digambar ulang.
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

// Menggambar bitmap ke layar menggunakan driver GFX.
#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif

  // Memberi tahu LVGL bahwa proses flush telah selesai.
  lv_disp_flush_ready(disp);
}

// Callback timer untuk menaikkan tick LVGL.
// LVGL memerlukan tick ini untuk manajemen waktu internal.
void example_increase_lvgl_tick(void *arg) {
  lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

// Callback untuk membaca status input sentuh.
// Fungsi ini dipanggil oleh LVGL untuk mendapatkan data dari perangkat input.
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  // To avoid spamming the I2C bus, only query the touch controller when
  // the interrupt flag was set by the ISR. This keeps I2C traffic low.
  if (!FT3168->IIC_Interrupt_Flag) {
    data->state = LV_INDEV_STATE_REL;
    return;
  }

  // Clear the interrupt flag here (we will service it now)
  FT3168->IIC_Interrupt_Flag = false;

  // Touch reading has HIGH PRIORITY - wait longer for mutex to ensure responsiveness
  // We DON'T use mutex here because FT3168 library will handle it internally
  uint8_t touch_points = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_FINGER_NUMBER);
  if (touch_points > 0) {
    int32_t touchX = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_X);
    int32_t touchY = FT3168->IIC_Read_Device_Value(FT3168->Arduino_IIC_Touch::Value_Information::TOUCH_COORDINATE_Y);

    data->state = LV_INDEV_STATE_PR; // PRESSED
    data->point.x = touchX;
    data->point.y = touchY;

    // Reset sleep timer saat ada sentuhan
    resetSleepTimer();
  } else {
    data->state = LV_INDEV_STATE_REL; // RELEASED
  }
}

// Fungsi inisialisasi utama untuk LVGL dan semua driver terkait.
void lvgl_init() {
  // Inisialisasi kontroler sentuh FT3168.
  // FT3168 uses I2C; protect initialization with the i2c_mutex if available.
  bool ft_ok = false;
  while (!ft_ok) {
    if (i2c_mutex != NULL) {
      if (xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        ft_ok = FT3168->begin();
        xSemaphoreGive(i2c_mutex);
      } else {
        // Can't get mutex; wait a bit and retry
        delay(100);
      }
    } else {
      // No mutex declared (fallback) -- try normally but this is not ideal
      ft_ok = FT3168->begin();
    }

    if (!ft_ok) {
      USBSerial.println("FT3168 initialization fail");
      delay(2000);
    }
  }
  USBSerial.println("FT3168 initialization successfully");

  // Inisialisasi driver layar GFX dan mengatur kecerahan.
  gfx->begin();
  gfx->Display_Brightness(200);

  // Inisialisasi pustaka LVGL.
  lv_init();

#if LV_USE_LOG != 0
  // Mendaftarkan fungsi print kustom untuk log LVGL.
  lv_log_register_print_cb(my_print);
#endif

  // Mengatur mode daya kontroler sentuh.
  FT3168->IIC_Write_Device_State(FT3168->Arduino_IIC_Touch::Device::TOUCH_POWER_MODE,
                                 FT3168->Arduino_IIC_Touch::Device_Mode::TOUCH_POWER_MONITOR);

  // Inisialisasi buffer gambar LVGL.
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, LCD_WIDTH * LCD_HEIGHT / 10);

  // Inisialisasi dan registrasi driver display untuk LVGL.
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_WIDTH;
  disp_drv.ver_res = LCD_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // Inisialisasi dan registrasi driver input (touchpad) untuk LVGL.
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  // Membuat dan memulai timer periodik ESP32 untuk menghasilkan tick LVGL.
  const esp_timer_create_args_t lvgl_tick_timer_args = {
    .callback = &example_increase_lvgl_tick,
    .name = "lvgl_tick"
  };

  esp_timer_handle_t lvgl_tick_timer = NULL;
  esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
  esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000);
  
  // Inisialisasi timer sleep
  lastInteractionTime = millis();
  displaySleeping = false;
}

// Fungsi handler LVGL.
// Harus dipanggil secara berulang di dalam loop utama program.
// Fungsi ini menangani tugas-tugas LVGL seperti rendering dan event handling.
void lvgl_handler() {
  lv_timer_handler();
}

// ==================== BRIGHTNESS & SLEEP MANAGEMENT ====================

// Fungsi untuk mengatur kecerahan layar
void setDisplayBrightness(uint8_t brightness) {
  if (brightness > 255) brightness = 255;
  currentBrightness = brightness;
  gfx->Display_Brightness(brightness);
}

// Fungsi untuk mendapatkan kecerahan saat ini
uint8_t getDisplayBrightness() {
  return currentBrightness;
}

// Fungsi untuk mengatur kecerahan default
void setDefaultBrightness(uint8_t brightness) {
  if (brightness > 255) brightness = 255;
  defaultBrightness = brightness;
}

// Fungsi untuk reset timer sleep (dipanggil saat ada interaksi user)
void resetSleepTimer() {
  lastInteractionTime = millis();
  
  // Jika layar sedang sleep, bangunkan
  if (displaySleeping) {
    displaySleeping = false;
    setDisplayBrightness(defaultBrightness);
    USBSerial.println("Display woke up - brightness restored");
  }
}

// Fungsi untuk mengatur timeout sleep
void setSleepTimeout(uint32_t timeoutMs) {
  sleepTimeout = timeoutMs;
}

// Fungsi untuk mendapatkan status sleep
bool isDisplaySleeping() {
  return displaySleeping;
}

// Fungsi untuk menangani auto-sleep (harus dipanggil dari loop utama)
void handleAutoSleep() {
  // Cek apakah sudah waktunya untuk sleep
  if (!displaySleeping && (millis() - lastInteractionTime >= sleepTimeout)) {
    displaySleeping = true;
    setDisplayBrightness(0);  // Matikan layar
    USBSerial.println("Display entering sleep mode - brightness set to 0");
  }
}