#pragma once

#include "OrintationUnit.h"
#include <Kalman.h>
#include <MPU6050.h>

/*
 * Все типы int(Z)_t заменены на алтернативы C++
 */


class MpuOrintationUnit : public OrintationUnit {
  private:
    const int IMU_ADDRESS = 0x68;
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


  public:
    MpuOrintationUnit(float axesRotation = 0, bool accelCorrection = true);

    void getMetrics6(double* pAngleX, double* pAngleY, double* pAngleZ, long* pAccX, long* pAccY, long* pAccZ);
    void resetAngles();
};
