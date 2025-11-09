# Perbaikan UI yang Stuck/Tidak Responsif

## Update: Critical Memory Fix (Alarm Stuck Issue)

**PENTING:** Jika UI masih stuck setelah perbaikan timing, ini karena **LVGL memory terlalu kecil**!

### Masalah Baru yang Ditemukan
- UI stuck saat setting `timeinfoArray` dan tekan button alarm
- Root cause: **LVGL memory hanya 48KB** (terlalu kecil!)
- Alarm screen menggunakan banyak array → memory habis → freeze

### Solusi Memory (CRITICAL)
Lihat file: **`MEMORY_FIX_ALARM_STUCK.md`** untuk detail lengkap.

**Quick Fix:**
1. Switch LVGL ke PSRAM: `LV_MEM_CUSTOM = 1`
2. Expand partition: app 12MB → 13MB
3. Add watchdog timer untuk auto-recovery
4. Add memory monitoring untuk debug

---

## Masalah Awal (Timing Issues)
UI smartwatch terkadang stuck dan tidak merespon, meskipun berjalan normal di EEZ Studio simulator.

## Penyebab Masalah

### 1. **Task Scheduling Issues**
- `vTaskDelay(5)` terlalu kecil di loop() utama - hanya ~5ms
- Loop() berjalan terlalu cepat (~200x per detik) menyebabkan CPU starvation
- Semua background tasks memiliki priority sama (1), bersaing dengan UI

### 2. **Memory Constraints**
- LVGL memory hanya 48KB - terlalu kecil untuk UI kompleks
- Bisa menyebabkan memory fragmentation

### 3. **Refresh Rate Terlalu Tinggi**
- Display refresh 10ms (100 FPS) terlalu cepat untuk ESP32-S3
- Input polling 10ms juga terlalu agresif
- LVGL tick 2ms menghabiskan CPU cycles

## Perubahan yang Dilakukan

### 1. Loop() Timing (`src/main.cpp`)
```cpp
// SEBELUM:
vTaskDelay(5);  // ~200 updates/detik

// SESUDAH:
vTaskDelay(pdMS_TO_TICKS(10));  // ~100 updates/detik
```
**Manfaat**: Memberikan waktu lebih untuk task lain, tetap responsif

### 2. Task Priority (`src/main.cpp`)
```cpp
// SEBELUM: Semua priority = 1
WiFiTask    priority: 1
sensorTask  priority: 1
alarmTask   priority: 1

// SESUDAH:
WiFiTask    priority: 2  (lower)
sensorTask  priority: 2  (lower)
alarmTask   priority: 3  (lowest)
```
**Manfaat**: UI/loop() (priority 1) mendapat prioritas lebih tinggi

### 3. WiFi Task Delay (`src/main.cpp`)
```cpp
// SEBELUM:
vTaskDelay(10);  // 10ms

// SESUDAH:
vTaskDelay(pdMS_TO_TICKS(100));  // 100ms
```
**Manfaat**: WiFi tidak perlu dicek setiap 10ms, menghemat CPU

### 4. Sensor Task Delay (`src/main.cpp`)
```cpp
// SEBELUM:
vTaskDelay(5);  // 5ms

// SESUDAH:
vTaskDelay(pdMS_TO_TICKS(50));  // 50ms
```
**Manfaat**: Sensor reading 50ms sudah cukup cepat

### 5. LVGL Memory (`include/lv_conf.h`)
```cpp
// SEBELUM:
#define LV_MEM_SIZE (48U * 1024U)  // 48KB

// SESUDAH:
#define LV_MEM_SIZE (64U * 1024U)  // 64KB
```
**Manfaat**: Lebih banyak memory untuk UI kompleks, ESP32-S3 punya banyak RAM

### 6. Display Refresh Period (`include/lv_conf.h`)
```cpp
// SEBELUM:
#define LV_DISP_DEF_REFR_PERIOD 10  // 100 FPS

// SESUDAH:
#define LV_DISP_DEF_REFR_PERIOD 20  // 50 FPS
```
**Manfaat**: 50 FPS lebih dari cukup untuk UI, menghemat CPU

### 7. Input Read Period (`include/lv_conf.h`)
```cpp
// SEBELUM:
#define LV_INDEV_DEF_READ_PERIOD 10  // Poll setiap 10ms

// SESUDAH:
#define LV_INDEV_DEF_READ_PERIOD 30  // Poll setiap 30ms
```
**Manfaat**: Masih responsif tapi tidak waste CPU

### 8. LVGL Tick Period (`lib/lvgl_setup/lvgl_setup.cpp`)
```cpp
// SEBELUM:
#define EXAMPLE_LVGL_TICK_PERIOD_MS 2  // 500 ticks/detik

// SESUDAH:
#define EXAMPLE_LVGL_TICK_PERIOD_MS 5  // 200 ticks/detik
```
**Manfaat**: Lebih seimbang untuk ESP32-S3

## Hasil yang Diharapkan

✅ **UI lebih responsif dan smooth**
- Task priority yang benar memastikan UI tidak blocked
- Timing yang optimal mengurangi CPU overhead

✅ **Tidak ada stuck/freeze**
- Background tasks tidak "mencuri" CPU dari UI
- Memory LVGL yang cukup mencegah allocation failures

✅ **Battery life lebih baik**
- CPU tidak terlalu sibuk dengan polling yang tidak perlu
- Refresh rate yang wajar

✅ **Touch input tetap responsif**
- 30ms polling masih sangat cepat (< 1 frame delay)
- Interrupt-driven touch sudah menangani real-time detection

## Testing

Setelah upload firmware baru:

1. **Test UI Navigation**
   - Swipe antar halaman
   - Tap buttons
   - Harus smooth tanpa lag

2. **Test Background Tasks**
   - WiFi connection
   - Time update
   - Sensor readings
   - Semua harus tetap berfungsi

3. **Test Long-term Stability**
   - Biarkan running 30+ menit
   - Tidak boleh freeze/stuck
   - Memory tidak boleh leak

## Priority Map

```
Priority 0: Sistem critical (interrupt handlers)
Priority 1: loop() / UI (HIGHEST untuk user tasks)
Priority 2: WiFi & Sensor (background yang penting)
Priority 3: Alarm (hanya aktif saat dibutuhkan)
```

**Note**: Di FreeRTOS, angka priority lebih RENDAH = priority lebih TINGGI untuk execution!
(Berbeda dengan nilai priority di `xTaskCreatePinnedToCore` yang lebih tinggi = lebih penting)

## Build & Upload

```bash
pio run -t upload
```

Monitor serial untuk memastikan tidak ada error:
```bash
pio device monitor
```
