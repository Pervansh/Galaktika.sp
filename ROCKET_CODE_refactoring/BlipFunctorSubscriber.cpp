#include "BlipFunctorSubscriber.h"

BlipFunctorSubscriber::BlipFunctorSubscriber(BlipSubFunc func)
 : functor(func) {}

void BlipFunctorSubscriber::update(BlipSystem* system, BlipEventType event) {
    functor(system, event);
}

BlipFunctorSubscriber* blipMakeFuncSub(BlipSubFunc func) {
    return new BlipFunctorSubscriber(func);
}