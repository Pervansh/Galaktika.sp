#pragma once

#include "OrintationUnit.h"
#include <Kalman.h>
#include <MPU6050.h>
#include "BlipSubscriber.h"
#include "Arduino.h"

/*
 * Все типы int(Z)_t заменены на алтернативы C++
 */


class MpuOrintationUnit : public OrintationUnit {
  private:
    const int IMU_ADDRESS = 0x68;

    int16_t rawAccX = 0;
    int16_t rawAccY = 0;
    int16_t rawAccZ = 0;
    int16_t rawGyroX = 0;
    int16_t rawGyroY = 0;
    int16_t rawGyroZ = 0;
    
    Kalman kalmanX;
    Kalman kalmanY;
    Kalman kalmanZ;
    /* IMU Data */
    double temp;
    double gyroXangle = 180; // Angle calculate using the gyro
    double gyroYangle = 180;
    double gyroZangle = 180;
    double compAngleX = 180; // Calculate the angle using a Kalman filter
    double compAngleY = 180;
    double compAngleZ = 180;
    double kalAngleX = 0; // Calculate the angle using a Kalman filter
    double kalAngleY = 0;
    double kalAngleZ = 0;
    unsigned long timer;
    MPU6050 accelgyro;

    int16_t gyroErrorX = 0;
    int16_t gyroErrorY = 0;
    int16_t gyroErrorZ = 0;

  public:
    MpuOrintationUnit(float axesRotation = 0, bool accelCorrection = true);

    void getMetrics6(double* pAngleX, double* pAngleY, double* pAngleZ, long* pAccX, long* pAccY, long* pAccZ);
    void resetAngles();

    void useRawGyroCorrection(BlipSystem* system, const String& startStateId, const String& endStateId);

    int16_t getRawGyroX() { return rawGyroX; }
    int16_t getRawGyroY() { return rawGyroY; }
    int16_t getRawGyroZ() { return rawGyroZ; }
    int16_t getRawAccX() { return rawAccX; }
    int16_t getRawAccY() { return rawAccY; }
    int16_t getRawAccZ() { return rawAccZ; }

    void setGyroErrorX(int16_t error) { gyroErrorX = error; }
    void setGyroErrorY(int16_t error) { gyroErrorY = error; }
    void setGyroErrorZ(int16_t error) { gyroErrorZ = error; }
    int16_t getGyroErrorX() { return gyroErrorX; }
    int16_t getGyroErrorY() { return gyroErrorY; }
    int16_t getGyroErrorZ() { return gyroErrorZ; }
};

class MpuRawGyroCorrection : public BlipSubscriber {
  private:
    MpuOrintationUnit* unit;
    int64_t sumX = 0;
    int64_t sumY = 0;
    int64_t sumZ = 0;
    uint64_t cnt = 0;
    // typedef bool (*breakFuncType)(BlipSystem*, const String&);
    // breakFuncType breakFunc;

  public:
    // MpuRawGyroCorrection(MpuOrintationUnit* mpuUnit, breakFuncType condition);
    MpuRawGyroCorrection(MpuOrintationUnit* mpuUnit, const String& _endStateId);

    void update(BlipSystem* system, const String& event);
};
