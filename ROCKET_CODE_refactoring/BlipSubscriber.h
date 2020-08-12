#pragma once

#include <inttypes.h>

class BlipSystem;
class BlipSubscriber;
class String;
typedef BlipSubscriber BlipSub;

#define BLIP_EVENT_TYPES_COUNT 2

struct BlipEventTypes {
    static const String onExecution;
    static const String onBlipStateChange;

    static uint16_t count() { return BLIP_EVENT_TYPES_COUNT; }
};

class BlipSubscriber {
public:
    BlipSubscriber() {}
    virtual void update(BlipSystem* system, const String& event) = 0;
    virtual ~BlipSubscriber() {};
};
