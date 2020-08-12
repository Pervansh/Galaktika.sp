#pragma once

#include "Arduino.h"

class BlipSubscriber;
class BlipSystem;
class String;

class IEventManager {
public:
    virtual bool find(const String& event) = 0;
    virtual void add(BlipSubscriber* sub, const String& event) = 0;
    virtual void remove(BlipSubscriber* sub, const String& event) = 0;
    virtual void notify(BlipSystem* system, const String& event) = 0;
};
