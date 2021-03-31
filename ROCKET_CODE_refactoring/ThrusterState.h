#pragma once

#include <inttypes.h>
#include <Servo.h>
#include "BlipState.h"
#include "BlipSystem.h"

class BlipState;
class Servo;

class ThrusterState : public BlipState {
  private:
    bool isInAir;
    uint8_t pin;
    unsigned long timer;
    double kP = 0.555;
    double kI = 0.45;
    double kD = 0.261;
    double last_x_e = 0;
    double last_z_e = 0;
    double last_x_i = 0;
    double last_z_i = 0;
    Servo* servoX;
    Servo* servoZ;

    void thrusterStabilization(double dt);
  public:
    ThrusterState(BlipSystem* pBlipSystem, BlipState* nextState = getBlipEmptyState());

    void init();
    void execute();
};
