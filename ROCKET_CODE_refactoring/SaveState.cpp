#include "SaveState.h"
#include "BlipSystem.h"

SaveState::SaveState(BlipSystem* system, BlipState* nextState)
 : BlipState(system, "SaveState", nextState) {}

void SaveState::init() {
      bool cls[3] = {true, true, true};
      pBlipSystem->setIndication(P_TONE, 2000, cls, 2000, 1);
      pBlipSystem->getSystemLogger()->finish();
}

void SaveState::execute() {}