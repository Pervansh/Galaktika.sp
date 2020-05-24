#include "OrintationUnit.h"
#include <Wire.h>
#include "Arduino.h"

OrintationUnit::OrintationUnit(float axesRotation = 0, bool accelCorrection = true) {
  Wire.begin();
  offsetX = 0;
  offsetY = 0;
  offsetZ = 0;
  this->accelCorrection = accelCorrection;
  this->axesRotation = axesRotation;
}

void OrintationUnit::useAccelCorrection(bool accelCorrection) {
  this->accelCorrection = accelCorrection;
}

void OrintationUnit::setOffsetXYZ(double offsetX, double offsetY, double offsetZ) {
  this->offsetX = offsetX;
  this->offsetY = offsetY;
  this->offsetZ = offsetZ;
}

void OrintationUnit::setAxesRotation(float axesRotation) {
  this->axesRotation = axesRotation;
}

void OrintationUnit::rotateAxes(double* x, double* y, float rotation) {
  *x = (*x) * cos(rotation * DEG_TO_RAD) - (*y) * sin(rotation * DEG_TO_RAD);
  *y = (*x) * sin(rotation * DEG_TO_RAD) + (*y) * cos(rotation * DEG_TO_RAD);
}
