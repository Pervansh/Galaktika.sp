#pragma once

#include <IFunctional.h>
#include "BlipState.h"

class BlipSystem;

class WaitingState : public BlipState {
    typedef blip::IFunctional<void, BlipSystem*> InitializerFunctional;
    typedef blip::IFunctional<bool, BlipSystem*> ConditionFunctional;
private:
    InitializerFunctional* initializer;
    ConditionFunctional* condition;
public:
    WaitingState(BlipSystem* system, ConditionFunctional* _condition, BlipState* next = BlipState::getBlipEmptyState());
    WaitingState(BlipSystem* system, ConditionFunctional* _condition, InitializerFunctional* _initializer, BlipState* next = BlipState::getBlipEmptyState());

    void init();
    void execute();

    ~WaitingState();
};
