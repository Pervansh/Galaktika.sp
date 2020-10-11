#ifndef _ORINTATION_UNIT_BLIP_H_
#define _ORINTATION_UNIT_BLIP_H_

#include <Kalman.h>
#include <MPU6050.h>

/*
 * Все типы (u)int(Z)_t заменены на алтернативы C++
 */

class OrintationUnit {
  protected:
    double offsetX;
    double offsetY;
    double offsetZ;
    float axesRotation;
    bool accelCorrection;
  public:
    OrintationUnit(float axesRotation = 0, bool accelCorrection = true);

    virtual void getMetrics6(double* pAngleX, double* pAngleY, double* pAngleZ, long* pAccX, long* pAccY, long* pAccZ) = 0;
    virtual void resetAngles() {}
    
    virtual void useAccelCorrection(bool accelCorrection);
    virtual void setOffsetXYZ(double offsetX, double offsetY, double offsetZ);
    virtual void setAxesRotation(float axesRotation);
    static void rotateAxes(double* x, double* y, float rotation);
};

#endif

/*class Reader {
  public:
    virtual void read(int16_t* ax, int16_t* ay, int16_t* az,
                      int16_t* gx, int16_t* gy, int16_t* gz) = 0;
};*/

/*class I2CReader : Reader {
  public:
    I2CReader() {
      Wire.begin();
      Wire.beginTransmission(IMU_ADDRESS);
      Wire.write(0x6B);
      Wire.write(0);
      Wire.endTransmission(true);
    }

    void read(int16_t* ax, int16_t* ay, int16_t* az,
              int16_t* gx, int16_t* gy, int16_t* gz) {
      Wire.beginTransmission(IMU_ADDRESS);
      Wire.write(0x3B);
      Wire.endTransmission(false);
      Wire.requestFrom(IMU_ADDRESS, 14, true);
      *ax = Wire.read() << 8 | Wire.read();
      *ay = Wire.read() << 8 | Wire.read();
      *az = Wire.read() << 8 | Wire.read();
      *tempRaw = Wire.read() << 8 | Wire.read();
      *gx = Wire.read() << 8 | Wire.read();
      *gy = Wire.read() << 8 | Wire.read();
      *gz = Wire.read() << 8 | Wire.read();
    }
};*/

/*class MPUClassReader : Reader {
  private:
    MPU6050 accelgyro;

  public:
    MPUClassReader() {
      Wire.begin();
      // join I2C bus (I2Cdev library doesn't do this automatically)
      #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
      #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
      #endif
      // initialize device
      Serial.println("Initializing I2C devices...");
      accelgyro.initialize();

      // verify connection
      Serial.println("Testing device connections...");
      Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    }

    void read(int16_t* ax, int16_t* ay, int16_t* az,
              int16_t* gx, int16_t* gy, int16_t* gz) {
      accelgyro.getMotion6(ax, ay, az, gx, gy, gz);
    }
};*/
