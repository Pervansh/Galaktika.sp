#pragma once

#include "BlipSubscriber.h"

#define BLIP_SUB_FUNC(system, event) [](BlipSystem* system, BlipEventType event) -> void

typedef void (*BlipSubFunc)(BlipSystem*, BlipEventType);
class BlipFunctorSubscriber;
BlipFunctorSubscriber* blipMakeFuncSub(BlipSubFunc func);


class BlipFunctorSubscriber : public BlipSubscriber {
private:
    BlipSubFunc functor;
public:
    BlipFunctorSubscriber(BlipSubFunc func);
    virtual void update(BlipSystem* system, BlipEventType event);
};
