#include "MpuOrintationUnit.h"

#include <Wire.h>
#include "Arduino.h"

#include "BlipSystem.h"
#include "BlipSubscriber.h"
#include "BlipRangeSigner.h"

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
  // Serial.println("Testing MPU6050 connections...");
  // Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}

void MpuOrintationUnit::getMetrics6(double* pAngleX, double* pAngleY, double* pAngleZ, long* pAccX, long* pAccY, long* pAccZ) {
  accelgyro.getMotion6(&rawAccX, &rawAccY, &rawAccZ, &rawGyroX, &rawGyroY, &rawGyroZ);
  int16_t accX = rawAccX;
  int16_t accY = rawAccY;
  int16_t accZ = rawAccZ;
  int16_t gyroX = rawGyroX - gyroErrorX;
  int16_t gyroY = rawGyroY - gyroErrorY;
  int16_t gyroZ = rawGyroZ - gyroErrorZ;
  /*
  Serial.print("GYRO(x, y, z):\t");
  Serial.print(gyroErrorX);
  Serial.print("\t");
  Serial.print(gyroErrorY);
  Serial.print("\t");
  Serial.print(gyroErrorZ);
  Serial.print("\n");
  */
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
    kalAngleX += gyroXrate * (double)(micros() - timer) / 1000000;
    kalAngleY += gyroYrate * (double)(micros() - timer) / 1000000;
    kalAngleZ += gyroZrate * (double)(micros() - timer) / 1000000;
    /*Serial.println(kalAngleX);
    Serial.println(kalAngleY);
    Serial.println(kalAngleZ);
    Serial.println(gyroXrate * (double)(micros() - timer) / 1000000);
    Serial.println(gyroYrate * (double)(micros() - timer) / 1000000);
    Serial.println(gyroZrate * (double)(micros() - timer) / 1000000);*/
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
  kalAngleX = offsetX * cos(axesRotation * DEG_TO_RAD) + offsetZ * sin(axesRotation * DEG_TO_RAD);
  kalAngleY = offsetY;
  kalAngleZ = offsetZ * cos(axesRotation * DEG_TO_RAD) - offsetX * sin(axesRotation * DEG_TO_RAD);
  Serial.println("Agles were reseted");
  Serial.println(kalAngleX);
  Serial.println(kalAngleY);
  Serial.println(kalAngleZ);
  // delay(3000);
}

void MpuOrintationUnit::useRawGyroCorrection(BlipSystem* system, const String& startStateId, const String& endStateId) {
  MpuRawGyroCorrection* correction = new MpuRawGyroCorrection(this, endStateId);
  BlipRangeSigner* signer = new BlipRangeSigner(correction, BlipEventTypes::onExecution, startStateId, endStateId);
  signer->subscriberNeedDeleting(true);
  signer->init(system);
}

MpuRawGyroCorrection::MpuRawGyroCorrection(MpuOrintationUnit* mpuUnit, const String& _endStateId) 
: unit(mpuUnit) {}

void MpuRawGyroCorrection::update(BlipSystem* system, const String& event) {
  Serial.println("Summing");
  sumX += unit->getRawGyroX();
  sumY += unit->getRawGyroY();
  sumZ += unit->getRawGyroZ();
  cnt++;
  unit->setGyroErrorX(sumX / cnt);
  unit->setGyroErrorY(sumY / cnt);
  unit->setGyroErrorZ(sumZ / cnt);
}
