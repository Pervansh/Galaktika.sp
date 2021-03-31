#pragma once

#include <inttypes.h>

class BlipSystem;

#define BLIP_EVENT_TYPES_COUNT 2

enum class BlipEventType : uint8_t {
    onExecution,
    onBlipStateChange
};

class BlipSubscriber {
public:
    BlipSubscriber() {}
    virtual void update(BlipSystem* system, BlipEventType event) = 0;
};
