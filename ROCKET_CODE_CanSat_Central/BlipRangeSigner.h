#pragma once

#include "BlipSubscriber.h"

class BlipSystem;
class String;

class BlipRangeSigner : public BlipSubscriber {
private:
    String* startStateId;
    String* endStateId;
    BlipSubscriber* subscriber;
    String* eventName;
    int8_t state;
    bool needDeleting;
public:
    BlipRangeSigner(BlipSubscriber* _subscriber, const String& _eventName, const String& _startStateId, const String& _endStateId);
    
    void init(BlipSystem* system);
    void init(BlipSystem* system, bool isSubscriberNeedDeleting);
    void update(BlipSystem* system, const String& event);

    void subscriberNeedDeleting(bool need) { needDeleting = need; };

    ~BlipRangeSigner();
};
