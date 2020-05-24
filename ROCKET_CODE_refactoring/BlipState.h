#ifndef _BLIP_STATE_BLIP_H_
#define _BLIP_STATE_BLIP_H_

//#include "BlipSystem.h"
#include "BlipSubscriber.h"

class BlipSystem;
class BlipEmptyState;
class String;

class BlipState : public BlipSubscriber {
  private:
    static BlipEmptyState* emptyState;
    String* id;
  protected:
    BlipSystem* pBlipSystem;
  public:
    BlipState(BlipSystem* pBlipSystem, String id);
    
    virtual void execute() = 0;
    
    void update(BlipSystem* system, BlipEventType type) {}
    
    String getId();

    static BlipEmptyState* getBlipEmptyState() { return emptyState; }

    virtual ~BlipState();
};

class BlipEmptyState : public BlipState {
  public:
    BlipEmptyState(BlipSystem* pBlipSystem);
    void execute() {}
};

#endif
