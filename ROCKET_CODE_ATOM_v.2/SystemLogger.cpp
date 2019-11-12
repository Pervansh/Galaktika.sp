#include "BlipSystem.h"
#include "SystemLogger.h"
#include "Arduino.h"

SystemLogger::SystemLogger(BlipSystem* pBS, unsigned long _logDelta = 50) : pBlipSystem(pBS), logDelta(_logDelta) {
  randomSeed(analogRead(A7));
  fileName = "test_" + (String)random(0, 1000) + ".txt";
}

bool SystemLogger::start() {
  if (SD.begin(SD_CARD)) {
    file = SD.open("data.txt", FILE_WRITE);
    if (file) {
      file.println("\n\n\n>>>BLIP LOGGING SYSTEM<<<");
      file.println("-LOG NAME: '" + fileName + "'-\n");
      file.println("-# Time(s), Altitude(m), Velocity(m/s), Angles[x, y, z](grades), Servo[x, z](grades), State(num),,");
    } else {
      Serial.println("ERROR: SD-CARD CAN'T OPEN FILE");
    }
    startTimer = millis();
    return file;
  }
  Serial.println("ERROR: SD-CARD ISN'T OPEN");
  return false;
}

void SystemLogger::logInfo() {
  if (millis() - logTimer >= logDelta && file) {
    logTimer = millis();
    file.println((String)(millis() - startTimer) + ',' +
                 (String)pBlipSystem->getHeight() + ',' + '0' + ',' +
                 (String)pBlipSystem->getGyroX() + ',' +
                 (String)pBlipSystem->getGyroY() + ',' +
                 (String)pBlipSystem->getGyroZ() + ',' +
                 (String)pBlipSystem->getServoXPointer()->read() + ',' +
                 (String)pBlipSystem->getServoZPointer()->read() + ',' +
                 (String)pBlipSystem->getState() + ",,");
    /*file.print("\n\nLOG TIME: " + (String)(millis() - startTimer));
      file.print("\nSensors: [");
      file.print("BMP(m) : {" + (String)pBlipSystem->getHeight() + "}, ");
      file.print("IMU(x, y, z) : {" + (String)pBlipSystem->getGyroX() + ", " +
               (String)pBlipSystem->getGyroY() + ", " + (String)pBlipSystem->getGyroZ() + "}], ");
      file.print("\nServos(x, z): [" + (String)pBlipSystem->getServoXPointer()->read() + ", " + pBlipSystem->getServoZPointer()->read() + "], ");
      file.print("\nState(id): [" + (String)pBlipSystem->getState() + "]");*/
  }
}

void SystemLogger::logEvent(String eventName) {
  if (file) {
    file.println("LOG TIME: " + (String)(millis() - startTimer) +
                 ", EVENT: " + eventName + "!,,,,");
  }
}

void SystemLogger::finish() {
  file.close();
}
