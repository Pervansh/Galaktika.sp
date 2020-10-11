#include "ThrusterState.h"
#include "Arduino.h"
#include "config.h"

ThrusterState::ThrusterState(BlipSystem* pBlipSystem, BlipState* nextState)
    : BlipState(pBlipSystem, "ThrusterState", nextState) {
    pin = THRUSTER_PIN;
    /*
    pinMode(pin, OUTPUT);
    */
    servoX = pBlipSystem->getServoXPointer();
    servoZ = pBlipSystem->getServoZPointer();
}

void ThrusterState::init() {
    pBlipSystem->getImuUnitPointer()->useAccelCorrection(false);
    bool cls[3] = {true, false, false};
    pBlipSystem->setIndication(500, 0, cls, 100);
    pBlipSystem->getSystemLogger()->logEvent("ROCKET IN AIR");
    timer = millis();
    pidTimer = micros();
    pinMode(pin, OUTPUT);
    #ifdef USE_THRUSTER
        digitalWrite(pin, HIGH);
    #endif
    isInAir = false;
}

void ThrusterState::execute() {
    if (millis() - timer >= 2000) {
        Serial.println("LANDING ENGINE TURN OFF START");
        digitalWrite(pin, LOW);
        Serial.println("LANDING ENGINE TURN OFF END");
    }
        
    thrusterStabilization((double)(micros() - pidTimer) / 1000000);
    pidTimer = micros();
    // timer = millis();
    if (millis() - timer >= THRUSTER_WORKING_TIME) {
        digitalWrite(pin, LOW);
        servoX->write(AXIS_X_CENTER);
        servoZ->write(AXIS_Z_CENTER);
        Serial.println("CHANGE!");
        next();
        Serial.println("POST CHANGE!");
    }
    Serial.println("THRUSTER STATE OUT");
}

void ThrusterState::thrusterStabilization(double dt) {
    #ifndef DONT_USE_THRUSTER_STABILIZATION
    double x_u, z_u, x_e, z_e, x_d, z_d, x_i, z_i, x_p, z_p;
    x_e = X_ERROR_COEFFICIENT * pBlipSystem->getGyroX();
    z_e = Z_ERROR_COEFFICIENT * pBlipSystem->getGyroZ();
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
