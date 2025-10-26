#include <Arduino.h>
#include <math.h>
#include <Wire.h>

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
// #include "webapp/web.h"
#include "pin_config.h"
#include "ui.h"
#include "vars.h"

HWCDC USBSerial;
WiFiSetup wifiSetup(WIFI_SSID, WIFI_PASSWORD, WIFI_FALLBACK_SSID, WIFI_FALLBACK_PASSWORD);
NTPSetup ntpSetup(NTP_TIMEZONE, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
SensorPCF85063 rtc;

XPowersPMU power;
BatteryMonitor battery;

SensorQMI8658 sensorStepper;
StepCounter stepCounter;

void applyWiFiMode(void *param);
void applySensor(void *param);

extern bool switchWiFiAP();
extern bool switchWiFi();
extern bool saveDate();
extern void chargeState(bool isCharge);
extern void setTimeHHMM(const char* hhmm);
extern void setCurrentDate(const char* datetime);
extern void setBT(int32_t persent);
extern void setStep(int32_t step);
extern uint16_t *getDateTime();
extern int32_t getBatteryPersentage();
extern int isStep();


int lastDay = -1;
int currentDay = -1;
int isNtp = false;
unsigned long lastTimeUpdate = 0;
const unsigned long TIME_UPDATE_INTERVAL = 1000; // Update every second
bool wifiAP = false;
bool isWifiAP = false;
bool isWifi = false;
bool wifi = false;
bool lastSaveDate = false;
String HH_MM = "21:00";
String CURRENT_DATE = "TUE 18/10/2025";

// static Communication comm; 
// static Control control;

bool initializeHardware(){
  Wire.begin(IIC_SDA,IIC_SCL);
  bool powerResult = power.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  if (!powerResult)
  {
      USBSerial.println("PMU initialization failed!");
      return false;
  }

  // Configure power management
  power.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
  power.setChargeTargetVoltage(3);
  power.clearIrqStatus();
  power.enableIRQ(XPOWERS_AXP2101_PKEY_SHORT_IRQ);

  bool qmiResult = sensorStepper.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);
  if (!qmiResult) {
    USBSerial.println("QMI8658 initialization failed!");
    return false;
  }
  
  // Configure accelerometer
  sensorStepper.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, 
                          SensorQMI8658::ACC_ODR_1000Hz, 
                          SensorQMI8658::LPF_MODE_0);
  sensorStepper.enableAccelerometer();

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
        // if (hour12 == 0) {
        //     hour12 = 12; // 12 AM
        // } else if (hour12 > 12) {
        //     hour12 -= 12; // PM hours
        // }
        // update label di lvgl
        // Update hour label (12-hour format)
        // char hourStr[3];
        // snprintf(hourStr, sizeof(hourStr), "%02d", hour12);
        // // lv_label_set_text(objects.label_hh, hourStr);
        
        // // Update minute label
        // char minStr[3];
        // snprintf(minStr, sizeof(minStr), "%02d", timeinfo.tm_min);
        // lv_label_set_text(objects.label_mm, minStr);

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
                timeinfo.tm_mon, 
                timeinfo.tm_year);
        CURRENT_DATE = String(dateStr);
        
        // snprintf(dateStr, sizeof(dateStr), "%s - %d", 
        //         weekdays[timeinfo.tm_wday], 
        //         timeinfo.tm_mday);
        // lv_label_set_text(objects.label_date, dateStr);
        
        // USBSerial.printf("Time updated: %s %02d:%02d\n", 
        //                 weekdays[timeinfo.tm_wday], hour12, timeinfo.tm_min);
    }
}

void setup() {
  USBSerial.begin(115200);
  delay(2000);



  lvgl_init();
  USBSerial.println("LVGL initialized");
  ui_init();
  USBSerial.println("UI initialized");

    // Initialize hardware
  while (!initializeHardware()) {
      USBSerial.println("Hardware initialization failed!");
      delay(100);
  }
  
  // if (!RTC_TIME.begin(PCF85063_SLAVE_ADDRESS, IIC_SDA, IIC_SCL))
  // {
    
  //   USBSerial.println("RTC fail initialized");
  // }
  // USBSerial.println("RTC initialized");
  // RTC_TIME.iso8601();

  TaskHandle_t wifiTaskHandle;
  xTaskCreatePinnedToCore(
      applyWiFiMode,   /* Function to implement the task */
      "WiFiTask",     /* Name of the task */
      4096,           /* Stack size in words */
      NULL,           /* Task input parameter */
      1,              /* Priority of the task */
      &wifiTaskHandle,/* Task handle. */
      0);             /* Core where the task should run */ 

  TaskHandle_t sensorTaskHandle;
  xTaskCreatePinnedToCore(
      applySensor,   /* Function to implement the task */
      "sensorTask",     /* Name of the task */
      4096,           /* Stack size in words */
      NULL,           /* Task input parameter */
      2,              /* Priority of the task */
      &sensorTaskHandle,/* Task handle. */
      1);             /* Core where the task should run */
}

void loop() {
  lvgl_handler();
  ui_tick();
  vTaskDelay(5);
}

void applyWiFiMode(void *param){
  bool wifiAP = false;
  // if (!RTC_TIME.begin(PCF85063_SLAVE_ADDRESS, IIC_SDA, IIC_SCL))
  // {
    
  // USBSerial.println("RTC fail initialized");
  // }
  // USBSerial.println("RTC initialized");
  // RTC_TIME.iso8601();

  if (!STORAGE.init())
  {
      Serial.println(F("[Storage] init failed"));
  }
  // wifiSetup.connect();
  
  // // Initialize NTP if WiFi is connected
  // if (wifiSetup.isConnected()) {
  //     // Set custom update interval if needed
  //     ntpSetup.setUpdateInterval(NTP_UPDATE_INTERVAL);
  //     ntpSetup.initialize();
  //     isNtp = true;
  // } else {
  //     USBSerial.println("WiFi not connected - NTP initialization skipped");
  //     isNtp = false;
  // }
  while (true)
  {
    if (isWifiAP != wifiAP)
    {
      wifiAP = isWifiAP;

      if (wifiAP){
        // comm.init();
        // control.init();
        // static Webapp web; // local static so it persists
        // web.init();
      }
      else{
        // comm.stop();
      }
    }

    if (isWifi != wifi)
    {
      wifi = isWifi;

      if (wifi)
      {
        wifiSetup.connect();
        if (wifiSetup.isConnected()) {
            // Set custom update interval if needed
            ntpSetup.setUpdateInterval(NTP_UPDATE_INTERVAL);
            ntpSetup.initialize();
            isNtp = true;
        } else {
            USBSerial.println("WiFi not connected - NTP initialization skipped");
            isNtp = false;
        }
      }else
      {
        wifiSetup.disconnect();
        isNtp = false;
      }
    }
    
    
    if (isNtp)
    {
      ntpSetup.update();
      // USBSerial.println(isWifiAP);
      
      unsigned long currentTime = millis();
      if (currentTime - lastTimeUpdate >= TIME_UPDATE_INTERVAL) {
          updateTimeDisplay();
          lastTimeUpdate = currentTime;
      }
    }

    // static uint32_t lastCheck = 0;
    // if (millis() - lastCheck > 1000){
    //   lastCheck = millis();
    //   HH_MM = RTC_TIME.printTime();
    //   USBSerial.println(HH_MM);
    // }

    // comm.loop();
    // control.loop();

    vTaskDelay(10);
    
  }
}

void applySensor(void *param){


  if (!battery.init(&power))
  {
      USBSerial.println("Battery monitor initialization failed!");
  } else {
      USBSerial.println("Battery monitor initialized");
  }
  
  
  if (!rtc.begin(Wire, PCF85063_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
    USBSerial.println("Failed to find PCF8563 - check your wiring!");
  }else
  {
    USBSerial.println("Success to find PCF8563");
  }

  if (!stepCounter.init(&sensorStepper)) {
      USBSerial.println("Step counter initialization failed!");
  } else {
      USBSerial.println("Step counter initialized");
      // Set custom threshold if needed
      stepCounter.setThreshold(1.8);
      stepCounter.setSoundEnabled(true);
  }

  uint16_t year = 2025;
  uint8_t month = 10;
  uint8_t day = 18;
  uint8_t hour = 0;
  uint8_t minute = 0;
  uint8_t second = 0;

  rtc.setDateTime(year,month,day, hour, minute, second);



  
  while (true)
  {
    stepCounter.handleButton(isStep());
    battery.update();
    stepCounter.update();

    chargeState(battery.isCharging());
    setBT(battery.getBatteryPercentage());
    setStep(stepCounter.getStepCount());

    isWifiAP = switchWiFiAP();
    isWifi = switchWiFi();
    if (isNtp)
    {
      static uint32_t lastCheck = 0;
      if (millis() - lastCheck > 10000){
        lastCheck = millis();
        setTimeHHMM((HH_MM).c_str());
      }

      if (currentDay != lastDay)
      {
        setCurrentDate((CURRENT_DATE).c_str());
        lastDay = currentDay;
      }
    }else
    {

      if (saveDate() != lastSaveDate)
      {
        lastSaveDate = saveDate();
        if (lastSaveDate)
        {
          uint16_t *dt = getDateTime(); 
          rtc.setDateTime(dt[0],dt[1] + 1,dt[2],dt[3],dt[4],dt[5]);
        }
      }
      

      static uint32_t lastCheck = 0;
      if (millis() - lastCheck > 10000){
        lastCheck = millis();
        RTC_DateTime dt = rtc.getDateTime();
        char buf[7]; // "HH:MM" + null
        snprintf(buf, sizeof(buf), "%02d:%02d", dt.hour, dt.minute);
        // RTCHHMM = RTC_TIME.printTime();
        setTimeHHMM(buf);
        // USBSerial.println(HH_MM);

        // char bufDate[11]; // "DD/MM/YYYY" + null
        // snprintf(bufDate, sizeof(bufDate), "%02d/%02d/%04d", dt.day, dt.month, dt.year);
        // RTCDATE = RTC_TIME.printDate();
        currentDay = dt.week;
        const char* weekdays[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
        char dateStr[18];
        snprintf(dateStr, sizeof(dateStr), "%s %02d/%02d/%04d",
                weekdays[currentDay], 
                dt.day, 
                dt.month, 
                dt.year);
        if (currentDay != lastDay)
        {
          setCurrentDate(dateStr);
          lastDay = currentDay;
        }
        
      }

    }

    uint32_t status = power.getIrqStatus();
    if (power.isPekeyShortPressIrq())
    {
      battery.disableADC();
      power.shutdown();
    }
    power.clearIrqStatus();
    
  vTaskDelay(5);
  }
  
}