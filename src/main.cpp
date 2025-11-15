#include <math.h>
#include <Wire.h>
#include <cstring>
#include "esp_log.h"

#ifndef PI
#define PI 3.14159265358979323846
#endif

#include "config.h"
#include "lvgl_setup.h"

#include "wifiSetup/wifi_setup.h"
#include "SensorPCF85063.hpp"
#include "ntpSetup/ntp_setup.h"
#include "batteryMonitor/battery_monitor.h"
#include "SensorQMI8658.hpp"
#include "stepCounter/step_counter.h"
#include "storage/storage.h"
#include "memory/memory.h"
#include "clientServer/clientServer.h"
#include "audio/audio.h"
#include "ui.h"
#include "vars.h"
#include "screens.h"

// Include ui.h sudah punya loadScreen declaration di dalam extern "C"

#include "pin_config.h"
#include "ESP_I2S.h"
I2SClass i2s;

#include "canon.h"

static const char *TAG = "Smartwatch";
static const char *TAG_STORAGE = "Storage";
static const char *TAG_SENSOR = "Sensor";

SemaphoreHandle_t i2c_mutex = NULL;


HWCDC USBSerial;
WifiSetup *wifiSetup;
NTPSetup ntpSetup(NTP_TIMEZONE, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
Memory *memory;
clientServer *webServer;
Audio *audioSystem;
SensorPCF85063 rtc;

XPowersPMU power;
BatteryMonitor *battery;

SensorQMI8658 sensorStepper;
StepCounter *stepCounter;


void applyWiFiMode(void *param);
void applySensor(void *param);
void controlAlarmTask(void *param);

// Declare objects dari screens.h untuk akses screen
extern "C" {
  extern objects_t objects;
}


extern bool switchWiFiAP();
extern bool switchWiFi();
extern bool saveDate();
extern void chargeState(bool isCharge);
extern void setTimeHHMM(const char* hhmm);
extern void setCurrentDate(const char* datetime);
extern void setBT(int32_t persent);
extern void setStep(int32_t step);
extern void setDateTime(uint16_t* dt, size_t len);
extern uint16_t *getDateTime();
extern int32_t getBatteryPersentage();
extern int isStep();
extern bool getSaveAlarm();
extern void setSaveAlarm(bool data);
extern bool gethidenAlaram();
extern const char *getStartTime();
extern const char *getStartTime2();
extern const char *getStartTime3();
extern const char *getEndTime();
extern const char *getEndTime2();
extern const char *getEndTime3();
extern void timeAlarm(int32_t *data, size_t len );
extern void setScheduleTime(const char *startTime, const char *endTime);
extern void fileAudio(const char **data, size_t len);
extern bool playAlarm();
extern bool playAlarm2();
extern bool playAlarm3();
extern int playAudio();
extern const char *getFileAudioSelected();
extern bool getSetAlarm();
extern void setPlayAlarm(bool data);
extern void setPlayAlarm2(bool data);
extern void setPlayAlarm3(bool data);
extern const char *getNama();
extern int32_t getUsia();
extern int32_t getBB();
extern bool getCalculate();
extern void setCalculate(bool data);
extern void sethasilHitung(const char* hasil);
extern bool getClosePopup();
extern void setClosePopup(bool data);
extern int32_t getIndexAlarm();
extern int32_t getCairan();
extern void setCairan(int data);
extern void setCairanTotal(int data);
extern int32_t getCairanTotal();

int lastDay = -1;
int currentDay = -1;
int ntpLastDay = -1;
int ntpCurrentDay = -1;
char rtcDateStr[18];
bool isNtp = false;
bool isLastNtp = false;
unsigned long lastTimeUpdate = 0;
const unsigned long TIME_UPDATE_INTERVAL = 1000;
bool wifiAP = false;
bool isWifiAP = false;
bool isWifi = false;
bool wifi = false;
bool lastSaveDate = false;
bool isAlarmActive[3] = {false, false, false};  // Array untuk status aktif 3 alarm
bool isAlarm[3] = {false, false, false};  // Array untuk 3 alarm
bool setAlarm = false;
bool saveAlarm = false;
bool hiddenAlarm = false;
bool isPlay = false;
bool isCharging = false;
bool lastCharging = false;
int playTestAudio = 0;
bool isClosePopup = false;
bool lastAlarmActiveState[3] = {false, false, false};  // Array untuk state sebelumnya 3 alarm
bool tempAlarmFlag = false;  // Temporary flag for audio system compatibility - true to allow playback
String fileSelected;
int total = 0;
int indexAlarm = 0;
int cairanTubuh = 0;
int cairanTubuhTotal = 0;
lv_obj_t *previousScreen = NULL;  // Track screen sebelum alarm

unsigned long alarmUntil = 0;
String HH_MM = "21:00";
String CURRENT_DATE = "TUE 18/10/2025";
String jsonData = "[]";

bool initializeHardware(){
  bool isSDCard = memory->begin(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
  if (!isSDCard) {
    ESP_LOGE(TAG, "SD Card initialization failed!");
    return false;
  }
  
  Wire.begin(IIC_SDA,IIC_SCL);
  bool powerResult = power.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  if (!powerResult) {
    ESP_LOGE(TAG, "PMU initialization failed!");
    return false;
  }

  bool qmiResult = sensorStepper.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  if (!qmiResult) {
    ESP_LOGE(TAG, "QMI8658 initialization failed!");
    return false;
  }
  
  ESP_LOGI(TAG, "Hardware initialized successfully");
  return true;
}

void updateTimeDisplay() {
    if (!ntpSetup.isInitialized()) {
        isNtp = false;
        return;
    }
    isNtp = true;
    
    
    struct tm timeinfo;
    if (ntpSetup.getLocalTime(&timeinfo)) {
        // Convert to 12-hour format
        int hour12 = timeinfo.tm_hour;

        char hourMinStr[6];
        snprintf(hourMinStr, sizeof(hourMinStr), "%02d:%02d", hour12, timeinfo.tm_min);
        HH_MM = String(hourMinStr);
        
        // Update date label (format: TUE - 8)
        ntpCurrentDay = timeinfo.tm_wday;
        const char* weekdays[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
        char dateStr[18];
        snprintf(dateStr, sizeof(dateStr), "%s %02d/%02d/%04d",
                weekdays[ntpCurrentDay],
                timeinfo.tm_mday, 
                timeinfo.tm_mon + 1, 
                timeinfo.tm_year + 1900);
        CURRENT_DATE = String(dateStr);
    }
}

int parseHHMM(const char *t)
{
    if (!t)
        return -1;
    if (strlen(t) != 5 || t[2] != ':')
        return -1;
    int h = (t[0] - '0') * 10 + (t[1] - '0');
    int m = (t[3] - '0') * 10 + (t[4] - '0');
    if (h < 0 || h > 23 || m < 0 || m > 59)
        return -1;
    return h * 60 + m;
}

// Forward declarations
bool isWithinSchedule(const char *startHHMM, const char *endHHMM);

int current = 0;

// Helper function to save schedule by index
bool saveScheduleByIndex(int index, const char *startTime, const char *endTime) {
    if (!startTime || !endTime || strlen(startTime) != 5 || strlen(endTime) != 5) {
        ESP_LOGE(TAG_STORAGE, "Invalid time format - Index: %d, Start: %s, End: %s", 
                 index, startTime ? startTime : "NULL", endTime ? endTime : "NULL");
        return false;
    }
    
    if (STORAGE.saveSchedule(startTime, endTime, index)) {
        ESP_LOGI(TAG_STORAGE, "Schedule%d saved: %s - %s", index, startTime, endTime);
        return true;
    } else {
        ESP_LOGE(TAG_STORAGE, "Failed to save schedule%d", index);
        return false;
    }
}

// Helper function to check if any alarm should be active
bool isAnyAlarmActive() {
    // Get all schedule times
    const char* schedules[3][2] = {
        {STORAGE.getStartTime(), STORAGE.getEndTime()},
        {STORAGE.getStartTime2(), STORAGE.getEndTime2()},
        {STORAGE.getStartTime3(), STORAGE.getEndTime3()}
    };
    
    // Check each schedule with corresponding alarm
    for (int i = 0; i < 3; i++) {
        if (isWithinSchedule(schedules[i][0], schedules[i][1]) && isAlarm[i]) {
            return true;
        }
    }
    return false;
}

bool isWithinSchedule(const char *startHHMM, const char *endHHMM)
{
    int start = parseHHMM(startHHMM);
    int end = parseHHMM(endHHMM);
    if (start < 0 || end < 0)
        return true; // invalid => treat as always active
    if (isNtp)
    {
      struct tm timeinfo;
      if (!ntpSetup.getLocalTime(&timeinfo)) {
          return false;
      }
      current = timeinfo.tm_hour * 60 + timeinfo.tm_min;
      if (!setAlarm)
      {
        int32_t setTime[2] = {timeinfo.tm_hour, timeinfo.tm_min};
        timeAlarm(setTime, 2);
      }
      
    }else
    {
      RTC_DateTime now = rtc.getDateTime();
      current = now.hour * 60 + now.minute;
      if (!setAlarm)
      {
        int32_t setTime[2] = {now.hour, now.minute};
        timeAlarm(setTime, 2);
      }
    }
    if (start == end)
        return true; // full day
    if (start < end)
      return current >= start && current < end;
    // crosses midnight
    return current >= start || current < end;
}


void setup() {
  USBSerial.begin(115200);
  USBSerial.setDebugOutput(false);
  delay(1000);

  ESP_LOGI(TAG, "ESP32-S3 Smartwatch starting...");

  wifiSetup = new WifiSetup();
  memory = new Memory();
  webServer = new clientServer(memory);
  audioSystem = new Audio(&i2s, memory);
  stepCounter = new StepCounter();
  battery = new BatteryMonitor();

  i2c_mutex = xSemaphoreCreateMutex();
  if (i2c_mutex == NULL) {
    ESP_LOGE(TAG, "Failed to create I2C mutex!");
    while(1) delay(100);
  }

  while (!initializeHardware()) {
    ESP_LOGE(TAG, "Retrying hardware initialization...");
    delay(100);
  }

  lvgl_init();
  ui_init();
  ESP_LOGI(TAG, "UI initialized");
  
  // Watchdog temporarily disabled - causing errors with new API
  // TODO: Re-implement with proper task registration
  // esp_task_wdt_config_t wdt_config = {...};
  // esp_task_wdt_init(&wdt_config);

  TaskHandle_t wifiTaskHandle;
  xTaskCreatePinnedToCore(
      applyWiFiMode,   
      "WiFiTask",      
      4096,            
      NULL,            
      2,               // Priority 2 (lower than loop/UI)
      &wifiTaskHandle, 
      0);              // Core 0

  TaskHandle_t sensorTaskHandle;
  xTaskCreatePinnedToCore(
      applySensor,     
      "sensorTask",    
      8192,
      NULL,            
      2,               // Priority 2 (lower than loop/UI)
      &sensorTaskHandle,
      1);              // Core 1

  TaskHandle_t alarmTaskHandle;
  xTaskCreatePinnedToCore(
      controlAlarmTask,
      "alarmTask",
      16384,
      NULL,
      3,               // Priority 3 (lowest - only runs when needed)
      &alarmTaskHandle,
      0);              // Core 0
  
  ESP_LOGI(TAG, "All tasks created successfully");
}

void loop() {
  lvgl_handler();
  ui_tick();
  
  // Handle auto-sleep untuk brightness management
  handleAutoSleep();

  // Handle close popup - kembalikan ke screen sebelumnya
  if (isClosePopup) {
      // Check if any alarm was previously active
      bool wasAnyAlarmActive = false;
      bool wasAnyAlarmActive2 = false;
      bool wasAnyAlarmActive3 = false;
      for (int i = 0; i < 3; i++) {
          if (lastAlarmActiveState[i]) {
            if(i==0)
            {
              wasAnyAlarmActive = true;
            }
            else if(i==1)
            {
              wasAnyAlarmActive2 = true;
            }
            else if(i==2)
            {
              wasAnyAlarmActive3 = true;
            }
            lastAlarmActiveState[i] = false; // Reset state
          }
      }
      
      if (wasAnyAlarmActive && previousScreen != NULL) {
          // Kembalikan ke screen sebelum alarm
          lv_scr_load_anim(previousScreen, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
          ESP_LOGI(TAG, "Returning to previous screen");
          setPlayAlarm(false);
          tempAlarmFlag = false;
          previousScreen = NULL;
      }
      else if (wasAnyAlarmActive2 && previousScreen != NULL) {
          // Kembalikan ke screen sebelum alarm
          lv_scr_load_anim(previousScreen, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
          ESP_LOGI(TAG, "Returning to previous screen");
          setPlayAlarm2(false);
          tempAlarmFlag = false;
          previousScreen = NULL;
      }
      else if (wasAnyAlarmActive3 && previousScreen != NULL) {
          // Kembalikan ke screen sebelum alarm
          lv_scr_load_anim(previousScreen, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
          ESP_LOGI(TAG, "Returning to previous screen");
          setPlayAlarm3(false);
          tempAlarmFlag = false;
          previousScreen = NULL;
      }
      setCairanTotal(cairanTubuh + cairanTubuhTotal);
      
      setClosePopup(false);
  }
  
  // Give more time for other tasks to execute
  // 10ms delay ensures LVGL gets ~100 updates per second which is plenty
  vTaskDelay(pdMS_TO_TICKS(10));
}

void applyWiFiMode(void *param){
  bool wifiAP = false;

  while (true) {
    if (isWifiAP != wifiAP) {
      wifiAP = isWifiAP;

      if (wifiAP){
        wifiSetup->connectAP();
        wifiSetup->setupWiFiAP();
        webServer->begin();
        ESP_LOGI(TAG, "WiFi AP mode enabled");
      } else {
        wifiSetup->disconnectAP();
        ESP_LOGI(TAG, "WiFi AP mode disabled");
      }
    }

    if(isWifiAP){
      wifiSetup->loopDns();
    }

    if (isWifi != wifi) {
      wifi = isWifi;

      if (wifi) {
        wifiSetup->connectSTA();
        wifiSetup->setupWiFiSTA(WIFI_SSID, WIFI_PASSWORD);
        if (wifiSetup->isConnected()) {
            ntpSetup.setUpdateInterval(NTP_UPDATE_INTERVAL);
            ntpSetup.initialize();
            isNtp = true;
            ESP_LOGI(TAG, "WiFi STA connected, NTP initialized");
        } else {
            isNtp = false;
            ESP_LOGW(TAG, "WiFi not connected - NTP skipped");
        }
      } else {
        wifiSetup->disconnectSTA();
        isNtp = false;
        ESP_LOGI(TAG, "WiFi STA disconnected");
      }
    }
    
    if (isNtp && wifiSetup->isConnected()) {
      ntpSetup.update();
      unsigned long currentTime = millis();
      if (currentTime - lastTimeUpdate >= TIME_UPDATE_INTERVAL) {
          updateTimeDisplay();
          lastTimeUpdate = currentTime;
      }
    }

    // Longer delay for WiFi task - no need to check so frequently
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void applySensor(void *param){
  vTaskDelay(pdMS_TO_TICKS(500));
  
  if (!STORAGE.init()) {
    ESP_LOGE(TAG_STORAGE, "Storage init failed");
  } else {
    ESP_LOGI(TAG_STORAGE, "Loading saved schedule...");
    const char *savedStart = STORAGE.getStartTime();
    const char *savedEnd = STORAGE.getEndTime();
    setScheduleTime(savedStart, savedEnd);
    ESP_LOGI(TAG_STORAGE, "Schedule loaded: %s - %s", savedStart, savedEnd);
  }
  
  if (i2c_mutex != NULL && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
    Wire.begin(IIC_SDA,IIC_SCL);
    bool powerResult = power.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
    
    if (!powerResult) {
      ESP_LOGE(TAG_SENSOR, "PMU initialization failed!");
    } else {
      power.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
      power.setChargeTargetVoltage(3);
      power.clearIrqStatus();
      power.enableIRQ(XPOWERS_AXP2101_PKEY_SHORT_IRQ);
    }
    
    xSemaphoreGive(i2c_mutex);
  } else {
    ESP_LOGE(TAG_SENSOR, "Failed to acquire I2C mutex");
  }

  if (!battery->init(&power)) {
    ESP_LOGE(TAG_SENSOR, "Battery monitor init failed!");
  }
  
  if (i2c_mutex != NULL && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
    if (!rtc.begin(Wire, PCF85063_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
      ESP_LOGE(TAG_SENSOR, "Failed to find PCF8563!");
    } else {
      rtc.setDateTime(2025, 10, 18, 0, 0, 0);
    }
    xSemaphoreGive(i2c_mutex);
  }

  if (i2c_mutex != NULL && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
    bool qmiResult = sensorStepper.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
    if (!qmiResult) {
      ESP_LOGE(TAG_SENSOR, "QMI8658 init failed!");
    } else {
      sensorStepper.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, 
                            SensorQMI8658::ACC_ODR_1000Hz, 
                            SensorQMI8658::LPF_MODE_0);
      sensorStepper.enableAccelerometer();
    }
    xSemaphoreGive(i2c_mutex);
  }

  if (!stepCounter->init(&sensorStepper)) {
    ESP_LOGE(TAG_SENSOR, "Step counter init failed!");
  } else {
    stepCounter->setThreshold(1.8);
    stepCounter->setSoundEnabled(true);
  }
  
  ESP_LOGI(TAG_SENSOR, "All sensors initialized");
  bool stepDetected = false;

  while (true) {
    // lvgl_handler();
    // ui_tick();
    
    // // Handle auto-sleep untuk brightness management
    // handleAutoSleep();
    isClosePopup = getClosePopup();
    isWifiAP = switchWiFiAP();
    isWifi = switchWiFi();
    
    // Read all alarms efficiently
    isAlarm[0] = playAlarm();
    isAlarm[1] = playAlarm2();
    isAlarm[2] = playAlarm3();
    
    playTestAudio = playAudio();
    saveAlarm  = getSaveAlarm();
    hiddenAlarm = gethidenAlaram();
    setAlarm = getSetAlarm();
    indexAlarm = getIndexAlarm();
    cairanTubuh = getCairan();
    cairanTubuhTotal = getCairanTotal();
    const char *nama = getNama();
    int32_t bb = getBB();
    const char *startTime = getStartTime();
    const char *endTime = getEndTime();
    const char *startTime2 = getStartTime2();
    const char *endTime2 = getEndTime2();
    const char *startTime3 = getStartTime3();
    const char *endTime3 = getEndTime3();
    fileSelected = getFileAudioSelected();
    
    static unsigned long lastJsonUpdate = 0;
    static String fileNames[10];
    static const char* fileNamePtrs[10];
    static int fileCount = 0;
    static const unsigned long JSON_UPDATE_INTERVAL = 5000; // Update every 5 seconds instead of 1
    
    if (millis() - lastJsonUpdate > JSON_UPDATE_INTERVAL) {
      jsonData = memory->listDirJson("/");
      int newTotal = memory->totalFile();
      
      // Only update UI if file count changed (optimization)
      if (newTotal != total) {
        total = newTotal;
        
        fileCount = 0;
        int jsonLen = jsonData.length();
        String currentFile = "";
        bool inQuotes = false;
        
        for (int i = 0; i < jsonLen && fileCount < 10; i++) {
          char c = jsonData[i];
          
          if (c == '"') {
            if (inQuotes) {
              if (currentFile.length() > 0) {
                fileNames[fileCount] = currentFile;
                fileNamePtrs[fileCount] = fileNames[fileCount].c_str();
                fileCount++;
                currentFile = "";
              }
            }
            inQuotes = !inQuotes;
          } else if (inQuotes) {
            currentFile += c;
          }
        }
        
        if (fileCount > 0) {
          fileAudio(fileNamePtrs, fileCount);
        } else {
          fileAudio(nullptr, 0);
        }
        
        ESP_LOGD(TAG_SENSOR, "File list updated: %d files", fileCount);
      }
      
      lastJsonUpdate = millis();
    }
    
    if (playTestAudio == 1) {
      isPlay = true;
    } else {
      isPlay = false;
    }

    stepDetected = isStep();
    stepCounter->handleButton(stepDetected);
    battery->update();
    stepCounter->update();

    uint32_t status = power.getIrqStatus();
    if (power.isPekeyShortPressIrq()) {
      battery->disableADC();
      power.shutdown();
    }
    power.clearIrqStatus();

    isCharging = battery->isCharging();

    chargeState(isCharging);
    setBT(battery->getBatteryPercentage());
    setStep(stepCounter->getStepCount());

    if (getCalculate() == true && bb > 0) {
      int hidrasi = 0;
      
      if (bb < 10) {
        hidrasi = bb * 100;
      } else if (bb >= 10 && bb <= 20) {
        hidrasi = 1000 + ((bb - 10) * 50);
      } else {
        hidrasi = 1500 + ((bb - 20) * 20);
      }
      
      String hasil = String("         ") + String(nama) + ", \nkebutuhan cairanmu \n   " + String(hidrasi) + " ml / hari";
      sethasilHitung(hasil.c_str());
    }

    if (saveAlarm && indexAlarm >= 1 && indexAlarm <= 3) {
      // Get appropriate time variables based on alarm index
      const char* start = nullptr;
      const char* end = nullptr;
      
      switch (indexAlarm) {
        case 1: start = startTime; end = endTime; break;
        case 2: start = startTime2; end = endTime2; break;  
        case 3: start = startTime3; end = endTime3; break;
      }
      
      // Use unified save function with validation
      if (saveScheduleByIndex(indexAlarm, start, end)) {
        saveAlarm = false;
        setSaveAlarm(saveAlarm);
      } else {
        saveAlarm = false;
        setSaveAlarm(saveAlarm);
      }
    } else {
      // Check if any alarm should be active using helper function
      bool anyAlarmShouldBeActive = isAnyAlarmActive();
      
      if (anyAlarmShouldBeActive) {
        // setCairan(0);
        // Find which alarm is active and update corresponding state
        for (int i = 0; i < 3; i++) {
          bool shouldBeActive = false;
          
          // Check schedule for each alarm
          if (i == 0) {
            shouldBeActive = isWithinSchedule(STORAGE.getStartTime(), STORAGE.getEndTime()) && isAlarm[i];
          }
          else if (i == 1){
            shouldBeActive = isWithinSchedule(STORAGE.getStartTime2(), STORAGE.getEndTime2()) && isAlarm[i];
          }
          else if (i == 2){
            shouldBeActive = isWithinSchedule(STORAGE.getStartTime3(), STORAGE.getEndTime3()) && isAlarm[i];
          } 
          
          // Deteksi perubahan state dan buka screen data cairan masuk
          if (shouldBeActive && !isClosePopup && !lastAlarmActiveState[i]) {
            ESP_LOGI(TAG_SENSOR, "Alarm %d activated - Opening data cairan masuk screen", i+1);
            
            // Simpan screen sebelumnya hanya jika belum disimpan
            if (previousScreen == NULL) {
              previousScreen = lv_scr_act();
            }
            
            lv_textarea_set_text(objects.jumlah_cairan, "0");
            // lv_label_set_text(objects.jumlah_cairan, "0");
            // Switch ke screen data cairan masuk dengan animasi
            lv_scr_load_anim(objects.data_cairan_masuk, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false);
            lastAlarmActiveState[i] = true;
          }
          
          isAlarmActive[i] = shouldBeActive;
        }
      } else {
        // Reset all alarm active states
        for (int i = 0; i < 3; i++) {
          isAlarmActive[i] = false;
        }
      }
    }


    if (isNtp) {
      isLastNtp = isNtp;
      static uint32_t lastCheck = 0;
      if (millis() - lastCheck > 10000){
        lastCheck = millis();
        setTimeHHMM((HH_MM).c_str());
      }

      if (ntpCurrentDay != ntpLastDay) {
        setCurrentDate((CURRENT_DATE).c_str());
        if (!saveAlarm && !hiddenAlarm)
        {
          setPlayAlarm(true);
        }
        ntpLastDay = ntpCurrentDay;
      }
    } else {
      if (saveDate() != lastSaveDate) {
        lastSaveDate = saveDate();
        if (lastSaveDate) {
          uint16_t *dt = getDateTime();
            rtc.setDateTime(dt[0],dt[1] + 1,dt[2],dt[3],dt[4],dt[5]);
        }
      }

      if (isNtp != isLastNtp) {
        struct tm timeinfo;
        if (ntpSetup.getLocalTime(&timeinfo)) {
            rtc.setDateTime(timeinfo.tm_year+1900, timeinfo.tm_mon+1, timeinfo.tm_mday,
                                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        }
        uint16_t dt[] = {(uint16_t)(timeinfo.tm_year+1900), (uint16_t)(timeinfo.tm_mon+1),
                              (uint16_t)timeinfo.tm_mday, (uint16_t)timeinfo.tm_hour,
                              (uint16_t)timeinfo.tm_min, (uint16_t)timeinfo.tm_sec};
        setDateTime(dt, 6);
        isLastNtp = isNtp;
      }
      
      static uint32_t lastCheck = 0;
      if (millis() - lastCheck > 10000){
        lastCheck = millis();
        RTC_DateTime dt = rtc.getDateTime();
        
        char buf[7];
        snprintf(buf, sizeof(buf), "%02d:%02d", dt.hour, dt.minute);
        setTimeHHMM(buf);

        currentDay = dt.week;
        const char* weekdays[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
        
        snprintf(rtcDateStr, sizeof(rtcDateStr), "%s %02d/%02d/%04d",
                weekdays[currentDay], dt.day, dt.month, dt.year);
    if (currentDay != lastDay) {
      setCurrentDate(rtcDateStr);
      if (!saveAlarm && !hiddenAlarm)
      {
        setPlayAlarm(true);
      }
      lastDay = currentDay;
    }
  }
}

    // Sensor readings don't need to be super fast
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void controlAlarmTask(void *param) {
    bool isPlaying = false;
    
    ESP_LOGI(TAG, "Alarm task started, waiting for system ready...");
    vTaskDelay(pdMS_TO_TICKS(5000));
    
    // Initialize audio system
    if (!audioSystem->begin(BCLKPIN, WSPIN, DOPIN, DIPIN, MCLKPIN, PA)) {
        ESP_LOGE(TAG, "Audio system initialization failed!");
        vTaskDelete(NULL);
        return;
    }

    while (true) {

        if (isCharging != lastCharging)
        {
          lastCharging = isCharging;
          audioSystem->generateTestTone();
        }

        // Check if any alarm is active and start audio playback
        bool anyAlarmActive = false;
        for (int i = 0; i < 3; i++) {
            if (isAlarmActive[i]) {
                anyAlarmActive = true;
                isAlarmActive[i] = false; // Reset after processing
            }
        }
        
        if (anyAlarmActive && !isPlaying) {
            alarmUntil = millis() + 6000UL;
            tempAlarmFlag = true;  // Temporary flag for audio system compatibility - true to allow playback
        }

        bool shouldBePlaying = (long)(millis() - alarmUntil) < 0;
        
        // Test audio playback
        if (isPlay && !isPlaying && audioSystem->isInitialized()) {
            webServer->setFileSelected(fileSelected);
            
            isPlaying = true;
            audioSystem->setPowerAmplifier(true);
            
            static String wavFilePath;
            wavFilePath = "/" + String(webServer->getFileSelected());
            audioSystem->playWavFile(wavFilePath.c_str(), &isPlay, true);
            
            isPlaying = false;
        }

        // Alarm playback
        if (shouldBePlaying && !isPlaying && audioSystem->isInitialized()) {
            isPlaying = true;
            audioSystem->setPowerAmplifier(true);
            
            ESP_LOGI(TAG, "Alarm triggered (Schedule: %s - %s)",
                STORAGE.getStartTime(), STORAGE.getEndTime());
            
            String selectedFile = webServer->getFileSelected();
            bool playedSuccessfully = false;
           

            if (selectedFile != "") {
              static String wavFilePath;
              wavFilePath = "/" + selectedFile;
              playedSuccessfully = audioSystem->playWavFile(wavFilePath.c_str(), &tempAlarmFlag);
            }

            // Fallback to default audio if no file selected or playback failed
            if (!playedSuccessfully) {
              audioSystem->playDefaultAudio(canon_pcm, canon_pcm_len, &tempAlarmFlag);
            }

            // Ensure playback flag is cleared after playback finishes
            tempAlarmFlag = false;

            isPlaying = false;
            alarmUntil = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}