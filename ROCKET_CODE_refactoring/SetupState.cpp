#include "BlipSystem.h"
#include "SetupState.h"
#include "ThrusterState.h"
#include "Arduino.h"

SetupState::SetupState(BlipSystem* pBlipSystem, int sbp) : BlipState(pBlipSystem, "SetupState"), safeSwitchPin(sbp) {
  setupState = 0;
  warmingTime = WARMING_TIME;
  calibrationTime = CALIBRATION_TIME;
}

void SetupState::execute() {
  switch (setupState) {
    case 0:
      if (digitalRead(safeSwitchPin) || true) {
        setupState = 1;
        setupTimer = millis();
        bool cls[3] = {false, true, true};
        pBlipSystem->setIndication(200, 2000, cls, 2000);
        Serial.println("SH: switch->" + (String)setupState);
      }
      break;
    case 1:
      delay(10);
      if (millis() - setupTimer >= warmingTime) {
        setupState = 2;
        bool cls[3] = {false, true, true};
        pBlipSystem->setIndication(200, 1000, cls, 1000);
        xSum = zSum = hSum = sumCnt = 0;
        setupTimer = millis();
        Serial.println("SH: switch->" + (String)setupState);
      }
      break;
    case 2:
      if (millis() - setupTimer >= calibrationTime) {
        double offsetX;
        double offsetZ;
        if (sumCnt == 0) {
          offsetX = 0;
          offsetZ = 0;
        } else {
          offsetX = xSum / sumCnt;
          offsetZ = zSum / sumCnt;
        }
        pBlipSystem->getImuUnitPointer()->setOffsetXYZ(offsetX, 0, offsetZ);
        pBlipSystem->setZeroHeight(hSum / sumCnt);
        setupState = 3;
        bool cls[3] = {false, true, false};
        pBlipSystem->setIndication(200, 250, cls, 250, 5, 5);
        Serial.println("SH: offsets(x, z, h): " + (String)offsetX + ", " + (String)offsetZ + ", " + (String)(hSum / sumCnt));
        Serial.println("SH: switch->" + (String)setupState);
      } else {
        xSum += pBlipSystem->getGyroX();
        zSum += pBlipSystem->getGyroZ();
        hSum += pBlipSystem->getHeight();
        sumCnt++;
      }
      delay(10);
      break;
    case 3:
      if (digitalRead(safeSwitchPin) || true) {
        bool cls[3] = {true, true, false};
        pBlipSystem->setIndication(200, 2000, cls, 2000);
        Serial.println("SH: changing system state!");
        pBlipSystem->getSystemLogger()->logEvent("WAITING START");
        pBlipSystem->getImuUnitPointer()->resetAngles();
        pBlipSystem->setState(MIDDLE_AIR);
        pBlipSystem->setBlipState(new ThrusterState(pBlipSystem));
        delay(500);
      }
      break;
  }
}

void SetupState::reset() {
  setupState = 0;
}
