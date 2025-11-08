# 🌙 Panduan Brightness & Auto-Sleep System

## 📋 FITUR YANG DITAMBAHKAN

### ✅ **Brightness Management System**
Sistem untuk mengatur kecerahan layar dengan kontrol penuh.

### ✅ **Auto-Sleep Mode**
Layar otomatis mati (brightness 0) setelah 2 menit tanpa sentuhan, dan otomatis bangun saat ada sentuhan.

---

## 🎯 CARA KERJA

### **1. Normal Operation (Display Active)**
```
User Touch → Reset Timer
    ↓
Display Brightness: 200 (default)
    ↓
Timer counting: 0s → 120s
```

### **2. Sleep Mode (No Touch for 2 Minutes)**
```
Timer reaches 120s (2 minutes)
    ↓
Display Brightness: 0 (screen off)
    ↓
Status: displaySleeping = true
```

### **3. Wake Up (User Touch)**
```
User Touch detected
    ↓
resetSleepTimer() called
    ↓
Display Brightness: 200 (restored)
    ↓
Timer reset to 0
    ↓
Status: displaySleeping = false
```

---

## 📖 API FUNCTIONS

### **1. setDisplayBrightness(uint8_t brightness)**
Mengatur kecerahan layar secara manual.

```cpp
// Set brightness to 100 (range: 0-255)
setDisplayBrightness(100);

// Set to full brightness
setDisplayBrightness(255);

// Turn off display
setDisplayBrightness(0);
```

**Parameters:**
- `brightness`: 0-255 (0 = off, 255 = max)

---

### **2. getDisplayBrightness()**
Mendapatkan nilai kecerahan saat ini.

```cpp
uint8_t currentBrightness = getDisplayBrightness();
USBSerial.printf("Current brightness: %d\n", currentBrightness);
```

**Returns:** `uint8_t` - Brightness level (0-255)

---

### **3. setDefaultBrightness(uint8_t brightness)**
Mengatur brightness default yang akan dikembalikan setelah wake up dari sleep.

```cpp
// Set default brightness to 150 instead of 200
setDefaultBrightness(150);
```

**Parameters:**
- `brightness`: 0-255 (brightness yang diinginkan saat wake up)

**Note:** Default saat ini adalah 200.

---

### **4. handleAutoSleep()**
Fungsi yang menangani auto-sleep logic. **HARUS dipanggil dari loop utama!**

```cpp
void applySensor(void *param) {
    while (true) {
        lvgl_handler();
        ui_tick();
        
        handleAutoSleep();  // ✅ Sudah ditambahkan di main.cpp
        
        // ... kode lainnya
    }
}
```

**Behavior:**
- Cek waktu sejak interaksi terakhir
- Jika >= 2 menit → Set brightness ke 0
- Automatic, tidak perlu dipanggil manual

---

### **5. resetSleepTimer()**
Reset timer sleep ke 0. **Otomatis dipanggil saat ada sentuhan!**

```cpp
// Contoh: Reset timer saat button ditekan
if (buttonPressed) {
    resetSleepTimer();  // Reset counter
}
```

**Behavior:**
- Reset `lastInteractionTime` ke `millis()`
- Jika layar sedang sleep → Wake up dan restore brightness
- Otomatis dipanggil di `my_touchpad_read()` saat ada touch

---

### **6. setSleepTimeout(uint32_t timeoutMs)**
Mengatur timeout sleep dalam milidetik.

```cpp
// Set timeout to 1 minute (60,000 ms)
setSleepTimeout(60000);

// Set timeout to 5 minutes
setSleepTimeout(300000);

// Set timeout to 30 seconds
setSleepTimeout(30000);
```

**Parameters:**
- `timeoutMs`: Timeout dalam milidetik

**Default:** 120000 ms (2 menit)

---

### **7. isDisplaySleeping()**
Cek apakah layar sedang dalam mode sleep.

```cpp
if (isDisplaySleeping()) {
    USBSerial.println("Display is sleeping");
} else {
    USBSerial.println("Display is active");
}
```

**Returns:** `bool`
- `true` = Display sleeping (brightness 0)
- `false` = Display active

---

## 🔧 CONTOH PENGGUNAAN LANJUTAN

### **Example 1: Adjust Brightness Based on Battery**
```cpp
void adjustBrightnessBasedOnBattery() {
    int batteryPercent = battery->getBatteryPercentage();
    
    if (batteryPercent < 20) {
        setDefaultBrightness(100);  // Low battery = dim
        ESP_LOGI("POWER", "Low battery - brightness reduced to 100");
    } else if (batteryPercent < 50) {
        setDefaultBrightness(150);  // Medium
    } else {
        setDefaultBrightness(200);  // Full
    }
}

// Call di applySensor loop:
static uint32_t lastBrightnessCheck = 0;
if (millis() - lastBrightnessCheck > 30000) {  // Check every 30s
    adjustBrightnessBasedOnBattery();
    lastBrightnessCheck = millis();
}
```

### **Example 2: Night Mode (Auto Dim at Night)**
```cpp
void applyNightMode() {
    struct tm timeinfo;
    if (!ntpSetup.getLocalTime(&timeinfo)) return;
    
    int hour = timeinfo.tm_hour;
    
    // Night mode: 22:00 - 06:00
    if (hour >= 22 || hour < 6) {
        setDefaultBrightness(80);   // Very dim at night
        setSleepTimeout(60000);     // Sleep after 1 minute
        ESP_LOGI("UI", "Night mode activated");
    } else {
        setDefaultBrightness(200);  // Normal during day
        setSleepTimeout(120000);    // Sleep after 2 minutes
    }
}

// Call once per hour:
static int lastHour = -1;
if (timeinfo.tm_hour != lastHour) {
    applyNightMode();
    lastHour = timeinfo.tm_hour;
}
```

### **Example 3: Prevent Sleep During Alarm**
```cpp
void controlAlarmTask(void *param) {
    while (true) {
        if (isAlarmActive) {
            // Prevent sleep during alarm
            resetSleepTimer();  // Keep display awake
            
            // Play alarm...
        }
        
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
```

### **Example 4: Manual Wake-Up via Button**
```cpp
// Jika ada physical button untuk wake up
void checkWakeButton() {
    if (digitalRead(WAKE_BUTTON_PIN) == LOW) {
        if (isDisplaySleeping()) {
            resetSleepTimer();  // Wake up display
            ESP_LOGI("UI", "Manual wake-up triggered");
        }
    }
}
```

### **Example 5: Gradual Brightness Fade**
```cpp
void fadeInBrightness(uint8_t targetBrightness, uint32_t durationMs) {
    uint8_t startBrightness = getDisplayBrightness();
    uint32_t startTime = millis();
    
    while (millis() - startTime < durationMs) {
        float progress = (float)(millis() - startTime) / durationMs;
        uint8_t currentLevel = startBrightness + 
                              (targetBrightness - startBrightness) * progress;
        setDisplayBrightness(currentLevel);
        vTaskDelay(pdMS_TO_TICKS(20));  // 50Hz update
    }
    
    setDisplayBrightness(targetBrightness);
}

// Usage:
fadeInBrightness(200, 1000);  // Fade to 200 over 1 second
```

---

## ⚙️ KONFIGURASI DEFAULT

```cpp
// Di lvgl_setup.cpp
static uint8_t currentBrightness = 200;    // Brightness saat ini
static uint8_t defaultBrightness = 200;    // Brightness default (wake up)
static uint32_t sleepTimeout = 120000;     // 2 menit (120,000 ms)
static bool displaySleeping = false;        // Status sleep
```

### **Mengubah Default:**

#### **Option 1: Hardcode di lvgl_setup.cpp**
```cpp
// Change line 7-9 di lvgl_setup.cpp:
static uint8_t defaultBrightness = 150;    // 150 instead of 200
static uint32_t sleepTimeout = 60000;      // 1 menit instead of 2
```

#### **Option 2: Runtime Configuration**
```cpp
void setup() {
    // ... existing setup code
    
    lvgl_init();
    ui_init();
    
    // Configure brightness & sleep
    setDefaultBrightness(150);      // Custom default
    setSleepTimeout(60000);         // 1 minute timeout
    
    ESP_LOGI(TAG, "Custom brightness settings applied");
}
```

---

## 🔍 TROUBLESHOOTING

### **Problem 1: Display tidak sleep setelah 2 menit**

**Kemungkinan Penyebab:**
1. `handleAutoSleep()` tidak dipanggil di loop
2. Ada code yang terus memanggil `resetSleepTimer()`

**Solusi:**
```cpp
// Cek di applySensor loop:
while (true) {
    handleAutoSleep();  // ✅ Make sure ini ada
    // ...
}

// Debug logging:
if (millis() % 10000 == 0) {  // Every 10s
    ESP_LOGI("SLEEP", "Time since interaction: %lu ms", 
             millis() - lastInteractionTime);
}
```

---

### **Problem 2: Display tidak wake up saat touch**

**Kemungkinan Penyebab:**
1. Touch interrupt tidak bekerja
2. `resetSleepTimer()` tidak dipanggil di `my_touchpad_read()`

**Solusi:**
```cpp
// Cek di my_touchpad_read():
if (touch_points > 0) {
    // ...
    resetSleepTimer();  // ✅ Make sure ini ada
}

// Test manual wake-up:
void loop() {
    if (Serial.available()) {
        resetSleepTimer();
        ESP_LOGI("DEBUG", "Manual wake-up triggered");
    }
}
```

---

### **Problem 3: Brightness berubah-ubah**

**Kemungkinan Penyebab:**
1. Ada multiple code yang set brightness
2. Conflict dengan auto-brightness

**Solusi:**
```cpp
// Centralize brightness control
void updateBrightness(uint8_t newBrightness, const char* reason) {
    ESP_LOGI("BRIGHTNESS", "Setting to %d (Reason: %s)", 
             newBrightness, reason);
    setDisplayBrightness(newBrightness);
}

// Usage:
updateBrightness(100, "Battery low");
updateBrightness(200, "Normal mode");
```

---

## 📊 MONITORING & DEBUGGING

### **Add Debug Logging:**
```cpp
void applySensor(void *param) {
    static uint32_t lastDebugLog = 0;
    
    while (true) {
        handleAutoSleep();
        
        // Debug log every 30 seconds
        if (millis() - lastDebugLog > 30000) {
            uint32_t timeSinceTouch = millis() - lastInteractionTime;
            ESP_LOGI("SLEEP_DEBUG", 
                     "Status: %s | Brightness: %d | Time since touch: %lu ms | Timeout: %lu ms",
                     isDisplaySleeping() ? "SLEEPING" : "ACTIVE",
                     getDisplayBrightness(),
                     timeSinceTouch,
                     sleepTimeout);
            lastDebugLog = millis();
        }
        
        vTaskDelay(5);
    }
}
```

### **Expected Output:**
```
I (30000) SLEEP_DEBUG: Status: ACTIVE | Brightness: 200 | Time since touch: 15234 ms | Timeout: 120000 ms
I (60000) SLEEP_DEBUG: Status: ACTIVE | Brightness: 200 | Time since touch: 45234 ms | Timeout: 120000 ms
I (90000) SLEEP_DEBUG: Status: ACTIVE | Brightness: 200 | Time since touch: 75234 ms | Timeout: 120000 ms
I (120000) SLEEP_DEBUG: Status: SLEEPING | Brightness: 0 | Time since touch: 120034 ms | Timeout: 120000 ms
[Touch detected]
I (125000) SLEEP_DEBUG: Status: ACTIVE | Brightness: 200 | Time since touch: 234 ms | Timeout: 120000 ms
```

---

## 🔋 POWER SAVINGS ESTIMATE

### **Before (No Auto-Sleep):**
```
Display always ON @ 200 brightness
├─ Display power: ~150mA
├─ Total power: ~250mA
└─ Battery life: ~12-16 hours
```

### **After (With Auto-Sleep):**
```
Active usage: 6 hours/day @ 200 brightness
Sleep mode: 18 hours/day @ 0 brightness

Power consumption:
├─ Active (6h): 250mA × 6h = 1500mAh
├─ Sleep (18h): 100mA × 18h = 1800mAh
└─ Total: 3300mAh/day

Estimated battery life: ~24-30 hours
Improvement: +50-87% battery life! 🔋
```

---

## 🎯 BEST PRACTICES

### ✅ **DO:**
1. Call `handleAutoSleep()` di main loop (sudah ditambahkan)
2. Adjust timeout based on use case (meeting vs daily use)
3. Use lower brightness for better battery life
4. Test thoroughly dengan different scenarios

### ❌ **DON'T:**
1. Call `setDisplayBrightness()` terlalu sering (can flicker)
2. Set timeout terlalu pendek (< 30s annoying)
3. Forget to call `resetSleepTimer()` saat ada user interaction
4. Override brightness tanpa reason (hard to debug)

---

## 📝 CHANGELOG

### **Version 1.0 - Initial Release**
- ✅ Basic brightness control
- ✅ Auto-sleep after 2 minutes
- ✅ Auto wake-up on touch
- ✅ Configurable timeout
- ✅ Status monitoring functions

---

## 🚀 FUTURE ENHANCEMENTS (Optional)

### **Planned Features:**
1. **Ambient Light Sensor Integration**
   ```cpp
   void autoAdjustBrightness() {
       int lightLevel = readAmbientLight();
       uint8_t targetBrightness = map(lightLevel, 0, 1023, 50, 255);
       setDisplayBrightness(targetBrightness);
   }
   ```

2. **Gesture Wake-Up**
   ```cpp
   void checkGestureWakeup() {
       if (detectWristRotation()) {
           resetSleepTimer();
       }
   }
   ```

3. **Configurable Brightness Profiles**
   ```cpp
   struct BrightnessProfile {
       uint8_t indoor;
       uint8_t outdoor;
       uint8_t night;
       uint32_t timeout;
   };
   ```

4. **Progressive Dimming**
   ```cpp
   // Instead of instant sleep, dim gradually
   // 90s: 200 → 150
   // 105s: 150 → 100
   // 120s: 100 → 0
   ```

---

## 📞 SUPPORT & TESTING

### **Test Checklist:**
- [ ] Display sleeps after 2 minutes no touch
- [ ] Display wakes on touch
- [ ] Brightness restores to default (200)
- [ ] Timer resets properly after wake
- [ ] No flicker during normal use
- [ ] Battery life improved
- [ ] Serial logs show correct status

### **Test Script:**
```cpp
void runSleepTest() {
    ESP_LOGI("TEST", "=== Sleep Test Started ===");
    
    ESP_LOGI("TEST", "1. Setting timeout to 10 seconds for testing");
    setSleepTimeout(10000);
    
    ESP_LOGI("TEST", "2. Waiting 10 seconds...");
    vTaskDelay(pdMS_TO_TICKS(10000));
    
    if (isDisplaySleeping()) {
        ESP_LOGI("TEST", "✅ Display entered sleep mode");
    } else {
        ESP_LOGE("TEST", "❌ Display did NOT sleep!");
    }
    
    ESP_LOGI("TEST", "3. Simulating touch...");
    resetSleepTimer();
    
    if (!isDisplaySleeping() && getDisplayBrightness() == 200) {
        ESP_LOGI("TEST", "✅ Display woke up successfully");
    } else {
        ESP_LOGE("TEST", "❌ Wake-up failed!");
    }
    
    ESP_LOGI("TEST", "4. Restoring timeout to 2 minutes");
    setSleepTimeout(120000);
    
    ESP_LOGI("TEST", "=== Sleep Test Complete ===");
}
```

---

**Selamat! Auto-sleep system sudah siap digunakan! 🎉**

Silakan upload firmware dan test functionality-nya. Display akan otomatis mati setelah 2 menit, dan bangun kembali saat ada sentuhan! 💤✨
