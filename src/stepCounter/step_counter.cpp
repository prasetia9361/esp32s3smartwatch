#include "step_counter.h"

// Global instance pointer for event handling
StepCounter* g_stepCounter = nullptr;
IMUdata acc;

StepCounter::StepCounter() 
    : qmi(nullptr),
      stepCount(0), lastMagnitude(0), stepThreshold(1.8), 
      isActive(false), canDetect(true), timeBuff(0), 
      lastStepTime(0), soundEnabled(true), currentState(STATE_STOPPED) {
    g_stepCounter = this;
}

bool StepCounter::init(SensorQMI8658* sensor) {
    if (!sensor) {
        return false;
    }
    
    qmi = sensor;
    
    // label_steps = steps_label;
    // button_walk = walk_button;
    
    // Set up button event handler
    // lv_obj_add_event_cb(button_walk, buttonEventHandler, LV_EVENT_CLICKED, nullptr);
    
    // // Initialize button to unchecked state
    // lv_obj_clear_state(button_walk, LV_STATE_CHECKED);
    currentState = STATE_STOPPED;
    
    // Initialize UI
    updateUI();
    
    return true;
}

void StepCounter::start() {
    isActive = true;
    canDetect = true;
    lastStepTime = millis();
    // currentState = STATE_COUNTING;
    
    // Update button state to checked
    // lv_obj_add_state(button_walk, LV_STATE_CHECKED);
    
    Serial.println("Step counting started");
}

void StepCounter::stop() {
    isActive = false;
    // currentState = STATE_STOPPED;
    
    // Update button state to unchecked
    
    Serial.println("Step counting stopped");
    Serial.printf("Final step count: %d\n", stepCount);
}

void StepCounter::reset() {
    stepCount = 0;
    lastMagnitude = 0;
    canDetect = true;
    timeBuff = 0;
    lastStepTime = 0;
    isActive = false;
    // currentState = STATE_STOPPED;
    
    
    // Update UI
    updateUI();
    
    Serial.println("Step counter reset");
}

void StepCounter::update() {
    if (!qmi || !isActive) {
        Serial.println("qmi false");
    }else
    {
        /* code */

     
    
        // Reset detection cooldown
        if (!canDetect && millis() > timeBuff + STEP_COOLDOWN) {
            canDetect = true;
        }
        
        // Check for sensor data
        if (qmi->getDataReady()) {
            
            if (qmi->getAccelerometer(acc.x, acc.y, acc.z)) {
                // Calculate magnitude
                float magnitude = sqrt(acc.x * acc.x + acc.y * acc.y + acc.z * acc.z);
                
                // Detect step
                if (detectStep(magnitude)) {
                    stepCount++;
                    lastStepTime = millis();
                    canDetect = false;
                    timeBuff = millis();
                    
                    // Update UI
                    updateUI();
                    
                    // Play sound feedback
                    if (soundEnabled && stepCount < 30) {
                        playStepSound();
                    }
                    
                    Serial.printf("Step detected! Count: %d\n", stepCount);
                }
                
                lastMagnitude = magnitude;
            }
        }
        
        // Auto-stop if no steps detected for a while (optional)
        if (isActive && (millis() - lastStepTime) > MAX_STEP_INTERVAL * 30) {
            // Could implement auto-pause here if desired
        }
    }
}

bool StepCounter::detectStep(float magnitude) {
    // Step detection algorithm: threshold crossing with hysteresis
    return (lastMagnitude < stepThreshold && magnitude >= stepThreshold && canDetect);
}

void StepCounter::updateUI() {
    // if (label_steps) {
    //     char stepStr[16];
    //     snprintf(stepStr, sizeof(stepStr), "%d", stepCount);
    //     lv_label_set_text(label_steps, stepStr);
    // }
}

void StepCounter::playStepSound() {
    // Sound implementation would go here
    // For now, just a placeholder
    // Could implement beep functionality if audio system is available
}

void StepCounter::handleButton(int state) {
    if (currentState != state)
    {
        currentState = state;

        switch (currentState) {
            case STATE_STOPPED:
                stop();
                Serial.println("Button pressed: Stopping step counting");
                break;

                
            case STATE_COUNTING:

                reset();  // Reset count to 0
                start();  // Start counting
                Serial.println("Button pressed: Starting step counting");
                break;
        }
    }
}