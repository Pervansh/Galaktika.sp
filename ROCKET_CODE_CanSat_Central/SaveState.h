#pragma once

#include "BlipState.h"
#include "config.h"

class SaveState : public BlipState {
public:
    SaveState(BlipSystem* system, BlipState* nextState = getBlipEmptyState());

    void init();
    void execute();
};
