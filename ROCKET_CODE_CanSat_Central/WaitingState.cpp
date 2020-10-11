#include "WaitingState.h"
#include "Arduino.h"

WaitingState::WaitingState(BlipSystem* system, ConditionFunctional* _condition, BlipState* next)
: BlipState(system, "WaitingState", next), condition(_condition) {
    auto emptyLambda = [](BlipSystem* system){};
    initializer = blip::toFunctional<decltype (emptyLambda), void, BlipSystem*>(&emptyLambda);
}

WaitingState::WaitingState(BlipSystem* system, ConditionFunctional* _condition, InitializerFunctional* _initializer, BlipState* next)
: BlipState(system, "WaitingState", next), condition(_condition), initializer(_initializer) {}

void WaitingState::init() {
    (*initializer)(pBlipSystem);
}

void WaitingState::execute() {
    if ((*condition)(pBlipSystem)) {
        next();
    }
}

WaitingState::~WaitingState() {
    delete initializer;
    delete condition;
}
