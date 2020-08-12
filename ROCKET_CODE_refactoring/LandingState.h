#ifndef _LANDING_STATE_BLIP_H_
#define _LANDING_STATE_BLIP_H_

#include "BlipState.h"

class BlipState;

class LandingState : public BlipState {
  private:
    const double MAX_ACCEPTABLE_DELTA;
    int cnt = 0;
    double hMax, hMin;
    unsigned long timerDelta, timer;
  public:
    LandingState(BlipSystem* pBlipSystem, unsigned long _timerDelta = 250, double mad = 0.1);
    LandingState(BlipSystem* pBlipSystem, BlipState* nextState, unsigned long _timerDelta = 250, double mad = 0.1);

    void init() {
      Serial.println("Landing INIT");
    }
    void execute();
};

#endif
