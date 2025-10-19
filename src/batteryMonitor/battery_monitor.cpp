#include "battery_monitor.h"


BatteryMonitor::BatteryMonitor() 
    : power(nullptr), lastUpdate(0) {
}

bool BatteryMonitor::init(XPowersPMU* pmu){
    if (!pmu )
    {
        return false;
    }
    power = pmu;
    enableADC();
    return true;
}

void BatteryMonitor::update() {
    unsigned long currentTime = millis();
    
    // Update every UPDATE_INTERVAL milliseconds
    if (currentTime - lastUpdate >= UPDATE_INTERVAL) {
        lastUpdate = currentTime;
        
        if (!power) return;
        
        int batteryPercent = power->getBatteryPercent();
        bool charging = power->isCharging();
        
        // Ensure valid range
        if (batteryPercent < 0) batteryPercent = 0;
        if (batteryPercent > 100) batteryPercent = 100;
        
        // Update arc value (0-100)
        // lv_arc_set_value(arc_battery, batteryPercent);
        
        // Update label with just the number (no % symbol)
        char batteryStr[4];
        snprintf(batteryStr, sizeof(batteryStr), "%d", batteryPercent);
        // lv_label_set_text(label_battery, batteryStr);
        
        // Update battery icon based on charging status
        // if (battery_icon) {
        //     if (charging) {
        //         // Set to focused state to show charging icon
        //         lv_obj_add_state(battery_icon, LV_STATE_FOCUSED);
        //     } else {
        //         // Clear focused state to show default battery icon
        //         lv_obj_clear_state(battery_icon, LV_STATE_FOCUSED);
        //     }
        // }
    }
}

int BatteryMonitor::getBatteryPercentage() {
    if (!power) return 0;
    return power->getBatteryPercent();
}

bool BatteryMonitor::isCharging() {
    if (!power) return false;
    return power->isCharging();
}

void BatteryMonitor::enableADC() {
    if (!power) return;
    
    power->enableTemperatureMeasure();
    power->enableBattDetection();
    power->enableVbusVoltageMeasure();
    power->enableBattVoltageMeasure();
    power->enableSystemVoltageMeasure();
}

void BatteryMonitor::disableADC() {
    if (!power) return;
    
    power->disableTemperatureMeasure();
    power->disableBattDetection();
    power->disableVbusVoltageMeasure();
    power->disableBattVoltageMeasure();
    power->disableSystemVoltageMeasure();
}