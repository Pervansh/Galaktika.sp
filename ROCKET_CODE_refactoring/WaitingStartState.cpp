#include "WaitingStartState.h"
#include "BlipSystem.h"
#include "SystemLogger.h"
#include "Arduino.h"

WaitingStartState::WaitingStartState(BlipSystem* pBlipSystem, BlipState* nextState, bool useBmp, long maxAcc) 
 : WaitingStartState(pBlipSystem, useBmp, maxAcc) {
    setNextState(nextState); 
}

WaitingStartState::WaitingStartState(BlipSystem* pBlipSystem, bool useBmp, long maxAcc = 25000)
 : BlipState(pBlipSystem, "WaitingStartState") {
    this->maxAcc = maxAcc;
    #ifdef USE_ONLY_BMP_WHEN_WAITING
        this->useBmp = true;
    #else
        this->useBmp = useBmp;
    #endif
}

void WaitingStartState::init() {
    zeroHeight = pBlipSystem->getHeight();
}

void WaitingStartState::execute() {
    bool bmpCondition = useBmp && pBlipSystem->getHeight() - zeroHeight >= 1;
    bool accCondition = !useBmp && pBlipSystem->getAccY() >= 25000;
    if (bmpCondition || accCondition) {
        pBlipSystem->setState(MIDDLE_AIR);
        bool cls[3] = {false, false, true};
        pBlipSystem->setIndication(P_TONE, 1000, cls, 0);
        pBlipSystem->getImuUnitPointer()->useAccelCorrection(false);
        next();
        //setIndication(b_im, b_tn);
    }
}
