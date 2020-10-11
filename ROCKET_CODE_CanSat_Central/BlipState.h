#ifndef _BLIP_STATE_BLIP_H_
#define _BLIP_STATE_BLIP_H_

#include "BlipSubscriber.h"

class BlipSystem;
class BlipState;
class BlipEmptyState;
class String;

class BlipState : public BlipSubscriber {
  private:
    static BlipEmptyState* emptyState;
    BlipState* nextState;
    String* id;
  protected:
    BlipSystem* pBlipSystem;
  public:
    BlipState(BlipSystem* pBlipSystem, String id);
    BlipState(BlipSystem* pBlipSystem, String id, BlipState* nextState);
    
    virtual void init() = 0;
    virtual void execute() = 0;

    void next();
    
    virtual void update(BlipSystem* system, const String& event) {}

    virtual void setNextState(BlipState* newState) { nextState = newState; }
    virtual BlipState* getNextState() { return nextState; }
    String getId();

    static BlipEmptyState* getBlipEmptyState() { return emptyState; }

    virtual ~BlipState();
};

class BlipEmptyState : public BlipState {
  public:
    BlipEmptyState(BlipSystem* pBlipSystem);

    void init() {}
    void execute() {}
};

#endif
