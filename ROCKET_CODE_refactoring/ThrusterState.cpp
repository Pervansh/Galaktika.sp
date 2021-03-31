#include "ThrusterState.h"
#include "Arduino.h"
#include "config.h"

ThrusterState::ThrusterState(BlipSystem* pBlipSystem, BlipState* nextState)
    : BlipState(pBlipSystem, "ThrusterState", nextState) {
    timer = millis();
    isInAir = false;
    pin = 23;
    /*
    pinMode(pin, OUTPUT);
    */
    if (pBlipSystem->getSystemLogger()->start()) {
        Serial.println("Logger is started");
    } else {
        Serial.println("Logger isn't started");
    }
    servoX = pBlipSystem->getServoXPointer();
    servoZ = pBlipSystem->getServoZPointer();
}

void ThrusterState::init() {
    pBlipSystem->getImuUnitPointer()->useAccelCorrection(false);
    bool cls[3] = {true, false, false};
    pBlipSystem->setIndication(500, 0, cls, 100);
    pBlipSystem->getSystemLogger()->logEvent("ROCKET IN AIR");
}

void ThrusterState::execute() {
    /*
    if (5000 <= millis() - timer && millis() - timer < 6000) {
        if (isInAir) {
        #ifdef USE_THRUSTER
            digitalWrite(pin, HIGH);
        #endif
        } else {
            bool cls[3] = {false, false, true};
            pBlipSystem->setIndication(P_TONE, 1000, cls, 0);
            pBlipSystem->getSystemLogger()->logEvent("ROCKET IN AIR");
            isInAir = true;
        }
    } else if (6000 <= millis() - timer && millis() - timer < THRUSTER_WORKING_TIME + 5000) {
        digitalWrite(pin, LOW);
    } else if (THRUSTER_WORKING_TIME + 5000 <= millis() - timer) {
        pBlipSystem->setState(SAVING_STATE);
    }
    */
    thrusterStabilization((double)(millis() - timer) / 1000);
    timer = millis();
    if (millis() - timer >= THRUSTER_WORKING_TIME) {
        // pBlipSystem->setState(SAVING_STATE);
    }
}

void ThrusterState::thrusterStabilization(double dt) {
    #ifndef DONT_USE_THRUSTER_STABILIZATION
    double x_u, z_u, x_e, z_e, x_d, z_d, x_i, z_i, x_p, z_p;
    x_e = -pBlipSystem->getGyroX();
    z_e = -pBlipSystem->getGyroZ();
    x_p = kP * x_e;
    z_p = kP * z_e;
    x_i = last_x_i + kI * x_e * dt;
    z_i = last_z_i + kI * z_e * dt;
    x_d = kD * (x_e - last_x_e) / dt;
    z_d = kD * (z_e - last_z_e) / dt;
    x_u = x_p + x_i + x_d;
    z_u = z_p + z_i + z_d;
    double angleX = constrain(AXIS_X_CENTER + x_u, AXIS_X_MIN, AXIS_X_MAX);
    double angleZ = constrain(AXIS_Z_CENTER + z_u, AXIS_Z_MIN, AXIS_Z_MAX);
    servoX->write(angleX);
    servoZ->write(angleZ);
    Serial.println("servoX: " + (String)angleX + ", uX: " + (String)x_u);
    Serial.println("servoZ: " + (String)angleZ + ", uZ: " + (String)z_u);
    last_x_e = x_e;
    last_z_e = z_e;
    last_x_i = x_i;
    last_z_i = z_i;
    #endif
}
