#include <math.h>
#include <Wire.h>
#include <cstring> // <- add for memcmp/strcmp

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
#include "ui.h"
#include "vars.h"


#include "pin_config.h"
#include "ESP_I2S.h"
I2SClass i2s;

#include "esp_check.h"
#include "es8311.h"
#include "canon.h" 

#define EXAMPLE_SAMPLE_RATE 16000
#define EXAMPLE_VOICE_VOLUME 80                  // 0 - 100
#define EXAMPLE_MIC_GAIN (es8311_mic_gain_t)(3)  // 0 - 7

const char *TAG = "esp32p4_i2s_es8311";

// Mutex untuk melindungi akses I2C
SemaphoreHandle_t i2c_mutex = NULL;

esp_err_t es8311_codec_init(void) {
  USBSerial.println("Initializing ES8311 codec...");
  
  // Ambil mutex I2C sebelum akses
  if (i2c_mutex != NULL && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
    // Scan I2C untuk mencari ES8311 (address 0x18)
    Wire.beginTransmission(0x18); // ES8311_ADDRRES_0 = 0x18
    byte error = Wire.endTransmission();
    
    xSemaphoreGive(i2c_mutex); // Lepas mutex
    
    if (error != 0) {
      USBSerial.printf("ES8311 not found at I2C address 0x18, error: %d\n", error);
      return ESP_FAIL;
    }
  } else {
    USBSerial.println("Failed to acquire I2C mutex for ES8311 scan");
    return ESP_FAIL;
  }
  USBSerial.println("ES8311 device found on I2C bus");
  
  es8311_handle_t es_handle = es8311_create(0, ES8311_ADDRRES_0);
  if (!es_handle) {
    USBSerial.println("ES8311 create failed");
    return ESP_FAIL;
  }
  
  const es8311_clock_config_t es_clk = {
    .mclk_inverted = false,
    .sclk_inverted = false,
    .mclk_from_mclk_pin = true,
    .mclk_frequency = EXAMPLE_SAMPLE_RATE * 256,
    .sample_frequency = EXAMPLE_SAMPLE_RATE
  };

  // Gunakan error handling yang lebih soft tanpa ESP_ERROR_CHECK
  esp_err_t err = es8311_init(es_handle, &es_clk, ES8311_RESOLUTION_16, ES8311_RESOLUTION_16);
  if (err != ESP_OK) {
    USBSerial.printf("ES8311 init failed: %s\n", esp_err_to_name(err));
    return err;
  }
  
  err = es8311_sample_frequency_config(es_handle, es_clk.mclk_frequency, es_clk.sample_frequency);
  if (err != ESP_OK) {
    USBSerial.printf("ES8311 sample frequency config failed: %s\n", esp_err_to_name(err));
    return err;
  }
  
  err = es8311_microphone_config(es_handle, false);
  if (err != ESP_OK) {
    USBSerial.printf("ES8311 microphone config failed: %s\n", esp_err_to_name(err));
    return err;
  }
  
  // Enable output (DAC) untuk speaker
  err = es8311_voice_volume_set(es_handle, EXAMPLE_VOICE_VOLUME, NULL);
  if (err != ESP_OK) {
    USBSerial.printf("ES8311 volume set failed: %s\n", esp_err_to_name(err));
    return err;
  }
  USBSerial.printf("ES8311 volume set to: %d\n", EXAMPLE_VOICE_VOLUME);
  
  err = es8311_microphone_gain_set(es_handle, EXAMPLE_MIC_GAIN);
  if (err != ESP_OK) {
    USBSerial.printf("ES8311 microphone gain set failed: %s\n", esp_err_to_name(err));
    return err;
  }
  
  USBSerial.println("ES8311 codec initialized successfully");
  return ESP_OK;
}


HWCDC USBSerial;
WifiSetup *wifiSetup;
NTPSetup ntpSetup(NTP_TIMEZONE, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
// static Communication comm; 
Memory *memory;
clientServer *webServer;
SensorPCF85063 rtc;

XPowersPMU power;
BatteryMonitor *battery;

SensorQMI8658 sensorStepper;
StepCounter *stepCounter;


void applyWiFiMode(void *param);
void applySensor(void *param);
void controlAlarmTask(void *param);


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
extern const char *getEndTime();
extern void timeAlarm(int32_t *data, size_t len );
extern void setScheduleTime(const char *startTime, const char *endTime);
extern void fileAudio(const char **data, size_t len);
extern bool playAlarm();
extern int playAudio();
extern const char *getFileAudioSelected();
extern bool getSetAlarm();
extern void setPlayAlarm(bool data);
extern const char *getNama();
extern int32_t getUsia();
extern int32_t getBB();
extern bool getCalculate();
extern void setCalculate(bool data);
extern void sethasilHitung(const char* hasil);

int lastDay = -1;
int currentDay = -1;
bool isNtp = false;
bool isLastNtp = false;
unsigned long lastTimeUpdate = 0;
const unsigned long TIME_UPDATE_INTERVAL = 1000; // Update every second
bool wifiAP = false;
bool isWifiAP = false;
bool isWifi = false;
bool wifi = false;
bool lastSaveDate = false;
bool isAlarmActive = false;
bool isAlarm = false;
bool setAlarm = false;
bool saveAlarm = false;
bool hiddenAlarm = false;
bool isPlay = false;
int playTestAudio = 0;
String fileSelected;
int total = 0;
unsigned long lastStatusPrint = 0;

// --- MODIFIKASI: Pindahkan variabel alarm ke scope global agar bisa diakses oleh task alarm ---
unsigned long alarmUntil = 0; // millis timestamp until which alarm stays active
String HH_MM = "21:00";
String CURRENT_DATE = "TUE 18/10/2025";
String jsonData = "[]";  // Static storage untuk JSON data dari SD card

bool initializeHardware(){
  bool isSDCard = memory->begin(SDMMC_CLK, SDMMC_CMD, SDMMC_DATA);
  if (!isSDCard)
  {
    USBSerial.println("SD Card initialization failed!");
    return false;
  }else{
    USBSerial.println(memory->getCardInfo());
  }
  
  Wire.begin(IIC_SDA,IIC_SCL);
  bool powerResult = power.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  if (!powerResult)
  {
      USBSerial.println("PMU initialization failed!");
      return false;
  }

  bool qmiResult = sensorStepper.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  if (!qmiResult) {
    USBSerial.println("QMI8658 initialization failed!");
    return false;
  }
  
  USBSerial.println("Hardware initialized successfully");
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
        currentDay = timeinfo.tm_wday;
        const char* weekdays[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
        char dateStr[18];
        snprintf(dateStr, sizeof(dateStr), "%s %02d/%02d/%04d",
                weekdays[currentDay],
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

int current = 0;
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
  USBSerial.setDebugOutput(true);
  delay(1000);
  // pinMode(0, INPUT_PULLUP); // Button off alarm

  // --- PERBAIKAN: Pindahkan inisialisasi objek ke atas ---
  wifiSetup = new WifiSetup();
  memory = new Memory();
  webServer = new clientServer(memory);
  stepCounter = new StepCounter();
  battery = new BatteryMonitor();
  // ---------------------------------------------------------

  // Initialize I2C mutex
  i2c_mutex = xSemaphoreCreateMutex();
  if (i2c_mutex == NULL) {
      USBSerial.println("Failed to create I2C mutex!");
      while(1) delay(100);
  }

  // Initialize hardware
  while (!initializeHardware()) {
      USBSerial.println("Hardware initialization failed!");
      delay(100);
  }

  // Sekarang aman untuk menginisialisasi LVGL dan UI
  lvgl_init();
  USBSerial.println("LVGL initialized");
  ui_init();
  USBSerial.println("UI initialized");


  TaskHandle_t wifiTaskHandle;
  xTaskCreatePinnedToCore(
      applyWiFiMode,   
      "WiFiTask",      
      4096,            
      NULL,            
      1,               
      &wifiTaskHandle, 
      0);                           

  TaskHandle_t sensorTaskHandle;
  xTaskCreatePinnedToCore(
      applySensor,     
      "sensorTask",    
      8192,            // Increase stack untuk LVGL
      NULL,            
      1,               
      &sensorTaskHandle,
      1); 

  // --- MODIFIKASI: Buat task baru untuk kontrol alarm ---
  TaskHandle_t alarmTaskHandle;
  xTaskCreatePinnedToCore(
      controlAlarmTask,
      "alarmTask",
      16384,            // Increase stack size untuk audio processing (16KB)
      NULL,
      1,                // Sama dengan sensor task untuk menghindari konflik
      &alarmTaskHandle,
      0);               // Core 0 untuk menghindari konflik I2C di core 1
}

void loop() {
  vTaskDelay(5);
}

void applyWiFiMode(void *param){
  bool wifiAP = false;

  while (true)
  {
    if (isWifiAP != wifiAP)
    {
      wifiAP = isWifiAP;

      if (wifiAP){
        wifiSetup->connectAP();
        wifiSetup->setupWiFiAP();
        webServer->begin();
      }
      else{
        wifiSetup->disconnectAP();
        // comm.stop();
      }
    }

    if(isWifiAP){
      wifiSetup->loopDns();
    }

    if (isWifi != wifi)
    {
      wifi = isWifi;

      if (wifi)
      {
        wifiSetup->connectSTA();
        wifiSetup->setupWiFiSTA(WIFI_SSID, WIFI_PASSWORD);
        if (wifiSetup->isConnected()) {
            ntpSetup.setUpdateInterval(NTP_UPDATE_INTERVAL);
            ntpSetup.initialize();
            isNtp = true;
        } else {
            USBSerial.println("WiFi not connected - NTP initialization skipped");
            isNtp = false;
        }
      }else
      {
        wifiSetup->disconnectSTA();
        isNtp = false;
      }
    }
    
    if (isNtp)
    {
      ntpSetup.update();
      unsigned long currentTime = millis();
      if (currentTime - lastTimeUpdate >= TIME_UPDATE_INTERVAL) {
          updateTimeDisplay();
          lastTimeUpdate = currentTime;
      }
    }

    vTaskDelay(10);
  }
}

void applySensor(void *param){
  // Tunggu mutex tersedia dan UI flow siap
  vTaskDelay(pdMS_TO_TICKS(500));
  
  // Inisialisasi STORAGE dan load schedule
  if (!STORAGE.init())
  {
      USBSerial.println(F("[Storage] init failed"));
  } else {
      // Load schedule dari storage dan set ke flow variables
      USBSerial.println(F("[Storage] Loading saved schedule..."));
      const char *savedStart = STORAGE.getStartTime();
      const char *savedEnd = STORAGE.getEndTime();
      
      USBSerial.print("Saved Start Time: ");
      USBSerial.println(savedStart);
      USBSerial.print("Saved End Time: ");
      USBSerial.println(savedEnd);
      
      // Set ke flow variables (UI harus sudah ready di sini)
      setScheduleTime(savedStart, savedEnd);
  }
  
  if (i2c_mutex != NULL && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
    Wire.begin(IIC_SDA,IIC_SCL);
    bool powerResult = power.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
    
    if (!powerResult) {
        USBSerial.println("PMU initialization failed!");
    } else {
        power.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
        power.setChargeTargetVoltage(3);
        power.clearIrqStatus();
        power.enableIRQ(XPOWERS_AXP2101_PKEY_SHORT_IRQ);
    }
    
    xSemaphoreGive(i2c_mutex);
  } else {
    USBSerial.println("Failed to acquire I2C mutex in sensor task");
  }

  if (!battery->init(&power)) {
      USBSerial.println("Battery monitor initialization failed!");
  } else {
      USBSerial.println("Battery monitor initialized");
  }
  
  // Inisialisasi RTC dengan proteksi mutex
  if (i2c_mutex != NULL && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
    if (!rtc.begin(Wire, PCF85063_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
      USBSerial.println("Failed to find PCF8563 - check your wiring!");
    } else {
      USBSerial.println("Success to find PCF8563");
      // Set waktu default sekali saja
      rtc.setDateTime(2025, 10, 18, 0, 0, 0);
    }
    xSemaphoreGive(i2c_mutex);
  }

  // Inisialisasi QMI8658 dengan proteksi mutex
  if (i2c_mutex != NULL && xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
    bool qmiResult = sensorStepper.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
    if (!qmiResult) {
      USBSerial.println("QMI8658 initialization failed!");
    } else {
      sensorStepper.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, 
                            SensorQMI8658::ACC_ODR_1000Hz, 
                            SensorQMI8658::LPF_MODE_0);
      sensorStepper.enableAccelerometer();
    }
    xSemaphoreGive(i2c_mutex);
  }

  if (!stepCounter->init(&sensorStepper)) {
      USBSerial.println("Step counter initialization failed!");
  } else {
      USBSerial.println("Step counter initialized");
      stepCounter->setThreshold(1.8);
      stepCounter->setSoundEnabled(true);
  }
  bool stepDetected = false;

  while (true)
  {
    lvgl_handler();
    ui_tick();

    isWifiAP = switchWiFiAP();
    isWifi = switchWiFi();
    isAlarm = playAlarm();
    playTestAudio = playAudio();
    saveAlarm  = getSaveAlarm();
    hiddenAlarm = gethidenAlaram();
    setAlarm = getSetAlarm();
    const char *nama = getNama();
    int32_t bb = getBB();
    const char *startTime = getStartTime();
    const char *endTime = getEndTime();
    fileSelected = getFileAudioSelected();
    
    // Update JSON data di task yang sama dengan yang menggunakannya (menghindari race condition)
    static unsigned long lastJsonUpdate = 0;
    static String fileNames[10];  // Max 10 files
    static const char* fileNamePtrs[10];
    static int fileCount = 0;
    
    if (millis() - lastJsonUpdate > 1000) {  // Update setiap 1 detik
      jsonData = memory->listDirJson("/");
      total = memory->totalFile();
      
      // Parse JSON string menjadi array of filenames
      // JSON format: ["file1.wav","file2.wav",...]
      fileCount = 0;
      int jsonLen = jsonData.length();
      String currentFile = "";
      bool inQuotes = false;
      
      for (int i = 0; i < jsonLen && fileCount < 10; i++) {
        char c = jsonData[i];
        
        if (c == '"') {
          if (inQuotes) {
            // End of filename
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
      
      // Update UI dengan array of filenames
      if (fileCount > 0) {
        fileAudio(fileNamePtrs, fileCount);
      } else {
        // Set empty array jika tidak ada file
        fileAudio(nullptr, 0);
      }
      
      lastJsonUpdate = millis();
    }
    
    if (playTestAudio == 1)
    {
      isPlay = true;
      // USBSerial.println("isPlay true from playTestAudio");
    }else{
      isPlay = false;
      // USBSerial.println("isPlay false from playTestAudio");
    }
    

    // mButton->tick();
    stepDetected = isStep(); // Baca status step di dalam perlindungan mutex
    stepCounter->handleButton(stepDetected);
    battery->update();
    stepCounter->update();

    uint32_t status = power.getIrqStatus();
    if (power.isPekeyShortPressIrq()) {
      battery->disableADC();
      power.shutdown();
    }
    power.clearIrqStatus();
    
    // --- Logika yang tidak butuh I2C bisa ditaruh di luar ---
    chargeState(battery->isCharging());
    setBT(battery->getBatteryPercentage());
    setStep(stepCounter->getStepCount());

    if (getCalculate() == true && bb > 0)
    {
      // Hitung kebutuhan cairan berdasarkan berat badan
      int hidrasi = 0;
      
      if (bb < 10) {
        hidrasi = bb * 100;
      } else if (bb >= 10 && bb <= 20) {
        hidrasi = 1000 + ((bb - 10) * 50);
      } else { // bb > 20
        hidrasi = 1500 + ((bb - 20) * 20);
      }
      
      String hasil = String("         ") + String(nama) + ", \nkebutuhan cairanmu \n   " + String(hidrasi) + " ml / hari";
      sethasilHitung(hasil.c_str());
    }
    

    if (saveAlarm && !isAlarm)
    {
      USBSerial.print("saveAlarm : ");
      USBSerial.println(saveAlarm);
      USBSerial.print("isAlarm : ");
      USBSerial.println(isAlarm);
      USBSerial.print("setAlarm : ");
      USBSerial.println(setAlarm);
      
      // Debug: Tampilkan nilai startTime dan endTime
      USBSerial.print("startTime : ");
      USBSerial.println(startTime ? startTime : "NULL");
      USBSerial.print("endTime : ");
      USBSerial.println(endTime ? endTime : "NULL");
      
      // Validasi sebelum save
      if (startTime == NULL || endTime == NULL) {
        USBSerial.println("ERROR: startTime or endTime is NULL!");
        saveAlarm = false;
        setSaveAlarm(saveAlarm);
      } else if (strlen(startTime) != 5 || strlen(endTime) != 5) {
        USBSerial.printf("ERROR: Invalid time format - startTime: '%s' (len=%d), endTime: '%s' (len=%d)\n", 
                         startTime, strlen(startTime), endTime, strlen(endTime));
        saveAlarm = false;
        setSaveAlarm(saveAlarm);
      } else {
        USBSerial.printf("Attempting to save schedule: %s - %s\n", startTime, endTime);
        if (STORAGE.saveSchedule(startTime, endTime))
        {
          USBSerial.println("saved successfully");
          saveAlarm = false;
          setSaveAlarm(saveAlarm);
        } else {
          USBSerial.println("ERROR: STORAGE.saveSchedule() returned false!");
          saveAlarm = false;
          setSaveAlarm(saveAlarm);
        }
      }
    }else{
          // --- MODIFIKASI: Logika pemicu alarm ---
      if (isWithinSchedule(STORAGE.getStartTime(), STORAGE.getEndTime()) && isAlarm)
      {
        // if (mButton->longPress())
        // {
        //   USBSerial.println("Button long press detected - turning off alarm");
        //   setPlayAlarm(false);
        //   isAlarm = false;
        //   mButton->setRemove(false);
        //   // isAlarmActive = false;
        // }
        USBSerial.println("turning on alarm");
        isAlarmActive = true;
      }else
      {
        // USBSerial.println("isAlarmActive false");
        isAlarmActive = false;
      }

    }

    if (isNtp) {
      isLastNtp = isNtp;
      static uint32_t lastCheck = 0;
      if (millis() - lastCheck > 10000){
        lastCheck = millis();
        setTimeHHMM((HH_MM).c_str());
      }

      if (currentDay != lastDay) {
        setCurrentDate((CURRENT_DATE).c_str());
        if (!saveAlarm && !hiddenAlarm)
        {
          setPlayAlarm(true);
        }
        lastDay = currentDay;
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
        char dateStr[18];
        snprintf(dateStr, sizeof(dateStr), "%s %02d/%02d/%04d",
                weekdays[currentDay], dt.day, dt.month, dt.year);
        if (currentDay != lastDay) {
          setCurrentDate(dateStr);
          if (!saveAlarm && !hiddenAlarm)
          {
            setPlayAlarm(true);
          }
          lastDay = currentDay;
        }
      }
    }

    // --- MODIFIKASI: Pindahkan pemanggilan controlAlarm() ke task terpisah ---
    // controlAlarm(); // HAPUS PANGGILAN INI
    
    vTaskDelay(5);
  }
}

void listSDCard(const char* dirname) {
  USBSerial.printf("\n=== Listing directory: %s ===\n", dirname);
  File root = SD_MMC.open(dirname);
  if (!root) {
    USBSerial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    USBSerial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      USBSerial.printf("  DIR : %s\n", file.name());
    } else {
      USBSerial.printf("  FILE: %s\t\tSIZE: %d bytes\n", file.name(), file.size());
    }
    file = root.openNextFile();
  }
  USBSerial.println("=========================\n");
}


void readWavHeader(const char* filename) {
  File audioFile = SD_MMC.open(filename, FILE_READ);
  if (!audioFile) {
    USBSerial.printf("Failed to open %s\n", filename);
    return;
  }

  USBSerial.printf("\n=== WAV Header Info for %s ===\n", filename);
  USBSerial.printf("File Size: %d bytes\n", audioFile.size());
  
  // Baca WAV header (44 bytes)
  uint8_t header[44];
  audioFile.read(header, 44);
  
  // Parse WAV header
  char chunkID[5] = {0};
  memcpy(chunkID, header, 4);
  uint32_t chunkSize = *(uint32_t*)(header + 4);
  char format[5] = {0};
  memcpy(format, header + 8, 4);
  
  uint16_t audioFormat = *(uint16_t*)(header + 20);
  uint16_t numChannels = *(uint16_t*)(header + 22);
  uint32_t sampleRate = *(uint32_t*)(header + 24);
  uint32_t byteRate = *(uint32_t*)(header + 28);
  uint16_t blockAlign = *(uint16_t*)(header + 32);
  uint16_t bitsPerSample = *(uint16_t*)(header + 34);
  uint32_t subchunk2Size = *(uint32_t*)(header + 40);
  
  USBSerial.printf("ChunkID: %s\n", chunkID);
  USBSerial.printf("ChunkSize: %u\n", chunkSize);
  USBSerial.printf("Format: %s\n", format);
  USBSerial.printf("Audio Format: %u (1=PCM)\n", audioFormat);
  USBSerial.printf("Channels: %u\n", numChannels);
  USBSerial.printf("Sample Rate: %u Hz\n", sampleRate);
  USBSerial.printf("Byte Rate: %u\n", byteRate);
  USBSerial.printf("Block Align: %u\n", blockAlign);
  USBSerial.printf("Bits Per Sample: %u\n", bitsPerSample);
  USBSerial.printf("Data Size: %u bytes\n", subchunk2Size);
  USBSerial.printf("Expected duration: %.2f seconds\n", (float)subchunk2Size / byteRate);
  USBSerial.println("============================\n");
  
  audioFile.close();
}

// --- MODIFIKASI: Fungsi controlAlarm diubah menjadi task mandiri ---
void controlAlarmTask(void *param) {
    bool isPlaying = false;
    bool audioInitialized = false;
    
    // Tunggu hingga hardware sepenuhnya terinisialisasi
    // Delay lebih lama untuk memastikan semua task lain sudah running
    USBSerial.println("[Audio] Alarm task started, waiting for system ready...");
    vTaskDelay(pdMS_TO_TICKS(5000));  // 5 detik
    
    listSDCard("/");           // List semua file di root SD card
    readWavHeader("/test.wav"); // Baca dan tampilkan info header WAV
    // Enable PA (Power Amplifier) pin
    pinMode(PA, OUTPUT);
    digitalWrite(PA, HIGH);  // Enable amplifier
    USBSerial.println("PA (Power Amplifier) enabled");
    
    USBSerial.println("Starting audio initialization...");
    
    // Setup I2S dengan SEMUA pin termasuk MCLK
    i2s.setPins(BCLKPIN, WSPIN, DOPIN, DIPIN, MCLKPIN);
    
    if (i2s.begin(I2S_MODE_STD, EXAMPLE_SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO, I2S_STD_SLOT_BOTH)) {
        USBSerial.println("I2S initialized successfully");
        
        // Coba inisialisasi ES8311 dengan error handling
        esp_err_t codec_result = es8311_codec_init();
        if (codec_result == ESP_OK) {
            USBSerial.println("ES8311 codec initialized successfully");
            audioInitialized = true;
            
            // Test tone kecil - 100ms beep untuk verify audio output bekerja
            USBSerial.println("Playing test tone (100ms)...");
            const int testDuration = EXAMPLE_SAMPLE_RATE / 10; // 100ms
            int16_t *testTone = (int16_t*)malloc(testDuration * 2 * sizeof(int16_t));
            
            if (testTone) {
                for (int i = 0; i < testDuration; i++) {
                    // Generate 440Hz sine wave (A note)
                    float sample = sin(2.0 * PI * 440.0 * i / EXAMPLE_SAMPLE_RATE) * 10000;
                    testTone[i * 2] = (int16_t)sample;     // Left channel
                    testTone[i * 2 + 1] = (int16_t)sample; // Right channel
                }
                size_t written = i2s.write((uint8_t*)testTone, testDuration * 2 * sizeof(int16_t));
                USBSerial.printf("Test tone sent: %d bytes\n", written);
                free(testTone);
                vTaskDelay(pdMS_TO_TICKS(200));
            } else {
                USBSerial.println("Failed to allocate memory for test tone");
            }
            
        } else {
            USBSerial.printf("ES8311 codec init failed: %s\n", esp_err_to_name(codec_result));
            // Tetap set audioInitialized = true untuk test I2S tanpa codec
            audioInitialized = true;
        }
    } else {
        USBSerial.println("I2S initialization failed!");
    }

    while (true) {
        // Monitor stack usage setiap loop
        static unsigned long lastStackCheck = 0;
        if (millis() - lastStackCheck > 10000) {  // Check every 10 seconds
            UBaseType_t stackLeft = uxTaskGetStackHighWaterMark(NULL);
            USBSerial.printf("[Audio] Stack high water mark: %d bytes free\n", stackLeft * 4);
            lastStackCheck = millis();
        }
        

      
        if (isAlarmActive) {
            isAlarmActive = false; 
            if (!isPlaying) {
                alarmUntil = millis() + 6000UL;
            }
        }

        bool shouldBePlaying = (long)(millis() - alarmUntil) < 0;

        // JSON data sekarang diupdate dari applySensor task untuk menghindari race condition
        // Kode ini tidak lagi diperlukan di sini
        
        if(isPlay && !isPlaying && audioInitialized){
          USBSerial.println("[Audio] Test playback triggered by user");
          if (webServer->setFileSelected(fileSelected)) {
            USBSerial.printf("[Audio] Playing test file: %s\n", fileSelected.c_str());
            isPlaying = true;
            
            // Pastikan PA masih HIGH
            digitalWrite(PA, HIGH);
            
            // Determine which audio file to play
            const char* wavFile = nullptr;
            static String wavFilePath;  // Static to keep string in memory
            wavFilePath = "/" + String(webServer->getFileSelected());
            wavFile = wavFilePath.c_str();
            int32_t wavDataSize = memory->getWavDataSize(wavFile);
            const size_t CHUNK_SIZE = 2048;
            uint8_t* audioBuffer = (uint8_t*)malloc(CHUNK_SIZE);
            int16_t* stereoBuffer = (int16_t*)malloc(CHUNK_SIZE * 2);  // Pre-allocate stereo buffer
            if (!audioBuffer || !stereoBuffer) {
                USBSerial.println("[Audio] Failed to allocate memory, falling back to default");
                if (audioBuffer) free(audioBuffer);
                if (stereoBuffer) free(stereoBuffer);
            } else {
                size_t offset = 0;
                size_t totalWritten = 0;
                bool playbackSuccess = true;
                while (audioInitialized && offset < (size_t)wavDataSize) {
                        size_t remainingBytes = wavDataSize - offset;
                        size_t bytesToRead = (remainingBytes > CHUNK_SIZE) ? CHUNK_SIZE : remainingBytes;
                        
                        // Read chunk from SD card
                        int32_t bytesRead = memory->readWavChunk(wavFile, audioBuffer, offset, bytesToRead);
                        
                        if (bytesRead > 0) {
                            // Convert mono to stereo
                            int numSamples = bytesRead / 2;
                            int16_t* monoBuffer = (int16_t*)audioBuffer;
                            
                            // Optimized: use memcpy-like approach for better performance
                            for (int i = 0; i < numSamples; i++) {
                                int16_t sample = monoBuffer[i];
                                stereoBuffer[i * 2] = sample;       // Left channel
                                stereoBuffer[i * 2 + 1] = sample;   // Right channel
                            }
                            
                            // Write stereo data to I2S
                            size_t bytesWritten = i2s.write((uint8_t*)stereoBuffer, numSamples * 2 * sizeof(int16_t));
                            
                            if (bytesWritten > 0) {
                                offset += bytesRead;
                                totalWritten += bytesWritten;
                                
                                // Log progress every 50KB
                                if (totalWritten % 51200 == 0) {
                                    USBSerial.printf("[Audio] Progress: %d/%d bytes (%.1f%%)\n", 
                                                    totalWritten, wavDataSize * 2, 
                                                    (float)totalWritten * 100.0 / (wavDataSize * 2));
                                }
                            } else {
                                USBSerial.println("[Audio] Warning: No bytes written to I2S, retrying...");
                                vTaskDelay(pdMS_TO_TICKS(10));
                            }
                        } else {
                            USBSerial.printf("[Audio] Error reading chunk at offset %d\n", offset);
                            playbackSuccess = false;
                            break;
                        }

                        // Check if user turned off playback
                        if (!isPlay) {
                            USBSerial.println("[Audio] Playback stopped by user");
                            break;
                        }
                        
                        // Minimal delay for task yielding
                        vTaskDelay(pdMS_TO_TICKS(1));
                }
                  
                // Free allocated buffers
                free(audioBuffer);
                free(stereoBuffer);
            }
            isPlaying = false;

          }
        }

        if (shouldBePlaying && !isPlaying && audioInitialized) {
            // --- ALARM MULAI AKTIF ---
            isPlaying = true;
            
            // Pastikan PA masih HIGH
            digitalWrite(PA, HIGH);
            USBSerial.println("[Audio] PA enabled for playback");
            
            USBSerial.printf("[Control] Window:ON Alarm:TRIGGER Start:%s End:%s\n",
                STORAGE.getStartTime(), STORAGE.getEndTime());
            
            // Determine which audio file to play
            const char* wavFile = nullptr;
            bool useSDCard = false;
            
            if (webServer->getFileSelected() != "") {
                // User has selected a WAV file from SD card
                static String wavFilePath;  // Static to keep string in memory
                wavFilePath = "/" + String(webServer->getFileSelected());
                wavFile = wavFilePath.c_str();
                useSDCard = true;
                USBSerial.printf("[Audio] Playing user-selected file: %s\n", wavFile);
            }
            
            if (useSDCard && wavFile) {
                // Play WAV file from SD card
                int32_t wavDataSize = memory->getWavDataSize(wavFile);
                
                if (wavDataSize <= 0) {
                    USBSerial.printf("[Audio] Failed to get WAV file size: %s, falling back to default\n", wavFile);
                    useSDCard = false;  // Fallback to default audio
                } else {
                    USBSerial.printf("[Audio] Playing %s - Size: %d bytes\n", wavFile, wavDataSize);
                    
                    // Allocate buffers once outside the loop
                    const size_t CHUNK_SIZE = 2048;
                    uint8_t* audioBuffer = (uint8_t*)malloc(CHUNK_SIZE);
                    int16_t* stereoBuffer = (int16_t*)malloc(CHUNK_SIZE * 2);  // Pre-allocate stereo buffer
                    
                    if (!audioBuffer || !stereoBuffer) {
                        USBSerial.println("[Audio] Failed to allocate memory, falling back to default");
                        if (audioBuffer) free(audioBuffer);
                        if (stereoBuffer) free(stereoBuffer);
                        useSDCard = false;
                    } else {
                        size_t offset = 0;
                        size_t totalWritten = 0;
                        bool playbackSuccess = true;
                        
                        while (isAlarm && audioInitialized && offset < (size_t)wavDataSize) {
                            size_t remainingBytes = wavDataSize - offset;
                            size_t bytesToRead = (remainingBytes > CHUNK_SIZE) ? CHUNK_SIZE : remainingBytes;
                            
                            // Read chunk from SD card
                            int32_t bytesRead = memory->readWavChunk(wavFile, audioBuffer, offset, bytesToRead);
                            
                            if (bytesRead > 0) {
                                // Convert mono to stereo
                                int numSamples = bytesRead / 2;
                                int16_t* monoBuffer = (int16_t*)audioBuffer;
                                
                                // Optimized: use memcpy-like approach for better performance
                                for (int i = 0; i < numSamples; i++) {
                                    int16_t sample = monoBuffer[i];
                                    stereoBuffer[i * 2] = sample;       // Left channel
                                    stereoBuffer[i * 2 + 1] = sample;   // Right channel
                                }
                                
                                // Write stereo data to I2S
                                size_t bytesWritten = i2s.write((uint8_t*)stereoBuffer, numSamples * 2 * sizeof(int16_t));
                                
                                if (bytesWritten > 0) {
                                    offset += bytesRead;
                                    totalWritten += bytesWritten;
                                    
                                    // Log progress every 50KB
                                    if (totalWritten % 51200 == 0) {
                                        USBSerial.printf("[Audio] Progress: %d/%d bytes (%.1f%%)\n", 
                                                        totalWritten, wavDataSize * 2, 
                                                        (float)totalWritten * 100.0 / (wavDataSize * 2));
                                    }
                                } else {
                                    USBSerial.println("[Audio] Warning: No bytes written to I2S, retrying...");
                                    vTaskDelay(pdMS_TO_TICKS(10));
                                }
                            } else {
                                USBSerial.printf("[Audio] Error reading chunk at offset %d\n", offset);
                                playbackSuccess = false;
                                break;
                            }
                            
                            // Check if user turned off alarm
                            if (!isAlarm) {
                                USBSerial.println("[Audio] Alarm stopped by user");
                                break;
                            }
                            
                            // Minimal delay for task yielding
                            vTaskDelay(pdMS_TO_TICKS(1));
                        }
                        
                        // Free allocated buffers
                        free(audioBuffer);
                        free(stereoBuffer);
                        
                        if (playbackSuccess) {
                            USBSerial.printf("[Audio] Playback complete - Total written: %d bytes\n", totalWritten);
                            USBSerial.println("[Control] Alarm:IDLE");
                        }
                    }
                }
            }
            
            // Fallback to default audio (canon.pcm) if no SD card file or error occurred
            if (!useSDCard) {
                USBSerial.printf("[Audio] Playing default audio - Size: %d bytes\n", canon_pcm_len);
                
                const size_t CHUNK_SIZE = 2048;
                size_t offset = 0;
                size_t totalWritten = 0;
                
                while (isAlarm && audioInitialized && offset < canon_pcm_len) {
                    size_t remainingBytes = canon_pcm_len - offset;
                    size_t bytesToWrite = (remainingBytes > CHUNK_SIZE) ? CHUNK_SIZE : remainingBytes;
                    
                    // Write chunk to I2S
                    size_t bytesWritten = i2s.write((uint8_t *)canon_pcm + offset, bytesToWrite);
                    
                    if (bytesWritten > 0) {
                        offset += bytesWritten;
                        totalWritten += bytesWritten;
                        
                        // Log progress every 50KB
                        if (totalWritten % 51200 == 0) {
                            USBSerial.printf("[Audio] Progress: %d/%d bytes (%.1f%%)\n", 
                                            totalWritten, canon_pcm_len, 
                                            (float)totalWritten * 100.0 / canon_pcm_len);
                        }
                    } else {
                        USBSerial.println("[Audio] Warning: No bytes written, retrying...");
                        vTaskDelay(pdMS_TO_TICKS(10));
                    }
                    
                    // Check if user turned off alarm
                    if (!isAlarm) {
                        USBSerial.println("[Audio] Alarm stopped by user");
                        break;
                    }
                    
                    // Minimal delay for task yielding
                    vTaskDelay(pdMS_TO_TICKS(1));
                }
                
                USBSerial.printf("[Audio] Playback complete - Total written: %d bytes\n", totalWritten);
                USBSerial.println("[Control] Alarm:IDLE");
            }
            
            // Reset state after playback
            isPlaying = false;
            alarmUntil = 0; 
        }

      vTaskDelay(pdMS_TO_TICKS(50));
    }
}