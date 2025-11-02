#include <math.h>
#include <Wire.h>

#include "config.h"
#include "i2sConfig.h"
#include "lvgl_setup.h"
// #include "communication/communication.h"
// #include "controll/contoll.h"
#include "wifiSetup/wifi_setup.h"
// #include "rtcTime/rtc_time.h"
#include "SensorPCF85063.hpp"
#include "ntpSetup/ntp_setup.h"
#include "batteryMonitor/battery_monitor.h"
#include "SensorQMI8658.hpp"
#include "stepCounter/step_counter.h"
#include "storage/storage.h"
#include "memory/memory.h"
#include "clientServer/clientServer.h"
#include "speaker.h"
// #include "webapp/web.h"
#include "pin_config.h"
#include "ui.h"
#include "vars.h"


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

speaker *mOutput;

void applyWiFiMode(void *param);
void applySensor(void *param);
// --- MODIFIKASI: Deklarasi fungsi untuk task alarm baru ---
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
unsigned long lastStatusPrint = 0;

// --- MODIFIKASI: Pindahkan variabel alarm ke scope global agar bisa diakses oleh task alarm ---
unsigned long alarmUntil = 0; // millis timestamp until which alarm stays active
String HH_MM = "21:00";
String CURRENT_DATE = "TUE 18/10/2025";

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
    if (start == end)
        return true; // full day
    if (isNtp)
    {
      struct tm timeinfo;
      if (!ntpSetup.getLocalTime(&timeinfo)) {
          return false;
      }
      current = timeinfo.tm_hour * 60 + timeinfo.tm_min;
    }else
    {
      RTC_DateTime now = rtc.getDateTime();
      current = now.hour * 60 + now.minute;
    }
    if (start < end)
      return current >= start && current < end;
    // crosses midnight
    return current >= start || current < end;
}

// --- MODIFIKASI: Fungsi controlAlarm diubah menjadi task mandiri ---
void controlAlarmTask(void *param) {
    bool isPlaying = false;

    while (true) {
        // Cek apakah ada sinyal alarm BARU yang masuk
        if (isAlarmActive) {
            // 1. Langsung reset flag setelah sinyal diterima.
            // Ini memastikan kita hanya memproses pemicu ini satu kali.
            isAlarmActive = false; 

            // 2. Hanya perpanjang alarm jika sedang tidak diputar.
            // Jika sedang diputar, biarkan saja selesai.
            if (!isPlaying) {
                alarmUntil = millis() + 6000UL;
            }
        }

        bool shouldBePlaying = (long)(millis() - alarmUntil) < 0;

        if (shouldBePlaying && !isPlaying) {
            // --- ALARM MULAI AKTIF ---
            isPlaying = true;
            USBSerial.printf("[Control] Window:ON Alarm:TRIGGER Start:%s End:%s\n",
                STORAGE.getStartTime(), STORAGE.getEndTime());
            
            mOutput->startSpeaker(); 

            File audioFile = memory->openFile("/audio2.wav");
            if (audioFile && !audioFile.isDirectory()) {
                const size_t bufferSize = 512;
                uint8_t buffer[bufferSize];
                size_t bytesRead;

                audioFile.seek(44); // Lewati header WAV

                // Loop pemutaran sampai file selesai, BUKAN berdasarkan alarmUntil
                while (audioFile.available()) {
                    bytesRead = audioFile.read(buffer, bufferSize);
                    for (size_t i = 0; i < bytesRead; i++) {
                      USBSerial.printf("%02X ", buffer[i]);
                    }
                    if (bytesRead > 0) {
                      USBSerial.printf("[Control] Playing audio chunk of size: %d bytes\n", bytesRead);
                    } else {
                        break; // Akhir file
                    }
                }
                audioFile.close();
            } else {
                USBSerial.println("Failed to open /audio2.wav");
            }

            mOutput->stopAudio();
            USBSerial.println("[Control] Alarm:IDLE");
            
            // 3. Reset state setelah selesai
            isPlaying = false;
            alarmUntil = 0; 
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}


void setup() {
  USBSerial.begin(115200);
  delay(1000);

  // --- PERBAIKAN: Pindahkan inisialisasi objek ke atas ---
  wifiSetup = new WifiSetup();
  memory = new Memory();
  webServer = new clientServer(memory);
  stepCounter = new StepCounter();
  battery = new BatteryMonitor();
  mOutput = new speaker(i2sPort,i2sPins,i2s_speaker_config,256);
  samples = (int16_t *)malloc(sizeof(int16_t) * BYTE_RATE);
  // ---------------------------------------------------------

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
      4096,            
      NULL,            
      1,               
      &sensorTaskHandle,
      1); 

  // --- MODIFIKASI: Buat task baru untuk kontrol alarm ---
  TaskHandle_t alarmTaskHandle;
  xTaskCreatePinnedToCore(
      controlAlarmTask,
      "alarmTask",
      8192,             // Stack mungkin perlu besar untuk file I/O
      NULL,
      2,                // Prioritas sama dengan WiFi
      &alarmTaskHandle,
      1);               // Bisa di core 1 juga
}

void loop() {
  vTaskDelay(5);
}

void applyWiFiMode(void *param){
  bool wifiAP = false;

  if (!STORAGE.init())
  {
      USBSerial.println(F("[Storage] init failed"));
  }

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

  if (!battery->init(&power)) {
      USBSerial.println("Battery monitor initialization failed!");
  } else {
      USBSerial.println("Battery monitor initialized");
  }
  
  if (!rtc.begin(Wire, PCF85063_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
    USBSerial.println("Failed to find PCF8563 - check your wiring!");
  } else {
    USBSerial.println("Success to find PCF8563");
    // Set waktu default sekali saja
    rtc.setDateTime(2025, 10, 18, 0, 0, 0);
  }

  bool qmiResult = sensorStepper.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  if (!qmiResult) {
    USBSerial.println("QMI8658 initialization failed!");
  } else {
    sensorStepper.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, 
                          SensorQMI8658::ACC_ODR_1000Hz, 
                          SensorQMI8658::LPF_MODE_0);
    sensorStepper.enableAccelerometer();
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

    isWifiAP = switchWiFiAP();
    isWifi = switchWiFi();

    // --- MODIFIKASI: Logika pemicu alarm ---
    if (isWithinSchedule(STORAGE.getStartTime(), STORAGE.getEndTime()) && stepDetected) {
        // Jika ada gerakan dalam window aktif, kirim sinyal ke task alarm
        isAlarmActive = true;
    } else {
        // 4. Tambahkan else untuk memastikan flag tidak 'macet' jika kondisi tidak terpenuhi lagi
        // (Meskipun sudah di-handle di alarm task, ini adalah praktik yang baik)
        // isAlarmActive = false; // Baris ini opsional karena sudah di-handle di alarm task
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
          lastDay = currentDay;
        }
      }
    }

    // --- MODIFIKASI: Pindahkan pemanggilan controlAlarm() ke task terpisah ---
    // controlAlarm(); // HAPUS PANGGILAN INI
    
    vTaskDelay(5);
  }
}