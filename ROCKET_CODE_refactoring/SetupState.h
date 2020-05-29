#ifndef _SETUP_STATE_BLIP_H_
#define _SETUP_STATE_BLIP_H_

#include "BlipState.h"

class BlipState;

class SetupState : public BlipState {
  private:
    const int safeSwitchPin;
    int setupState, sumCnt;
    unsigned long setupTimer;
    unsigned long warmingTime;
    unsigned long calibrationTime;
    double zSum, xSum, hSum;
  public:
    SetupState(BlipSystem* pBlipSystem, int sbp, BlipState* nextState = getBlipEmptyState());

    void init() {}
    void execute();

    void reset();
};

#endif
