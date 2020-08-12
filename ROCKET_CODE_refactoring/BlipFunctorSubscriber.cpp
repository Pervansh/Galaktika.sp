#include "BlipFunctorSubscriber.h"

BlipFunctorSubscriber::BlipFunctorSubscriber(blip::IFunctional<void, BlipSystem*, const String&>* func)
 : functional(func) {}

void BlipFunctorSubscriber::update(BlipSystem* system, const String& event) {
    (*functional)(system, event);
}

BlipFunctorSubscriber* blipMakeFuncSub(void (*function)(BlipSystem*, const String&)) {
    return new BlipFunctorSubscriber(blip::toFunctional(function));
}

BlipFunctorSubscriber::~BlipFunctorSubscriber() {
    delete functional;
}
