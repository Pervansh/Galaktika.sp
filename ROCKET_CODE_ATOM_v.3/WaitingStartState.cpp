#include "WaitingStartState.h"
#include "BlipSystem.h"
#include "SystemLogger.h"
#include "Arduino.h"

WaitingStartState::WaitingStartState(BlipSystem* pBlipSystem, bool useBmp, long maxAcc = 25000) : BlipState(pBlipSystem) {
    this->maxAcc = maxAcc;
    #ifdef USE_ONLY_BMP_WHEN_WAITING
        this->useBmp = true;
    #else
        this->useBmp = useBmp;
    #endif
    zeroHeight = pBlipSystem->getHeight();
}

void WaitingStartState::execute() {
    bool bmpCondition = useBmp && pBlipSystem->getHeight() - zeroHeight >= 1;
    bool accCondition = !useBmp && pBlipSystem->getAccY() >= 25000;
    if (bmpCondition || accCondition) {
        pBlipSystem->setState(MIDDLE_AIR);
        bool cls[3] = {false, false, true};
        pBlipSystem->setIndication(P_TONE, 1000, cls, 0);
        pBlipSystem->getSystemLogger()->logEvent("ROCKET IN AIR");
        pBlipSystem->getImuUnitPointer()->useAccelCorrection(false);
        //setIndication(b_im, b_tn);
    }
}
