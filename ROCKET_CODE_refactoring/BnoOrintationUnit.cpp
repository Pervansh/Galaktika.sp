#include "BnoOrintationUnit.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

BnoOrintationUnit::BnoOrintationUnit(uint8_t address = 0x29, float axesRotation, bool accelCorrection = true)
: OrintationUnit(axesRotation, accelCorrection) {
    this->address = address;
    bno = Adafruit_BNO055(55, address);
    Serial.println("Orientation Sensor Test"); Serial.println("");
    if (!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    }
}

void BnoOrintationUnit::getMetrics6(double* pAngleX, double* pAngleY, double* pAngleZ, long* pAccX, long* pAccY, long* pAccZ) {
    sensors_event_t event; 
    bno.getEvent(&event);
    *pAccX = event.acceleration.x;
    *pAccY = event.acceleration.y;
    *pAccZ = event.acceleration.z;
    *pAngleX = event.orientation.x - offsetX;
    *pAngleY = event.orientation.y - offsetY;
    *pAngleZ = event.orientation.z - offsetZ;
    OrintationUnit::rotateAxes(pAngleX, pAngleZ, axesRotation);
}
