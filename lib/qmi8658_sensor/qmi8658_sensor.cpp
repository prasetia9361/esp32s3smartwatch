#include "qmi8658_sensor.h"

SensorQMI8658 qmi;
SensorData acc_data;
SensorData gyr_data;

bool qmi_init() {
  if (!qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, IIC_SDA, IIC_SCL)) {
    USBSerial.println("Failed to find QMI8658 - check your wiring!");
    return false;
  }

  qmi.configAccelerometer(SensorQMI8658::ACC_RANGE_4G, SensorQMI8658::ACC_ODR_1000Hz, SensorQMI8658::LPF_MODE_0);
  qmi.enableAccelerometer();

  qmi.configGyroscope(SensorQMI8658::GYR_RANGE_128DPS, SensorQMI8658::GYR_ODR_112_1Hz, SensorQMI8658::LPF_MODE_0);
  qmi.enableGyroscope();

  USBSerial.println("QMI8658 initialized successfully");
  return true;
}

void qmi_read_data() {
  if (qmi.getDataReady()) {
    if (qmi.getAccelerometer(acc_data.x, acc_data.y, acc_data.z)) {
      USBSerial.print("{ACCEL: ");
      USBSerial.print(acc_data.x);
      USBSerial.print(",");
      USBSerial.print(acc_data.y);
      USBSerial.print(",");
      USBSerial.print(acc_data.z);
      USBSerial.println("}");
    }

    if (qmi.getGyroscope(gyr_data.x, gyr_data.y, gyr_data.z)) {
      USBSerial.print("{GYRO: ");
      USBSerial.print(gyr_data.x);
      USBSerial.print(",");
      USBSerial.print(gyr_data.y);
      USBSerial.print(",");
      USBSerial.print(gyr_data.z);
      USBSerial.println("}");
    }
  }
}