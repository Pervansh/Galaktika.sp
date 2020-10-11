#include "BlipRangeSigner.h"

#include "Arduino.h"
#include "BlipSystem.h"


BlipRangeSigner::BlipRangeSigner(BlipSubscriber* _subscriber, const String& _eventName, const String& _startStateId, const String& _endStateId) 
: subscriber(_subscriber) {
    eventName = new String(_eventName);
    startStateId = new String(_startStateId);
    endStateId = new String(_endStateId);
    state = -1;
    needDeleting = false;
}

void BlipRangeSigner::init(BlipSystem* system) {
    system->subscribe(this, BlipEventTypes::onBlipStateChange);
}

void BlipRangeSigner::init(BlipSystem* system, bool isSubscriberNeedDeleting) {
    subscriberNeedDeleting(isSubscriberNeedDeleting);
    init(system);
}
    
void BlipRangeSigner::update(BlipSystem* system, const String& event) {
    if (state == -1 && system->getState()->getId() == *startStateId) {
        state = 0;
        Serial.println(">SUBSING<");
        system->subscribe(subscriber, *eventName);
    }
    if (state == 0 && system->getState()->getId() == *endStateId) {
        state = 1;
    }
    if (state == 1 && system->getState()->getId() != *endStateId) {
        system->unsubscribe(subscriber, *eventName);
        system->unsubscribe(this, BlipEventTypes::onBlipStateChange);
        if (needDeleting) {
            delete subscriber;
        }
        delete this;
    }
}

BlipRangeSigner::~BlipRangeSigner() {
    delete startStateId;
    delete endStateId;
    delete eventName;
}
