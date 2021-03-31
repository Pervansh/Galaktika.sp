#pragma once

#include "Arduino.h"
#include "OrintationUnit.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <inttypes.h>

class BnoOrintationUnit : public OrintationUnit {
private:
    uint8_t address;
    Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29);
public:
    BnoOrintationUnit(uint8_t address = 0x29, float axesRotation = 0, bool accelCorrection = true);

    void getMetrics6(double* pAngleX, double* pAngleY, double* pAngleZ, long* pAccX, long* pAccY, long* pAccZ);
};
