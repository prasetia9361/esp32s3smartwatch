#ifndef STEP_COUNTER_H
#define STEP_COUNTER_H

#include <Arduino.h>
// #include "lvgl.h"
#include "SensorQMI8658.hpp"
// #include "qmi8658_sensor.h"
#include <math.h>

class StepCounter {
private:
    SensorQMI8658* qmi;
    // lv_obj_t* label_steps;
    // lv_obj_t* button_walk;
    
    // Step counting variables
    int stepCount;
    float lastMagnitude;
    float stepThreshold;
    bool isActive;
    bool canDetect;
    unsigned long timeBuff;
    unsigned long lastStepTime;
    
    // Sound settings (optional)
    bool soundEnabled;
    
    // State management
    enum CounterState {
        STATE_STOPPED,    // Initial state and after stop
        STATE_COUNTING    // Currently counting steps
    };
    int currentState;
    
    // Detection parameters
    const unsigned long STEP_COOLDOWN = 100; // milliseconds between step detections
    const unsigned long MAX_STEP_INTERVAL = 2000; // max time between steps in ms
    
public:
    StepCounter();
    
    // Initialize with sensor and UI objects
    // bool init(SensorQMI8658* sensor, lv_obj_t* steps_label, lv_obj_t* walk_button);
    bool init(SensorQMI8658* sensor);
    
    // Start/stop step counting
    void start();
    void stop();
    void reset();
    
    // Update step detection (call in main loop)
    void update();
    
    // Get current step count
    int getStepCount() const { return stepCount; }
    
    // Check if currently counting
    bool isCountingActive() const { return isActive; }
    
    // Set step detection threshold
    void setThreshold(float threshold) { stepThreshold = threshold; }
    
    // Enable/disable sound feedback
    void setSoundEnabled(bool enabled) { soundEnabled = enabled; }
    
    void handleButton(int state);
    // Button event handler
    // static void buttonEventHandler(lv_event_t* e);
    
private:
    // Internal step detection logic
    bool detectStep(float magnitude);
    void updateUI();
    void playStepSound();
};

#endif // STEP_COUNTER_H