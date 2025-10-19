#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <Arduino.h>
#include "XPowersLib.h"

class BatteryMonitor {
private:
    XPowersPMU* power;
    // lv_obj_t* arc_battery;
    // lv_obj_t* label_battery;
    // lv_obj_t* battery_icon;
    unsigned long lastUpdate;
    const unsigned long UPDATE_INTERVAL = 5000; // Update every 5 seconds
    
public:
    BatteryMonitor();
    
    // Initialize with power management unit and UI objects
    // bool init(XPowersPMU* pmu, lv_obj_t* battery_arc, lv_obj_t* battery_label, lv_obj_t* battery_icon_obj);
    bool init(XPowersPMU* pmu);
    // Update battery display
    void update();
    
    // Get current battery percentage
    int getBatteryPercentage();
    
    // Check if charging
    bool isCharging();
    
    // Enable/disable ADC for battery monitoring
    void enableADC();
    void disableADC();
};

#endif // BATTERY_MONITOR_H