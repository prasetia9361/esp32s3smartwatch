#ifndef QMI8658_SENSOR_H
#define QMI8658_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include "SensorQMI8658.hpp"
#include "pin_config.h"
#include "HWCDC.h"

extern HWCDC USBSerial;

typedef struct {
  float x;
  float y;
  float z;
} SensorData;

extern SensorData acc_data;
extern SensorData gyr_data;

bool qmi_init();
void qmi_read_data();

#endif