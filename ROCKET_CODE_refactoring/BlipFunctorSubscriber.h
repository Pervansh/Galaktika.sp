#pragma once

#include "BlipSubscriber.h"
#include <IFunctional.h>

#define BLIP_SUB_FUNC(system, event) [](BlipSystem* system, const String& event) -> void

class BlipFunctorSubscriber;
class String;
typedef BlipFunctorSubscriber BlipFuncSub;

class BlipFunctorSubscriber : public BlipSubscriber {
private:
    blip::IFunctional<void, BlipSystem*, const String&>* functional;
public:
    virtual BlipFunctorSubscriber(blip::IFunctional<void, BlipSystem*, const String&>* func);
    virtual void update(BlipSystem* system, const String& event);
    virtual ~BlipFunctorSubscriber();
};

BlipFunctorSubscriber* blipMakeFuncSub(void (*function)(BlipSystem*, const String&));

template <typename T>
BlipFunctorSubscriber* blipMakeFuncSub(T* functor) {
    return new BlipFunctorSubscriber(blip::toFunctional< T, void, BlipSystem*, const String& >(functor));
}

template <typename T>
BlipFunctorSubscriber* blipMakeFuncSub(void (T::* memberFunction)(BlipSystem*, const String&), T* object) {
    return new BlipFunctorSubscriber(blip::toFunctional(memberFunction, object));
}
