#include "BlipSystem.h"
#include "LandingState.h"
#include "Arduino.h"
#include "SaveState.h"

LandingState::LandingState(BlipSystem* pBlipSystem, BlipState* nextState, unsigned long _timerDelta, double mad)
 : LandingState(pBlipSystem, _timerDelta, mad) {
   setNextState(nextState);
 }

LandingState::LandingState(BlipSystem* pBlipSystem, unsigned long _timerDelta = 250, double mad = 0.1)
 : BlipState(pBlipSystem, "LandingState"), MAX_ACCEPTABLE_DELTA(mad) {
  timerDelta = _timerDelta;
  timer = 0;
  hMax = hMin = 0;
}

void LandingState::execute() {
  if (cnt >= 20) {
    if (abs(hMax - hMin) <= MAX_ACCEPTABLE_DELTA) {
      // pBlipSystem->setBlipState(new SaveState(pBlipSystem));
      // pBlipSystem->setState(SAVING_STATE);
      bool cls[3] = {false, true, false};
      pBlipSystem->setIndication(P_TONE, 500, cls, 500);
      pBlipSystem->getSystemLogger()->logEvent("ROCKET WAS LANDED");
      pBlipSystem->getSystemLogger()->finish();
      next();
    }
    cnt = 0;
  }
  else if (millis() - timer >= timerDelta) {
    if (cnt == 0) {
      hMax = hMin = pBlipSystem->getHeight();
    } else {
      hMax = max(hMax, pBlipSystem->getHeight());
      hMin = min(hMin, pBlipSystem->getHeight());
    }
    cnt++;
    timer = millis();
  }
}
