#include "MpuOrintationUnit.h"
#include <Wire.h>
#include "Arduino.h"

MpuOrintationUnit::MpuOrintationUnit(float axesRotation = 0, bool accelCorrection = true)
: OrintationUnit(axesRotation, accelCorrection) {
  Wire.begin();
  kalmanX.setAngle(180); // Set starting angle
  kalmanY.setAngle(180);
  kalmanZ.setAngle(180);
  timer = micros();
  accelgyro.initialize();
  // accelgyro.dmpInitialize();
  accelgyro.setXGyroOffset(50);
  accelgyro.setZGyroOffset(50);
  Serial.println("Testing MPU6050 connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}

void MpuOrintationUnit::getMetrics6(double* pAngleX, double* pAngleY, double* pAngleZ, long* pAccX, long* pAccY, long* pAccZ) {
  int16_t accX, accY, accZ, gyroX, gyroY, gyroZ;
  accelgyro.getMotion6(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);
  /* Calculate the angls based on the different sensors and algorithm */
  double accYangle = (atan2(accX, accZ) + PI) * RAD_TO_DEG; // Angle calculate using the accelerometer
  double accXangle = (atan2(accY, accZ) + PI) * RAD_TO_DEG;
  double accZangle = (atan2(accX, accY) + PI) * RAD_TO_DEG;
  double gyroXrate = (double)gyroX / 131.0;
  double gyroYrate = -((double)gyroY / 131.0);
  double gyroZrate = ((double)gyroZ / 131.0);
  gyroXangle += kalmanX.getRate() * ((double)(micros() - timer) / 1000000); // Calculate gyro angle using the unbiased rate
  gyroYangle += kalmanY.getRate() * ((double)(micros() - timer) / 1000000);
  gyroZangle += kalmanZ.getRate() * ((double)(micros() - timer) / 1000000);
  if (accelCorrection) {
    kalAngleX = kalmanX.getAngle(accXangle, gyroXrate, (double)(micros() - timer) / 1000000); // Calculate the angle using a Kalman filter
    kalAngleY = kalmanY.getAngle(accYangle, gyroYrate, (double)(micros() - timer) / 1000000);
    kalAngleZ = kalmanZ.getAngle(accZangle, gyroZrate, (double)(micros() - timer) / 1000000);
  } else {
    kalAngleX += (gyroXrate - 3.2899) * (double)(micros() - timer) / 1000000;
    kalAngleY += gyroYrate * (double)(micros() - timer) / 1000000;
    kalAngleZ += (gyroZrate - 3.26347) * (double)(micros() - timer) / 1000000;
  }
  timer = micros();
  *pAngleX = (kalAngleX) * cos(axesRotation * DEG_TO_RAD) -
             (kalAngleZ) * sin(axesRotation * DEG_TO_RAD);
  *pAngleY = kalAngleY - offsetY;
  *pAngleZ = (kalAngleX) * sin(axesRotation * DEG_TO_RAD) +
             (kalAngleZ) * cos(axesRotation * DEG_TO_RAD);
  *pAngleX = fmod(*pAngleX + 540 - offsetX, 360) - 180;
  *pAngleZ = fmod(*pAngleZ + 540 - offsetZ, 360) - 180;
  /*
  *pAngleX = (kalAngleX) * cos(axesRotation * DEG_TO_RAD) -
             (kalAngleZ) * sin(axesRotation * DEG_TO_RAD) - offsetX;
  *pAngleY = kalAngleY - offsetY;
  *pAngleZ = (kalAngleX) * sin(axesRotation * DEG_TO_RAD) +
             (kalAngleZ) * cos(axesRotation * DEG_TO_RAD) - offsetZ;
  */
  *pAccX = accX;
  *pAccY = accY;
  *pAccZ = accZ;
  // The accelerometer's maximum samples rate is 1kHz
}

void MpuOrintationUnit::resetAngles() {
  kalAngleX = 0;
  kalAngleY = 0;
  kalAngleZ = 0;
  Serial.println("Agles were reseted");
}
