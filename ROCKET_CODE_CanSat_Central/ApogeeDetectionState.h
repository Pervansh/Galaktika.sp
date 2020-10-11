#pragma once;

#include "BlipState.h"
#include "Arduino.h"

class String;

class ApogeeDetectionState : public BlipState {
private:
    unsigned long timer;
    unsigned long delta;
    int count;
    int apogeeMaxCount;
    float lastHeight;
public:
    ApogeeDetectionState(BlipSystem* pBlipSystem, BlipState* nextState = nullptr);
    ApogeeDetectionState(BlipSystem* pBlipSystem, unsigned long delta, int apogeeMaxCount, BlipState* nextState = nullptr);
    void init();
    void execute();

    unsigned long getDelta() { return delta; }
    unsigned long getApogeeMaxCount() { return apogeeMaxCount; }

    void setDelta(unsigned long delta) { this->delta = delta; }
    void setApogeeMaxCount(unsigned long apogeeMaxCount) { this->apogeeMaxCount = apogeeMaxCount; }
};
