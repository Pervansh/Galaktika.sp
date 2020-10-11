#include "ApogeeDetectionState.h"
#include "BlipSystem.h"

ApogeeDetectionState::ApogeeDetectionState(BlipSystem* pBlipSystem, BlipState* nextState = nullptr)
 : ApogeeDetectionState(pBlipSystem, 250, 6, nextState) {}

ApogeeDetectionState::ApogeeDetectionState(BlipSystem* pBlipSystem, unsigned long delta, int apogeeMaxCount, BlipState* nextState = nullptr)
 : BlipState(pBlipSystem, "ApogeeDetectionState", nextState) {
    this->delta = delta;
    this->apogeeMaxCount = apogeeMaxCount;
}

void ApogeeDetectionState::init() {
    timer = millis();
    count = 0;
    lastHeight = pBlipSystem->getHeight();
}

void ApogeeDetectionState::execute() {
    float height = pBlipSystem->getHeight();
    if (millis() - timer >= delta) {
        if (height < lastHeight) {
            count++;
        } else {
            count--;
        }
        count = max(count, 0);
        lastHeight = height;
        timer += delta;

        if (count >= apogeeMaxCount) {
            pBlipSystem->getSystemLogger()->logEvent("ROCKET REACHED APOGEE");
            next();
        }
    }
}
