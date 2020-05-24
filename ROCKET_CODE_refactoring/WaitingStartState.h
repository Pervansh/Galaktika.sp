#pragma once

#include "BlipState.h"

class WaitingStartState : public BlipState {
private:
    long maxAcc;
    bool useBmp;
    double zeroHeight;
public:
    WaitingStartState(BlipSystem* pBlipSystem, bool useBmp = false, long maxAcc = 25000);

    void execute();
};
